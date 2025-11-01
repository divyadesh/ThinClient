#include "ServerInfo.h"

ServerInfo::ServerInfo(const QString &connId, QObject *parent)
    : QObject{parent}
    ,m_connectionId{connId}
{
}

QString ServerInfo::connectionId() const
{
    return m_connectionId;
}

void ServerInfo::setConnectionId(const QString &newConnectionId)
{
    if (m_connectionId == newConnectionId)
        return;
    m_connectionId = newConnectionId;
    emit connectionIdChanged();
}
