/*!
 *  \file WifiNetworkDetailsColl.cpp
 *  \brief Implementation of WifiNetworkDetailsColl, a model for Wi-Fi networks.
 */

#include "WifiNetworkDetailsColl.h"
#include "WifiNetworkDetails.h"

#include <QQmlEngine>
#include <QRegularExpression>
#include <QDebug>
#include <QSet>
#include <algorithm>

/* -------------------------------------------------------------------------- */
/*                             CONSTRUCTOR / SETUP                            */
/* -------------------------------------------------------------------------- */

/*!
 *  \brief Constructs a WifiNetworkDetailsColl instance.
 *  \param parent Parent QObject.
 *
 *  Sets up the asynchronous scan process, auto-refresh timer, and initial scan.
 */
WifiNetworkDetailsColl::WifiNetworkDetailsColl(QObject *parent)
    : QAbstractListModel{parent}
{
    connect(&m_asyncProcess,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            [this](int exitCode, QProcess::ExitStatus status)
            {
                if (status != QProcess::NormalExit || exitCode != 0) {
                    finalizeScanFailure();
                    return;
                }

                QStringList lines =
                    QString(m_asyncProcess.readAllStandardOutput())
                        .split('\n', Qt::SkipEmptyParts);

                QVector<ParsedInfo> parsed;
                parseScanOutput(lines, parsed);

                // 🔥 MAIN FIX — Remove duplicates BEFORE updating model
                parsed = deduplicateParsedResults(parsed);

                removeMissingEntries(parsed);
                updateOrInsertEntries(parsed);
                updateActiveInfo();
                finalizeScanSuccess();
            });

    connect(&m_autoRefreshTimer, &QTimer::timeout,
            this, &WifiNetworkDetailsColl::scanWifiNetworksAsync);

    startAutoRefresh();
}

/*!
 * \brief Called when a Wi-Fi scan completes successfully.
 */
void WifiNetworkDetailsColl::finalizeScanSuccess()
{
    m_scanning = false;
    emit scanningChanged();
    emit infoMessage("Wi-Fi scan completed.");
    emit sigWifiListUpdated();
}

/*!
 * \brief Called when a Wi-Fi scan fails.
 */
void WifiNetworkDetailsColl::finalizeScanFailure()
{
    m_scanning = false;
    emit scanningChanged();
    emit errorMessage("Wi-Fi scan failed.");
}


/*!
 * \brief Deduplicates parsed scan results based on BSSID (primary) or SSID.
 *
 * If multiple entries share the same BSSID:
 *   - If any entry has ACTIVE="yes", keep that entry.
 *   - If all are "no", keep the one with stronger signal.
 *
 * \param input List with raw parsed scan results (may contain duplicates).
 * \return A cleaned, deduplicated list.
 */
QVector<ParsedInfo> WifiNetworkDetailsColl::deduplicateParsedResults(
    const QVector<ParsedInfo> &input)
{
    QHash<QString, ParsedInfo> map;

    for (const ParsedInfo &p : input) {
        QString key = !p.bssid.isEmpty() ? p.bssid : p.ssid;

        if (!map.contains(key)) {
            map[key] = p;
            continue;
        }

        ParsedInfo &old = map[key];

        // RULE 1 — Prefer ACTIVE=yes
        if (p.active == "yes" && old.active == "no") {
            map[key] = p;
            continue;
        }

        // RULE 2 — If both inactive, keep stronger signal
        if (p.active == "no" && old.active == "no") {
            if (p.bars > old.bars) {
                map[key] = p;
                continue;
            }
        }
    }

    return map.values().toVector();
}

/*!
 *  \brief Parses raw nmcli output lines into a list of ParsedInfo items.
 *
 *  \param lines Lines obtained from nmcli -t device wifi list.
 *  \param parsed Output vector populated with parsed Wi-Fi entries.
 */
