#include "WifiNetworkDetailsColl.h"
#include "WifiNetworkDetails.h"
#include <QQmlEngine>
#include <QRegularExpression>
#include <QDebug>
#include <algorithm>

WifiNetworkDetailsColl::WifiNetworkDetailsColl(QObject *parent)
    : QAbstractListModel{parent}
{
    // ✅ Connect QProcess finished handler
    connect(&m_asyncProcess,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            [this](int exitCode, QProcess::ExitStatus status) {
                if (status != QProcess::NormalExit || exitCode != 0) {
                    qWarning() << "Async scan failed:" << m_asyncProcess.errorString();
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
                                           [&](const auto &ptr){ return ptr->ssid() == wifi.ssid; });

                    if (it != m_WifiDetailsColl.end()) {
                        // ✅ Update existing entry
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
                        // ✅ Insert new entry
                        int row = m_WifiDetailsColl.size();
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

                    // ✅ Track active SSID
                    if (wifi.active == "yes") {
                        setActiveSsid(wifi.ssid);
                        setActiveBars(wifi.strength);
                    }
                }

                m_scanning = false;
                emit scanningChanged();
                emit sigWifiListUpdated();
            });

    connect(&m_autoRefreshTimer, &QTimer::timeout,
            this, &WifiNetworkDetailsColl::getWifiDetailsAsync);

    startAutoRefresh();
}

QHash<int, QByteArray> WifiNetworkDetailsColl::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[eWifiListCollectionRole] = "wifiDetails";
    return roles;
}

int WifiNetworkDetailsColl::rowCount([[maybe_unused]] const QModelIndex &refModelIndex) const {
    return static_cast<int>(m_WifiDetailsColl.size());
}

QVariant WifiNetworkDetailsColl::data(const QModelIndex &index, int role) const {
    if (index.row() >= 0 && static_cast<size_t>(index.row()) < m_WifiDetailsColl.size()) {
        if (role == eWifiListCollectionRole)
            return QVariant::fromValue(static_cast<QObject*>(m_WifiDetailsColl.at(index.row()).get()));
    }
    return QVariant{};
}

void WifiNetworkDetailsColl::clear() {
    beginResetModel();
    m_WifiDetailsColl.clear();
    endResetModel();
    setActiveSsid("");
    setActiveBars(-1);
}

/* -------------------------------------------------------------------------- */
/*                                ASYNC SCAN                                  */
/* -------------------------------------------------------------------------- */

void WifiNetworkDetailsColl::getWifiDetails() {
    // Just trigger the async one-shot version for backward compatibility
    getWifiDetailsAsync();
}

