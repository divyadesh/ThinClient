#include "connectioninfo.h"

/**
 * @brief Constructs a new ConnectionInfo object.
 * Initializes all boolean values to false and strings to empty.
 */
ConnectionInfo::ConnectionInfo(QObject *parent)
    : QObject(parent)
{}

/* ============================================================
 *  Getters Implementation
 * ============================================================ */

QString ConnectionInfo::connectionId() const
{
    return m_connectionId;
}
QString ConnectionInfo::connectionName() const
{
    return m_connectionName;
}
QString ConnectionInfo::serverIp() const
{
    return m_serverIp;
}
QString ConnectionInfo::deviceName() const
{
    return m_deviceName;
}
QString ConnectionInfo::userName() const
{
    return m_userName;
}
QString ConnectionInfo::password() const
{
    return m_password;
}
QString ConnectionInfo::performance() const
{
    return m_performance;
}

bool ConnectionInfo::enableAudio() const
{
    return m_enableAudio;
}
bool ConnectionInfo::enableMicrophone() const
{
    return m_enableMicrophone;
}
bool ConnectionInfo::redirectDrive() const
{
    return m_redirectDrive;
}
bool ConnectionInfo::redirectUsbDevice() const
{
    return m_redirectUsbDevice;
}
bool ConnectionInfo::security() const
{
    return m_security;
}
bool ConnectionInfo::gateway() const
{
    return m_gateway;
}

QString ConnectionInfo::gatewayIp() const
{
    return m_gatewayIp;
}
QString ConnectionInfo::gatewayUserName() const
{
    return m_gatewayUserName;
}
QString ConnectionInfo::gatewayPassword() const
{
    return m_gatewayPassword;
}

bool ConnectionInfo::autoConnect() const
{
    return m_autoConnect;
}

/* ============================================================
 *  Setters Implementation
 * ============================================================ */

void ConnectionInfo::setConnectionId(const QString &value)
{
    if (m_connectionId == value)
        return;
    m_connectionId = value;
    emit connectionIdChanged();
}

void ConnectionInfo::setConnectionName(const QString &value)
{
    if (m_connectionName == value)
        return;
    m_connectionName = value;
    emit connectionNameChanged();
}

void ConnectionInfo::setServerIp(const QString &value)
{
    if (m_serverIp == value)
        return;
    m_serverIp = value;
    emit serverIpChanged();
}

void ConnectionInfo::setDeviceName(const QString &value)
{
    if (m_deviceName == value)
        return;
    m_deviceName = value;
    emit deviceNameChanged();
}

void ConnectionInfo::setUserName(const QString &value)
{
    if (m_userName == value)
        return;
    m_userName = value;
    emit userNameChanged();
}

void ConnectionInfo::setPassword(const QString &value)
{
    if (m_password == value)
        return;
    m_password = value;
    emit passwordChanged();
}

void ConnectionInfo::setPerformance(const QString &value)
{
    if (m_performance == value)
        return;
    m_performance = value;
    emit performanceChanged();
}

void ConnectionInfo::setEnableAudio(bool value)
{
    if (m_enableAudio == value)
        return;
    m_enableAudio = value;
    emit enableAudioChanged();
}

void ConnectionInfo::setEnableMicrophone(bool value)
{
    if (m_enableMicrophone == value)
        return;
    m_enableMicrophone = value;
    emit enableMicrophoneChanged();
}

void ConnectionInfo::setRedirectDrive(bool value)
{
    if (m_redirectDrive == value)
        return;
    m_redirectDrive = value;
    emit redirectDriveChanged();
}

void ConnectionInfo::setRedirectUsbDevice(bool value)
{
    if (m_redirectUsbDevice == value)
        return;
    m_redirectUsbDevice = value;
    emit redirectUsbDeviceChanged();
}

void ConnectionInfo::setSecurity(bool value)
{
    if (m_security == value)
        return;
    m_security = value;
    emit securityChanged();
}

void ConnectionInfo::setGateway(bool value)
{
    if (m_gateway == value)
        return;
    m_gateway = value;
    emit gatewayChanged();
}

void ConnectionInfo::setGatewayIp(const QString &value)
{
    if (m_gatewayIp == value)
        return;
    m_gatewayIp = value;
    emit gatewayIpChanged();
}

void ConnectionInfo::setGatewayUserName(const QString &value)
{
    if (m_gatewayUserName == value)
        return;
    m_gatewayUserName = value;
    emit gatewayUserNameChanged();
}

void ConnectionInfo::setGatewayPassword(const QString &value)
{
    if (m_gatewayPassword == value)
        return;
    m_gatewayPassword = value;
    emit gatewayPasswordChanged();
}

void ConnectionInfo::setAutoConnect(bool value)
{
    if (m_autoConnect == value)
        return;
    m_autoConnect = value;
    emit autoConnectChanged();
}