void WifiNetworkDetailsColl::parseScanOutput(const QStringList &lines,
                                             QVector<ParsedInfo> &parsed)
{
    for (const QString &line : lines) {
        QStringList f = line.split(":");
        if (f.size() < 7)
            continue;

        ParsedInfo p;
        p.active = f[0];
        p.ssid   = f[1];
        QString barsStr = f[2];
        QString secStr  = f[3];
        p.chan   = f[4].toInt();
        p.rate   = f[5];

        if (f.size() >= 12)
            p.bssid = QStringList(f.mid(6, 6)).join(":");

        if (p.ssid.isEmpty())
            continue;

        if      (barsStr == "****") p.bars = StrengthExcellent;
        else if (barsStr == "*** ") p.bars = StrengthGood;
        else if (barsStr == "**  ") p.bars = StrengthFair;
        else if (barsStr == "*   ") p.bars = StrengthWeak;
        else                        p.bars = StrengthNone;

        p.secured = secStr.contains("WPA", Qt::CaseInsensitive);

        parsed.append(p);
    }
}

/*!
 *  \brief Removes Wi-Fi entries that are no longer visible in the latest scan.
 *
 *  \param parsed Parsed list of currently scanned SSIDs.
 */
/*!
 * \brief Removes model entries whose BSSID/SSID no longer appear in the scan.
 */
void WifiNetworkDetailsColl::removeMissingEntries(const QVector<ParsedInfo> &parsed)
{
    QSet<QString> keys;

    for (const auto &p : parsed)
        keys.insert(!p.bssid.isEmpty() ? p.bssid : p.ssid);

    for (int i = 0; i < m_wifiList.size(); ++i) {
        QString k = !m_wifiList[i]->bssid().isEmpty()
        ? m_wifiList[i]->bssid()
        : m_wifiList[i]->ssid();
        if (!keys.contains(k)) {
            beginRemoveRows(QModelIndex(), i, i);
            m_wifiList.removeAt(i);
            endRemoveRows();
            --i;
        }
    }
}

/*!
 * \brief Updates existing entries or inserts new ones incrementally.
 */
void WifiNetworkDetailsColl::updateOrInsertEntries(const QVector<ParsedInfo> &parsed)
{
    for (const auto &p : parsed) {

        QString key = !p.bssid.isEmpty() ? p.bssid : p.ssid;

        int found = -1;
        for (int i = 0; i < m_wifiList.size(); ++i) {
            QString existingKey = !m_wifiList[i]->bssid().isEmpty()
            ? m_wifiList[i]->bssid()
            : m_wifiList[i]->ssid();
            if (existingKey == key) {
                found = i;
                break;
            }
        }

        if (found >= 0) {
            // ---- Update ----
            WifiNetworkDetails *item = m_wifiList[found];
            bool changed = false;

            if (item->active() != p.active) { item->setActive(p.active); changed = true; }
            if (item->ssid()   != p.ssid)   { item->setSsid(p.ssid);     changed = true; }
            if (item->bars()   != p.bars)   { item->setBars(p.bars);     changed = true; }
            if (item->security()!= p.secured){item->setSecurity(p.secured); changed = true; }
            if (item->bssid()  != p.bssid)  { item->setBssid(p.bssid);   changed = true; }
            if (item->chan()   != p.chan)   { item->setChan(p.chan);     changed = true; }
            if (item->rate()   != p.rate)   { item->setRate(p.rate);     changed = true; }

            if (changed)
                emit dataChanged(index(found), index(found));
        }
        else {
            // ---- Insert new ----
            int row = m_wifiList.size();
            beginInsertRows(QModelIndex(), row, row);

            WifiNetworkDetails *wifi =
                new WifiNetworkDetails(this, p.active, p.ssid, p.bars,
                                       p.secured, p.bssid, p.chan, p.rate);

            m_wifiList.append(wifi);

            endInsertRows();
        }
    }
}



