#include "ServerInfo.h"

ServerInfo::ServerInfo(QObject *parent, QString connectionName, QString serverIp, bool autoEnable)
    : QObject{parent} {
    setConnectionName(connectionName);
    setServerIp(serverIp);
    setAutoEnable(autoEnable);
}

QString ServerInfo::connectionName() const
{
    return m_connectionName;
}

void ServerInfo::setConnectionName(const QString &newConnectionName)
{
    if (m_connectionName == newConnectionName)
        return;
    m_connectionName = newConnectionName;
    emit sigConnectionNameChanged(m_connectionName);
}

QString ServerInfo::serverIp() const
{
    return m_serverIp;
}

void ServerInfo::setServerIp(const QString &newServerIp)
{
    if (m_serverIp == newServerIp)
        return;
    m_serverIp = newServerIp;
    emit sigServerIpChanged(m_serverIp);
}

bool ServerInfo::autoEnable() const
{
    return m_autoEnable;
}

void ServerInfo::setAutoEnable(const bool &newAutoEnable)
{
    if (m_autoEnable == newAutoEnable)
        return;
    m_autoEnable = newAutoEnable;
    emit sigAutoEnableChanged(m_autoEnable);
}
