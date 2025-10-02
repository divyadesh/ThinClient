#include "WifiNetworkDetailsColl.h"
#include "WifiNetworkDetails.h"
#include <QQmlEngine>

WifiNetworkDetailsColl::WifiNetworkDetailsColl(QObject *parent)
    : QAbstractListModel{parent}
{}

QHash<int, QByteArray> WifiNetworkDetailsColl::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[eWifiListCollectionRole] = "wifiDetails";
    return roles;
}

int WifiNetworkDetailsColl::rowCount([[maybe_unused]] const QModelIndex &refModelIndex) const {
    return static_cast<int>(m_WifiDetailsColl.size());
}

QVariant WifiNetworkDetailsColl::data(const QModelIndex &index, int role) const {
    if((int)index.row() >= 0 && (std::size_t)index.row() < m_WifiDetailsColl.size()) {
        switch(role) {
        case eWifiListCollectionRole: {
            return QVariant::fromValue(static_cast<QObject*>(m_WifiDetailsColl.at(index.row()).get()));
        }
        }
    }
    return QVariant {};
}

void WifiNetworkDetailsColl::clear() {
    beginResetModel(); // Notify views that the model is about to change
    m_WifiDetailsColl.clear(); // Actually clear the data
    endResetModel(); // Notify views that the model has been reset
    setActiveSsid("");
    setActiveBars(-1);
}

void WifiNetworkDetailsColl::getWifiDetails() {
    clear();
    m_process.start("nmcli", QStringList() << "-t" << "-f" << "ACTIVE,SSID,BARS" << "dev" << "wifi" << "list");
    m_process.waitForFinished();
    QString output = m_process.readAllStandardOutput();
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);

    for (const QString &line : lines) {
        QStringList fields = line.split(":");
        if (fields.size() >= 3) {
            QString active = fields[0];
            QString ssid   = fields[1];
            QString bars   = fields[2];
            int bar = -1;
            if(bars == "▂▄▆█")
                bar = StrengthExcellent;
            else if(bars == "▂▄▆_")
                bar = StrengthGood;
            else if(bars == "▂▄__")
                bar = StrengthFair;
            else if(bars == "▂___")
                bar = StrengthWeak;
            else if(bars == "____")
                bar = StrengthNone;
            if(active == "yes") {
                setActiveSsid(ssid);
                setActiveBars(bar);
            }
            qDebug() <<"ACTIVE:"<<active <<", SSID:" << ssid << ", Signal Bars:" << bars;
            beginInsertRows(QModelIndex{}, static_cast<int>(m_WifiDetailsColl.size()), static_cast<int>(m_WifiDetailsColl.size()));
            std::shared_ptr<WifiNetworkDetails> spNewWifiNetwork = std::make_shared<WifiNetworkDetails>(this, active, ssid, bar);
            if(spNewWifiNetwork) {
                QQmlEngine::setObjectOwnership(spNewWifiNetwork.get(), QQmlEngine::CppOwnership);
                m_WifiDetailsColl.emplace_back(spNewWifiNetwork);
            }
            endInsertRows();
        }
    }
}

void WifiNetworkDetailsColl::fetchActiveWifiDetails() {
    setActiveSsid("");
    setActiveBars(-1);
    m_process.start("nmcli", QStringList() << "-t" << "-f" << "ACTIVE,SSID,BARS" << "dev" << "wifi" << "list");
    m_process.waitForFinished();
    QString output = m_process.readAllStandardOutput();
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);

    for (const QString &line : lines) {
        QStringList fields = line.split(":");
        if (fields.size() >= 3) {
            QString active = fields[0];
            QString ssid   = fields[1];
            QString bars   = fields[2];
            int bar = -1;
            if(bars == "▂▄▆█")
                bar = StrengthExcellent;
            else if(bars == "▂▄▆_")
                bar = StrengthGood;
            else if(bars == "▂▄__")
                bar = StrengthFair;
            else if(bars == "▂___")
                bar = StrengthWeak;
            else if(bars == "____")
                bar = StrengthNone;
            if(active == "yes") {
                setActiveSsid(ssid);
                setActiveBars(bar);
            }
            qDebug() <<"ACTIVE:"<<active <<", SSID:" << ssid << ", Signal Bars:" << bars;
        }
    }
}

void WifiNetworkDetailsColl::connectToSsid(QString ssid, QString password) {
    //nmcli dev wifi connect "Home WiFi"
    QString program = "nmcli";
    QStringList arguments;
    arguments << "dev" << "wifi" << "connect" << ssid << "password" << password;

    m_process.start(program, arguments);
    m_process.waitForFinished();

    QString output      = m_process.readAllStandardOutput();
    QString errorOutput = m_process.readAllStandardError();

    qDebug() << "connectToSsid() Command Output:\n" << output;
    if (!errorOutput.isEmpty()) {
        qDebug() << "Error Output:\n" << errorOutput;
    }
    fetchActiveWifiDetails();
}

void WifiNetworkDetailsColl::disconnectWifiNetwork(const QString& ssid) {
    QString program = "nmcli";
    QStringList arguments;
    arguments << "con" << "down" << ssid;

    m_process.start(program, arguments);
    m_process.waitForFinished();

    QString output = m_process.readAllStandardOutput();
    QString error = m_process.readAllStandardError();
    qDebug() << "Output:" << output;
    qDebug() << "Error:" << error;
    fetchActiveWifiDetails();
}

/*QString WifiNetworkDetailsColl::getActiveSsid() {
    for(auto& spWifiNetworkDetail : m_WifiDetailsColl) {
        if(spWifiNetworkDetail->active() == "yes")
            return spWifiNetworkDetail->ssid();
    }
    return QString("");
}

QString WifiNetworkDetailsColl::getActiveBars() {
    for(auto& spWifiNetworkDetail : m_WifiDetailsColl) {
        if(spWifiNetworkDetail->active() == "yes")
            return spWifiNetworkDetail->bars();
    }
    return QString("");
}*/

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