/*!
 *  \brief Updates the active Wi-Fi information (SSID + signal bars)
 *         based on the current list.
 */
void WifiNetworkDetailsColl::updateActiveInfo()
{
    setActiveSsid("");
    setActiveBars(-1);

    for (auto &i : m_wifiList) {
        if (i->active() == "yes") {
            setActiveSsid(i->ssid());
            setActiveBars(i->bars());
        }
    }
}

/* -------------------------------------------------------------------------- */
/*                             MODEL INTERFACE                                */
/* -------------------------------------------------------------------------- */

/*!
 *  \brief Returns the role names used by the model.
 *  \return Mapping of role IDs to role names.
 */
QHash<int, QByteArray> WifiNetworkDetailsColl::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[RoleActive]   = "active";
    roles[RoleSsid]     = "ssid";
    roles[RoleBars]     = "bars";
    roles[RoleSecurity] = "security";
    roles[RoleBssid]    = "bssid";
    roles[RoleChan]     = "chan";
    roles[RoleRate]     = "rate";
    return roles;
}

/*!
 *  \brief Returns the number of rows in the model.
 *  \param parent Parent index (unused for list models).
 *  \return Number of Wi-Fi entries.
 */
int WifiNetworkDetailsColl::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return static_cast<int>(m_wifiList.size());
}

/*!
 *  \brief Returns data for a given index and role.
 *  \param index Model index.
 *  \param role Data role.
 *  \return Requested data wrapped in QVariant.
 */
QVariant WifiNetworkDetailsColl::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() ||
        index.row() < 0 ||
        index.row() >= m_wifiList.size())
    {
        return {};
    }

    WifiNetworkDetails* details = m_wifiList.at(index.row());

    switch (role) {
    case RoleActive:
        return details->active();

    case RoleSsid:
        return details->ssid();

    case RoleBars:
        return details->bars();

    case RoleSecurity:
        return details->security();

    case RoleBssid:
        return details->bssid();

    case RoleChan:
        return details->chan();

    case RoleRate:
        return details->rate();

    default:
        return {};
    }
}

/*!
 *  \brief Clears all Wi-Fi entries from the model and resets active info.
 */
void WifiNetworkDetailsColl::clear()
{
    beginResetModel();
    m_wifiList.clear();
    endResetModel();

    setActiveSsid(QString());
    setActiveBars(-1);

    emit infoMessage(QStringLiteral("Wi-Fi network list cleared."));
}

/* -------------------------------------------------------------------------- */
/*                                ASYNC SCAN                                  */
/* -------------------------------------------------------------------------- */

/*!
 *  \brief Legacy API: triggers the asynchronous Wi-Fi scan.
 *
 *  Keeps backward compatibility by delegating to scanWifiNetworksAsync().
 */
void WifiNetworkDetailsColl::getWifiDetails()
{
    scanWifiNetworksAsync();
}

/*!
 *  \brief Fetches the currently active Wi-Fi details (SSID and bars).
 *
 *  Uses nmcli asynchronously and updates activeSsid and activeBars.
 */
void WifiNetworkDetailsColl::fetchActiveWifiDetails()
{
    // Use QProcess asynchronously (safe on the main thread)
    QProcess *proc = new QProcess(this);

    connect(proc,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            [this, proc](int exitCode, QProcess::ExitStatus status)
            {
                proc->deleteLater();

                if (status != QProcess::NormalExit || exitCode != 0) {
                    qWarning() << "fetchActiveWifiDetails failed:" << proc->errorString();
                    emit errorMessage(QStringLiteral("Failed to fetch active Wi-Fi details."));
                    return;
                }

                QString output = proc->readAllStandardOutput();
                QStringList lines = output.split('\n', Qt::SkipEmptyParts);

                setActiveSsid(QString());
                setActiveBars(-1);

                for (const QString &line : lines) {
                    QStringList fields = line.split(":");
                    if (fields.size() < 3)
                        continue;

                    QString active = fields[0];
                    QString ssid   = fields[1];
                    QString bars   = fields[2];

                    if (ssid.isEmpty()) {
                        continue;
                    }

                    int bar = StrengthNone;
                    if      (bars == "****") bar = StrengthExcellent;
                    else if (bars == "*** ") bar = StrengthGood;
                    else if (bars == "**  ") bar = StrengthFair;
                    else if (bars == "*   ") bar = StrengthWeak;
                    else if (bars == "    ") bar = StrengthNone;

                    if (active == "yes") {
                        setActiveSsid(ssid);
                        setActiveBars(bar);
                    }
                }
            });

    // Run nmcli to list active Wi-Fi networks
    proc->start("nmcli", QStringList() << "-t" << "-f" << "ACTIVE,SSID,BARS" << "dev" << "wifi" << "list");

    startAutoRefresh();
}

