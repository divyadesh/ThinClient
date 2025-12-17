#include "wificonfigmanager.h"

#include <QDebug>
#include <QHostAddress>

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/IpAddress>

#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WirelessSetting>
#include <NetworkManagerQt/WirelessSecuritySetting>
#include <NetworkManagerQt/Ipv4Setting>

WifiConfigManager::WifiConfigManager(QObject *parent)
    : QObject(parent)
{
}

/* ---------------------------------------------------------
 * Helpers
 * --------------------------------------------------------- */

void WifiConfigManager::setBusy(bool busy)
{
    if (m_isBusy == busy)
        return;
    m_isBusy = busy;
    emit isBusyChanged();
}

void WifiConfigManager::setError(const QString &error)
{
    m_lastError = error;
    emit lastErrorChanged();
    emit failed(error);
    setBusy(false);
}

bool WifiConfigManager::updateConnection(NetworkManager::Connection::Ptr &conn)
{
    NetworkManager::WirelessDevice::Ptr dev;

    for (const auto &d : NetworkManager::networkInterfaces()) {
        if (d->interfaceName() == m_interface &&
            d->type() == NetworkManager::Device::Wifi) {
            dev = d.objectCast<NetworkManager::WirelessDevice>();
            break;
        }
    }

    if (!dev) {
        setError("Wi-Fi device not found: " + m_interface);
        return false;
    }

    if (auto active = dev->activeConnection())
        conn = active->connection();

    if (!conn) {
        setError("No active Wi-Fi connection");
        return false;
    }

    return true;
}

/* ---------------------------------------------------------
 * Set DHCP
 * --------------------------------------------------------- */

bool WifiConfigManager::setDhcp()
{
    setBusy(true);
    m_lastError.clear();
    emit lastErrorChanged();

    NetworkManager::Connection::Ptr conn;
    if (!updateConnection(conn))
        return false;

    auto settings = conn->settings();
    auto ipv4 = settings->setting(NetworkManager::Setting::Ipv4)
                    .dynamicCast<NetworkManager::Ipv4Setting>();

    if (!ipv4) {
        setError("IPv4 settings missing");
        return false;
    }

    ipv4->setMethod(NetworkManager::Ipv4Setting::Automatic);
    ipv4->setAddresses({});
    ipv4->setGateway(QString());
    ipv4->setDns({});

    qInfo() << "[WifiConfigManager] Applying DHCP configuration";

    conn->update(settings->toMap());   // ✅ async DBus call

    setBusy(false);
    emit success();
    return true;
}

/* ---------------------------------------------------------
 * Set Static IPv4
 * --------------------------------------------------------- */

bool WifiConfigManager::setStatic(const QString &ip,
                                  const QString &subnet,
                                  const QString &gateway,
                                  const QString &dns1,
                                  const QString &dns2)
{
    setBusy(true);
    m_lastError.clear();
    emit lastErrorChanged();

    NetworkManager::Connection::Ptr conn;
    if (!updateConnection(conn))
        return false;

    auto settings = conn->settings();
    auto ipv4 = settings->setting(NetworkManager::Setting::Ipv4)
                    .dynamicCast<NetworkManager::Ipv4Setting>();

    if (!ipv4) {
        setError("IPv4 settings missing");
        return false;
    }

    QHostAddress ipAddr(ip);
    QHostAddress maskAddr(subnet);
    QHostAddress gwAddr(gateway);

    if (ipAddr.isNull() || maskAddr.isNull() || gwAddr.isNull()) {
        setError("Invalid IP, subnet, or gateway");
        return false;
    }

    NetworkManager::IpAddress addr;
    addr.setIp(ipAddr);
    addr.setNetmask(maskAddr);
    addr.setGateway(gwAddr);

    ipv4->setMethod(NetworkManager::Ipv4Setting::Manual);
    ipv4->setAddresses({addr});
    ipv4->setGateway(gateway);

    QList<QHostAddress> dns;
    if (!dns1.isEmpty()) dns.append(QHostAddress(dns1));
    if (!dns2.isEmpty()) dns.append(QHostAddress(dns2));
    ipv4->setDns(dns);

    qInfo() << "[WifiConfigManager] Applying static configuration";

    conn->update(settings->toMap());   // ✅ CORRECT

    setBusy(false);
    emit success();
    return true;
}


bool WifiConfigManager::connectToSsid(const QString &ssid,
                                      const QString &password)
{
    setBusy(true);
    m_lastError.clear();
    emit lastErrorChanged();

    auto dev = wifiDevice();
    if (!dev) {
        setError("Wi-Fi device not found");
        return false;
    }

    NetworkManager::ConnectionSettings settings(
        NetworkManager::ConnectionSettings::Wireless);

    settings.setId(ssid);
    settings.setUuid(NetworkManager::ConnectionSettings::createNewUuid());
    settings.setAutoconnect(true);
    settings.setInterfaceName(m_interface);

    // Wireless
    auto wifi = settings.setting(NetworkManager::Setting::Wireless)
                    .dynamicCast<NetworkManager::WirelessSetting>();
    wifi->setSsid(ssid.toUtf8());
    wifi->setMode(NetworkManager::WirelessSetting::Infrastructure);

    // Security (optional)
    if (!password.isEmpty()) {
        auto sec = settings.setting(NetworkManager::Setting::WirelessSecurity)
        .dynamicCast<NetworkManager::WirelessSecuritySetting>();
        sec->setKeyMgmt(NetworkManager::WirelessSecuritySetting::WpaPsk);
        sec->setPsk(password);
    }

    // IPv4 default to DHCP
    auto ipv4 = settings.setting(NetworkManager::Setting::Ipv4)
                    .dynamicCast<NetworkManager::Ipv4Setting>();
    ipv4->setMethod(NetworkManager::Ipv4Setting::Automatic);

    qInfo() << "[WifiConfigManager] Connecting to SSID:" << ssid;

    NetworkManager::addAndActivateConnection(settings.toMap(),
                                             dev->uni(),
                                             QString());

    setBusy(false);
    emit success();
    return true;
}

bool WifiConfigManager::disconnect()
{
    setBusy(true);

    auto dev = wifiDevice();
    if (!dev) {
        setError("Wi-Fi device not found");
        return false;
    }

    if (auto active = dev->activeConnection()) {
        qInfo() << "[WifiConfigManager] Disconnecting Wi-Fi";
        NetworkManager::deactivateConnection(active->path());
    }

    setBusy(false);
    emit success();
    return true;
}

bool WifiConfigManager::forget()
{
    setBusy(true);

    NetworkManager::Connection::Ptr conn;
    if (!updateConnection(conn))
        return false;

    qInfo() << "[WifiConfigManager] Forgetting Wi-Fi network";
    conn->remove();

    setBusy(false);
    emit success();
    return true;
}


NetworkManager::WirelessDevice::Ptr WifiConfigManager::wifiDevice()
{
    for (const auto &d : NetworkManager::networkInterfaces()) {
        if (d->interfaceName() == m_interface &&
            d->type() == NetworkManager::Device::Wifi) {
            return d.objectCast<NetworkManager::WirelessDevice>();
        }
    }
    return {};
}
