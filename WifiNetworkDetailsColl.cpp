#include "WifiNetworkDetailsColl.h"
#include "WifiNetworkDetails.h"
#include <QQmlEngine>
#include <QRegularExpression>
#include <QDebug>

WifiNetworkDetailsColl::WifiNetworkDetailsColl(QObject *parent)
    : QAbstractListModel{parent}
{
    // ✅ FIX: Use QOverload to disambiguate QProcess::finished
    connect(&m_asyncProcess,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            [this](int exitCode, QProcess::ExitStatus status) {
                if (status != QProcess::NormalExit || exitCode != 0) {
                    qWarning() << "Async scan failed:" << m_asyncProcess.errorString();
                    return;
                }

                QString output = m_asyncProcess.readAllStandardOutput();
                QStringList lines = output.split('\n', Qt::SkipEmptyParts);

                beginResetModel();
                m_WifiDetailsColl.clear();
                endResetModel();

                for (const QString &line : lines) {
                    QStringList fields = line.split(":");
                    if (fields.size() < 7)
                        continue;

                    QString active   = fields[0];
                    QString ssid     = fields[1];
                    QString bars     = fields[2];
                    QString security = fields[3];
                    int chan         = fields[4].toInt();
                    QString rate     = fields[5];
                    QString bssid    = fields.size() >= 12
                                        ? QStringList(fields.mid(6, 6)).join(":")
                                        : "";

                    if(ssid.isEmpty()) {
                        continue;
                    }

                    int bar = StrengthNone;
                    if      (bars == "▂▄▆█") bar = StrengthExcellent;
                    else if (bars == "▂▄▆_") bar = StrengthGood;
                    else if (bars == "▂▄__") bar = StrengthFair;
                    else if (bars == "▂___") bar = StrengthWeak;
                    else if (bars == "____") bar = StrengthNone;

                    if (active == "yes") {
                        setActiveSsid(ssid);
                        setActiveBars(bar);
                    }

                    bool isSecured = security.contains("WPA1") || security.contains("WPA2");

                    beginInsertRows(QModelIndex{}, m_WifiDetailsColl.size(), m_WifiDetailsColl.size());
                    auto spNewWifiNetwork = std::make_shared<WifiNetworkDetails>(
                        this, active, ssid, bar, isSecured, bssid, chan, rate);
                    QQmlEngine::setObjectOwnership(spNewWifiNetwork.get(), QQmlEngine::CppOwnership);
                    m_WifiDetailsColl.emplace_back(spNewWifiNetwork);
                    endInsertRows();
                }

                emit sigWifiListUpdated();
            });

    connect(&m_autoRefreshTimer, &QTimer::timeout, this, &WifiNetworkDetailsColl::getWifiDetailsAsync);
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
                    if      (bars == "▂▄▆█") bar = StrengthExcellent;
                    else if (bars == "▂▄▆_") bar = StrengthGood;
                    else if (bars == "▂▄__") bar = StrengthFair;
                    else if (bars == "▂___") bar = StrengthWeak;

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