/* -------------------------------------------------------------------------- */
/*                            CONNECT / DISCONNECT                            */
/* -------------------------------------------------------------------------- */

/*!
 *  \brief Connects to a Wi-Fi network using SSID and password.
 *  \param ssid Target Wi-Fi network SSID.
 *  \param password Password for the given SSID.
 */
void WifiNetworkDetailsColl::connectToSsid(QString ssid, QString password)
{
    emit infoMessage(QString("Connecting to Wi-Fi network: %1").arg(ssid));
    emit sigConnectionStarted();

    QProcess *proc = new QProcess(this);

    connect(proc,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            [this, proc, ssid](int exitCode, QProcess::ExitStatus exitStatus)
            {
                QString output = proc->readAllStandardOutput();
                QString errorOutput = proc->readAllStandardError();

                qDebug() << "[ConnectWiFi] Command Output:\n" << output;
                qDebug() << "[ConnectWiFi] Error Output:\n" << errorOutput;

                // Success case
                if (exitStatus == QProcess::NormalExit && exitCode == 0) {
                    qDebug() << "[ConnectWiFi] Successfully connected to:" << ssid;
                    emit successMessage(QString("Connected to Wi-Fi network \"%1\" successfully.").arg(ssid));
                } else {
                    qWarning() << "[ConnectWiFi] Failed to connect to:" << ssid
                               << "Exit:" << exitCode
                               << "Status:" << exitStatus;

                    // More precise user message based on nmcli output:
                    if (errorOutput.contains("No network with SSID", Qt::CaseInsensitive)) {
                        emit errorMessage(QString("Network \"%1\" not found.").arg(ssid));
                    } else if (errorOutput.contains("incorrect password", Qt::CaseInsensitive) ||
                               errorOutput.contains("wrong password", Qt::CaseInsensitive)) {
                        emit errorMessage(QString("Incorrect password for Wi-Fi \"%1\".").arg(ssid));
                    } else {
                        emit errorMessage(QString("Failed to connect to Wi-Fi \"%1\".").arg(ssid));
                    }

                    emit warningMessage(QStringLiteral("Wi-Fi connection attempt failed."));
                }

                emit sigConnectionFinished();
                proc->deleteLater();
                fetchActiveWifiDetails();
            });

    // Start nmcli connection attempt
    proc->start("nmcli",
                {"device", "wifi",
                 "connect", ssid,
                 "password", password});
}

/*!
 *  \brief Disconnects from the specified Wi-Fi network.
 *  \param ssid SSID of the Wi-Fi network to disconnect from.
 */
