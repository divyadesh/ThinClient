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

void WifiNetworkDetailsColl::getWifiDetails() {
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
            if(active == "yes") {
                setActiveSsid(ssid);
                setActiveBars(bars);
            }
            qDebug() <<"ACTIVE:"<<active <<", SSID:" << ssid << ", Signal Bars:" << bars;
            beginInsertRows(QModelIndex{}, static_cast<int>(m_WifiDetailsColl.size()), static_cast<int>(m_WifiDetailsColl.size()));
            std::shared_ptr<WifiNetworkDetails> spNewWifiNetwork = std::make_shared<WifiNetworkDetails>(this, active, ssid, bars);//{ new WifiNetworkDetails(this, active, ssid, bars) };
            if(spNewWifiNetwork) {
                QQmlEngine::setObjectOwnership(spNewWifiNetwork.get(), QQmlEngine::CppOwnership);
                m_WifiDetailsColl.emplace_back(spNewWifiNetwork);
            }
            endInsertRows();
        }
    }
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

QString WifiNetworkDetailsColl::activeBars() const {
    return m_activeBars;
}

void WifiNetworkDetailsColl::setActiveBars(const QString &newActiveBars) {
    if (m_activeBars == newActiveBars)
        return;
    m_activeBars = newActiveBars;
    emit sigActiveBarsChanged(m_activeBars);
}
