#include "ServerInfoColl.h"
#include "ServerInfo.h"
#include <QQmlEngine>
#include <memory>

ServerInfoColl::ServerInfoColl(QObject *parent)
    : QAbstractListModel{parent}
{}

QHash<int, QByteArray> ServerInfoColl::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[eServerInfoCollectionRole] = "serverInformation";
    return roles;
}

int ServerInfoColl::rowCount([[maybe_unused]] const QModelIndex &refModelIndex) const {
    return static_cast<int>(m_ServerInfoColl.size());
}

QVariant ServerInfoColl::data(const QModelIndex &index, int role) const {
    if((int)index.row() >= 0 && (std::size_t)index.row() < m_ServerInfoColl.size()) {
        switch(role) {
        case eServerInfoCollectionRole: {
            return QVariant::fromValue(static_cast<QObject*>(m_ServerInfoColl.at(index.row()).get()));
        }
        }
    }
    return QVariant {};
}

int ServerInfoColl::getIndexToBeRemoved(QString connectionName, QString serverIp) {
    int index = -1;
    for(const auto& spServerInfo: m_ServerInfoColl) {
        ++index;
        if(spServerInfo->connectionName() == connectionName && spServerInfo->serverIp() == serverIp)
            return index;
    }
    return -1;
}

void ServerInfoColl::setServerInfo(QString connectionName, QString serverIp) {
    beginInsertRows(QModelIndex{}, static_cast<int>(m_ServerInfoColl.size()), static_cast<int>(m_ServerInfoColl.size()));
    std::shared_ptr<ServerInfo> spServerInfo = std::make_shared<ServerInfo>(this, connectionName, serverIp);
    if(spServerInfo) {
        QQmlEngine::setObjectOwnership(spServerInfo.get(), QQmlEngine::CppOwnership);
        m_ServerInfoColl.emplace_back(spServerInfo);
    }
    endInsertRows();
}

void ServerInfoColl::removeConnection(QString connectionName, QString serverIp) {
    int index = getIndexToBeRemoved(connectionName, serverIp);
    if (index < 0 || index >= static_cast<int>(m_ServerInfoColl.size()))
        return;

    beginRemoveRows(QModelIndex(), index, index);
    m_ServerInfoColl.erase(m_ServerInfoColl.begin() + index);
    endRemoveRows();
}