void WifiNetworkDetailsColl::disconnectWifiNetwork(const QString &ssid)
{
    emit infoMessage(QString("Disconnecting from Wi-Fi network: %1").arg(ssid));

    QProcess *proc = new QProcess(this);

    connect(proc,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            [this, proc, ssid](int exitCode, QProcess::ExitStatus exitStatus)
            {
                QString output = proc->readAllStandardOutput();
                QString errorOutput = proc->readAllStandardError();

                qDebug() << "[DisconnectWiFi] Command Output:\n" << output;
                if (!errorOutput.isEmpty())
                    qWarning() << "[DisconnectWiFi] Error Output:\n" << errorOutput;

                // Normal successful disconnection
                if (exitStatus == QProcess::NormalExit && exitCode == 0) {
                    qDebug() << "[DisconnectWiFi] Successfully disconnected from:" << ssid;
                    emit successMessage(QString("Disconnected from Wi-Fi network \"%1\".").arg(ssid));
                } else {
                    qWarning() << "[DisconnectWiFi] Failed to disconnect from:" << ssid
                               << "ExitCode:" << exitCode
                               << "Status:" << exitStatus;

                    emit warningMessage(QString("Wi-Fi disconnection may have failed: \"%1\".").arg(ssid));
                    emit errorMessage(QString("Failed to disconnect from Wi-Fi network \"%1\".").arg(ssid));
                }

                proc->deleteLater();
                fetchActiveWifiDetails();
            });

    // Execute: nmcli connection down "<SSID>"
    proc->start("nmcli", {"connection", "down", ssid});
}

/*!
 *  \brief Forgets the specified Wi-Fi network by deleting its connection profile.
 *  \param ssid SSID of the Wi-Fi network to forget.
 */
void WifiNetworkDetailsColl::forgetWifiNetwork(const QString &ssid)
{
    emit infoMessage(QString("Forgetting Wi-Fi network: %1").arg(ssid));

    auto *proc = new QProcess(this);

    connect(proc,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            [this, proc, ssid](int exitCode, QProcess::ExitStatus status)
            {
                QString output = proc->readAllStandardOutput();
                QString errorOutput = proc->readAllStandardError();

                qDebug() << "[ForgetNetwork] Output:\n" << output;
                if (!errorOutput.isEmpty())
                    qWarning() << "[ForgetNetwork] Error:\n" << errorOutput;

                if (status == QProcess::NormalExit && exitCode == 0) {
                    emit successMessage(QString("Wi-Fi network \"%1\" has been forgotten.").arg(ssid));
                    qDebug() << "[ForgetNetwork] Successfully deleted Wi-Fi profile:" << ssid;
                } else {
                    emit errorMessage(QString("Failed to forget Wi-Fi network \"%1\".").arg(ssid));
                    qWarning() << "[ForgetNetwork] Failed to delete profile:" << ssid;
                }

                proc->deleteLater();
                fetchActiveWifiDetails();
            });

    // Forget the network → remove saved profile
    // Equivalent to: nmcli connection delete "<SSID>"
    proc->start("nmcli", {"connection", "delete", ssid});
}

/* -------------------------------------------------------------------------- */
/*                             AUTO REFRESH / ASYNC                           */
/* -------------------------------------------------------------------------- */

/*!
 *  \brief Starts an asynchronous Wi-Fi scan using nmcli.
 *
 *  If a scan is already running, the request is skipped.
 */
void WifiNetworkDetailsColl::scanWifiNetworksAsync()
{
    // Prevent multiple scans overlapping
    if (m_asyncProcess.state() != QProcess::NotRunning) {
        qDebug() << "[WiFiScanner] Scan already in progress — skipping new request.";
        emit infoMessage(QStringLiteral("Wi-Fi scan already in progress."));
        return;
    }

    emit infoMessage(QStringLiteral("Starting Wi-Fi scan..."));

    QStringList args = {
        "-t", "-f",
        "ACTIVE,SSID,BARS,SECURITY,CHAN,RATE,BSSID",
        "device", "wifi", "list"
    };

    m_asyncProcess.start("nmcli", args);
}

/*!
 *  \brief Starts automatic periodic Wi-Fi scanning.
 *  \param intervalMs Scan interval in milliseconds.
 */