void WifiNetworkDetailsColl::fetchActiveWifiDetails()
{
    // Use QProcess asynchronously (safe on the main thread)
    QProcess *proc = new QProcess(this);

    // ✅ FIX: Disambiguate overloaded signal using QOverload
    connect(proc,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            [this, proc](int exitCode, QProcess::ExitStatus status) {
                proc->deleteLater();

                if (status != QProcess::NormalExit || exitCode != 0) {
                    qWarning() << "fetchActiveWifiDetails failed:" << proc->errorString();
                    return;
                }

                QString output = proc->readAllStandardOutput();
                QStringList lines = output.split('\n', Qt::SkipEmptyParts);

                setActiveSsid("");
                setActiveBars(-1);

                for (const QString &line : lines) {
                    QStringList fields = line.split(":");
                    if (fields.size() < 3)
                        continue;

                    QString active = fields[0];
                    QString ssid   = fields[1];
                    QString bars   = fields[2];

                    if(ssid.isEmpty()) {
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

void WifiNetworkDetailsColl::connectToSsid(QString ssid, QString password)
{
    QProcess *proc = new QProcess(this);

    // ✅ Use QOverload to specify correct QProcess::finished overload
    connect(proc,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            [this, proc](int exitCode, QProcess::ExitStatus exitStatus) {
                QString output = proc->readAllStandardOutput();
                QString errorOutput = proc->readAllStandardError();

                qDebug() << "connectToSsid() Output:\n" << output;
                if (!errorOutput.isEmpty())
                    qDebug() << "Error:\n" << errorOutput;

                if (exitStatus == QProcess::NormalExit && exitCode == 0)
                    qDebug() << "Wi-Fi connection successful.";
                else
                    qWarning() << "Wi-Fi connection failed.";

                emit sigConnectionFinished();
                proc->deleteLater();
                fetchActiveWifiDetails();
            });

    emit sigConnectionStarted();

    // Start nmcli connection command
    proc->start("nmcli", QStringList()
                             << "dev" << "wifi"
                             << "connect" << ssid
                             << "password" << password);
}

void WifiNetworkDetailsColl::disconnectWifiNetwork(const QString &ssid)
{
    QProcess *proc = new QProcess(this);

    // ✅ Again, use QOverload to disambiguate the signal
    connect(proc,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            [this, proc](int exitCode, QProcess::ExitStatus exitStatus) {
                QString output = proc->readAllStandardOutput();
                QString errorOutput = proc->readAllStandardError();

                qDebug() << "disconnectWifiNetwork() Output:\n" << output;
                if (!errorOutput.isEmpty())
                    qDebug() << "Error:\n" << errorOutput;

                if (exitStatus == QProcess::NormalExit && exitCode == 0)
                    qDebug() << "Wi-Fi disconnected successfully.";
                else
                    qWarning() << "Wi-Fi disconnection failed.";

                proc->deleteLater();
                fetchActiveWifiDetails();
            });

    // Start nmcli disconnect command
    proc->start("nmcli", QStringList() << "con" << "down" << ssid);
}

/* -------------------------------------------------------------------------- */
/*                             AUTO REFRESH / ASYNC                           */
/* -------------------------------------------------------------------------- */

void WifiNetworkDetailsColl::getWifiDetailsAsync() {
    if (m_asyncProcess.state() != QProcess::NotRunning) {
        qDebug() << "Scan already running — skipping this cycle.";
        return;
    }

    QStringList args = { "-t", "-f", "ACTIVE,SSID,BARS,SECURITY,CHAN,RATE,BSSID", "dev", "wifi", "list" };
    m_asyncProcess.start("nmcli", args);
}

void WifiNetworkDetailsColl::startAutoRefresh(int intervalMs) {
    if (m_autoRefreshTimer.isActive()) {
        qDebug() << "Auto-refresh already running.";
        return;
    }

    m_autoRefreshTimer.start(intervalMs);
    qDebug() << "Wi-Fi auto-refresh started every" << intervalMs << "ms.";
    getWifiDetailsAsync(); // first scan immediately
}

void WifiNetworkDetailsColl::stopAutoRefresh() {
    if (m_autoRefreshTimer.isActive()) {
        m_autoRefreshTimer.stop();
        qDebug() << "Wi-Fi auto-refresh stopped.";
    }
}

/* -------------------------------------------------------------------------- */
/*                             GETTERS / SETTERS                              */
/* -------------------------------------------------------------------------- */

QString WifiNetworkDetailsColl::activeSsid() const {
    return m_activeSsid;
}

void WifiNetworkDetailsColl::setActiveSsid(const QString &newActiveSsid) {
    if (m_activeSsid == newActiveSsid)
        return;
    m_activeSsid = newActiveSsid;
    emit sigActiveSsidChanged(m_activeSsid);
}

int WifiNetworkDetailsColl::activeBars() const {
    return m_activeBars;
}

void WifiNetworkDetailsColl::setActiveBars(const int &newActiveBars) {
    if (m_activeBars == newActiveBars)
        return;
    m_activeBars = newActiveBars;
    emit sigActiveBarsChanged(m_activeBars);
}

bool WifiNetworkDetailsColl::scanning() const
{
    return m_scanning;
}

QString WifiNetworkDetailsColl::getActiveWifiDevice()
{
    QProcess p;
    p.start("sh", QStringList() << "-c" <<
                      "nmcli -t -f DEVICE,TYPE,STATE device status | grep ':wifi:connected' | cut -d: -f1");
    p.waitForFinished();

    QString dev = QString(p.readAllStandardOutput()).trimmed();
    qDebug() << "[WifiNetworkDetailsColl] Active WiFi device:" << dev;

    return dev;
}

bool WifiNetworkDetailsColl::disconnectWifi()
{
    QString device = getActiveWifiDevice();

    if (device.isEmpty()) {
        qWarning() << "[WifiNetworkDetailsColl] No active WiFi device found.";
        return false;
    }

    QString cmd = QString("nmcli device disconnect %1").arg(device);

    QProcess p;
    p.start("sh", QStringList() << "-c" << cmd);
    p.waitForFinished();

    QString out = p.readAllStandardError() + p.readAllStandardOutput();
    qDebug() << "[WifiNetworkDetailsColl] Disconnect result:" << out.trimmed();

    return true;
}

