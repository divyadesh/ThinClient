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
    // Connect QProcess finished handler for asynchronous Wi-Fi scans
    connect(&m_asyncProcess,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            [this](int exitCode, QProcess::ExitStatus status)
            {
                if (status != QProcess::NormalExit || exitCode != 0) {
                    qWarning() << "Async scan failed:" << m_asyncProcess.errorString();
                    emit errorMessage(QStringLiteral("Wi-Fi scan failed."));
                    m_scanning = false;
                    emit scanningChanged();
                    return;
                }

                QString output = m_asyncProcess.readAllStandardOutput();
                QStringList lines = output.split('\n', Qt::SkipEmptyParts);

                struct WifiInfo {
                    QString active;
                    QString ssid;
                    int strength;
                    bool secured;
                    QString bssid;
                    int chan;
                    QString rate;
                };

                QVector<WifiInfo> newList;

                for (const QString &line : lines) {
                    QStringList fields = line.split(":");
                    if (fields.size() < 7)
                        continue;

                    WifiInfo info;
                    info.active = fields[0];
                    info.ssid   = fields[1];
                    QString bars = fields[2];
                    QString security = fields[3];
                    info.chan   = fields[4].toInt();
                    info.rate   = fields[5];
                    info.bssid  = fields.size() >= 12
                                     ? QStringList(fields.mid(6, 6)).join(":")
                                     : "";

                    if (info.ssid.isEmpty())
                        continue;

                    if      (bars == "****") info.strength = StrengthExcellent;
                    else if (bars == "*** ") info.strength = StrengthGood;
                    else if (bars == "**  ") info.strength = StrengthFair;
                    else if (bars == "*   ") info.strength = StrengthWeak;
                    else                     info.strength = StrengthNone;

                    info.secured = security.contains("WPA1") || security.contains("WPA2");
                    newList.append(info);
                }

                // ---- Remove old items not in the new scan ----
                QSet<QString> newSsids;
                for (const auto &wifi : newList)
                    newSsids.insert(wifi.ssid);

                for (int i = 0; i < static_cast<int>(m_WifiDetailsColl.size()); ++i) {
                    const auto &existing = m_WifiDetailsColl[i];
                    if (!newSsids.contains(existing->ssid())) {
                        beginRemoveRows(QModelIndex(), i, i);
                        m_WifiDetailsColl.erase(m_WifiDetailsColl.begin() + i);
                        endRemoveRows();
                        --i;
                    }
                }

                // ---- Update existing or insert new ----
                for (const auto &wifi : newList) {
                    auto it = std::find_if(m_WifiDetailsColl.begin(), m_WifiDetailsColl.end(),
                                           [&](const auto &ptr) { return ptr->ssid() == wifi.ssid; });

                    if (it != m_WifiDetailsColl.end()) {
                        // Update existing entry
                        auto &item = *it;
                        bool changed = false;

                        if (item->active() != wifi.active) { item->setActive(wifi.active); changed = true; }
                        if (item->bars() != wifi.strength) { item->setBars(wifi.strength); changed = true; }
                        if (item->security() != wifi.secured) { item->setSecurity(wifi.secured); changed = true; }
                        if (item->bssid() != wifi.bssid) { item->setBssid(wifi.bssid); changed = true; }
                        if (item->chan() != wifi.chan) { item->setChan(wifi.chan); changed = true; }
                        if (item->rate() != wifi.rate) { item->setRate(wifi.rate); changed = true; }

                        if (changed) {
                            int row = std::distance(m_WifiDetailsColl.begin(), it);
                            emit dataChanged(index(row), index(row));
                        }
                    } else {
                        // Insert new entry
                        int row = static_cast<int>(m_WifiDetailsColl.size());
                        beginInsertRows(QModelIndex(), row, row);

                        auto spNew = std::make_shared<WifiNetworkDetails>(
                            this,
                            wifi.active,
                            wifi.ssid,
                            wifi.strength,
                            wifi.secured,
                            wifi.bssid,
                            wifi.chan,
                            wifi.rate
                            );

                        QQmlEngine::setObjectOwnership(spNew.get(), QQmlEngine::CppOwnership);
                        m_WifiDetailsColl.emplace_back(spNew);

                        endInsertRows();
                    }

                    // Track active SSID
                    if (wifi.active == "yes") {
                        setActiveSsid(wifi.ssid);
                        setActiveBars(wifi.strength);
                    }
                }

                emit infoMessage(QStringLiteral("Wi-Fi scan completed."));
                m_scanning = false;
                emit scanningChanged();
                emit sigWifiListUpdated();
            });

    connect(&m_autoRefreshTimer, &QTimer::timeout,
            this, &WifiNetworkDetailsColl::scanWifiNetworksAsync);

    startAutoRefresh();
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
    roles[eWifiListCollectionRole] = "wifiDetails";
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
    return static_cast<int>(m_WifiDetailsColl.size());
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
        static_cast<size_t>(index.row()) >= m_WifiDetailsColl.size()) {
        return {};
    }

    const auto &details = m_WifiDetailsColl.at(index.row());

    switch (role) {
    case eWifiListCollectionRole:
        return QVariant::fromValue(static_cast<QObject*>(details.get()));

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
    m_WifiDetailsColl.clear();
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