void WifiNetworkDetailsColl::startAutoRefresh(int intervalMs)
{
    if (m_autoRefreshTimer.isActive()) {
        qDebug() << "[WiFiScanner] Auto-refresh is already active.";
        return;
    }

    m_autoRefreshTimer.start(intervalMs);

    qDebug() << "[WiFiScanner] Auto-refresh enabled."
             << "Interval:" << intervalMs << "ms";

    // Trigger first scan immediately
    scanWifiNetworksAsync();
}

/*!
 *  \brief Stops automatic periodic Wi-Fi scanning.
 */
void WifiNetworkDetailsColl::stopAutoRefresh()
{
    if (!m_autoRefreshTimer.isActive()) {
        qDebug() << "[WiFiScanner] Auto-refresh is not currently running.";
        return;
    }

    m_autoRefreshTimer.stop();
    qDebug() << "[WiFiScanner] Auto-refresh disabled.";
}

/* -------------------------------------------------------------------------- */
/*                             GETTERS / SETTERS                              */
/* -------------------------------------------------------------------------- */

/*!
 *  \brief Returns the active SSID.
 *  \return Current active SSID string.
 */
QString WifiNetworkDetailsColl::activeSsid() const
{
    return m_activeSsid;
}

/*!
 *  \brief Sets the active SSID.
 *  \param newActiveSsid New SSID value.
 */
void WifiNetworkDetailsColl::setActiveSsid(const QString &newActiveSsid)
{
    if (m_activeSsid == newActiveSsid)
        return;

    m_activeSsid = newActiveSsid;
    emit sigActiveSsidChanged(m_activeSsid);
}

/*!
 *  \brief Returns the signal strength bars for the active Wi-Fi.
 *  \return Integer representing signal bars.
 */
int WifiNetworkDetailsColl::activeBars() const
{
    return m_activeBars;
}

/*!
 *  \brief Sets the signal strength bars for the active Wi-Fi.
 *  \param newActiveBars New bars value.
 */
void WifiNetworkDetailsColl::setActiveBars(const int &newActiveBars)
{
    if (m_activeBars == newActiveBars)
        return;

    m_activeBars = newActiveBars;
    emit sigActiveBarsChanged(m_activeBars);
}

/*!
 *  \brief Indicates whether an asynchronous Wi-Fi scan is in progress.
 *  \return True if scanning, otherwise false.
 */
bool WifiNetworkDetailsColl::scanning() const
{
    return m_scanning;
}

/*!
 *  \brief Returns the active Wi-Fi device (interface name).
 *  \return Device name (e.g. "wlan0") or empty string if not found.
 */
QString WifiNetworkDetailsColl::getActiveWifiDevice()
{
    QProcess p;
    p.start("sh", QStringList() << "-c"
                                << "nmcli -t -f DEVICE,TYPE,STATE device status | grep ':wifi:connected' | cut -d: -f1");
    p.waitForFinished();

    QString dev = QString(p.readAllStandardOutput()).trimmed();
    qDebug() << "[WifiNetworkDetailsColl] Active WiFi device:" << dev;

    return dev;
}

/*!
 *  \brief Disconnects the active Wi-Fi device using nmcli.
 *  \return True if a disconnect was attempted, false if no device was found.
 */
bool WifiNetworkDetailsColl::disconnectWifi()
{
    QString device = getActiveWifiDevice();

    if (device.isEmpty()) {
        qWarning() << "[WifiNetworkDetailsColl] No active WiFi device found.";
        emit warningMessage(QStringLiteral("No active Wi-Fi device found to disconnect."));
        return false;
    }

    QString cmd = QString("nmcli device disconnect %1").arg(device);

    QProcess p;
    p.start("sh", QStringList() << "-c" << cmd);
    p.waitForFinished();

    QString out = p.readAllStandardError() + p.readAllStandardOutput();
    qDebug() << "[WifiNetworkDetailsColl] Disconnect result:" << out.trimmed();

    emit infoMessage(QStringLiteral("Wi-Fi device disconnect command executed."));
    return true;
}
