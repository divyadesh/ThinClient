#include "wifinetworkinfo.h"

#include <QDebug>
#include <QtEndian>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QFile>
#include <QTextStream>
#include <NetworkManagerQt/AccessPoint>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/AccessPoint>
#include <NetworkManagerQt/IpConfig>
#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/Ipv4Setting>

WifiNetworkInfo::WifiNetworkInfo(QObject *parent)
    : QObject(parent)
{
    updateWifiNetworkInfo();
}

QString WifiNetworkInfo::readDefaultGateway(const QString &iface) const
{
    QFile file("/proc/net/route");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[WifiNetworkInfo] Failed to open /proc/net/route";
        return {};
    }

    QTextStream in(&file);
    in.readLine(); // skip header

    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;

        const QStringList parts = line.split(QRegExp("\\s+"));
        if (parts.size() < 3)
            continue;

        const QString routeIface = parts.at(0);
        const QString destination = parts.at(1);
        const QString gatewayHex = parts.at(2);

        // Default route for the requested interface
        if (routeIface == iface && destination == "00000000") {

            bool ok = false;
            quint32 gw = gatewayHex.toUInt(&ok, 16);
            if (!ok)
                continue;

            // Correct byte extraction (NO qFromLittleEndian)
            const quint8 b1 =  gw        & 0xFF;
            const quint8 b2 = (gw >> 8)  & 0xFF;
            const quint8 b3 = (gw >> 16) & 0xFF;
            const quint8 b4 = (gw >> 24) & 0xFF;

            const QString gateway =
                QString("%1.%2.%3.%4")
                    .arg(b1).arg(b2).arg(b3).arg(b4);

            qInfo() << "[WifiNetworkInfo] DHCP gateway for"
                    << iface << ":" << gateway;

            return gateway;
        }
    }

    qWarning() << "[WifiNetworkInfo] No default gateway found for interface" << iface;
    return {};
}

void WifiNetworkInfo::clear()
{
    m_ssid.clear();
    m_method.clear();
    m_ip.clear();
    m_subnet.clear();
    m_gateway.clear();
    m_dns1.clear();
    m_dns2.clear();

    m_status.clear();
    m_security.clear();
    m_mac.clear();
}

QString WifiNetworkInfo::cidrToNetmask(int prefix) const
{
    quint32 mask = (prefix == 0) ? 0 : 0xFFFFFFFF << (32 - prefix);
    QHostAddress addr(mask);
    return addr.toString();
}

void WifiNetworkInfo::updateWifiNetworkInfo()
{
    clear();

    qInfo() << "[WifiNetworkInfo] Refreshing Wi-Fi network info for" << m_interface;

    NetworkManager::WirelessDevice::Ptr dev;

    for (const auto &d : NetworkManager::networkInterfaces()) {
        if (d->interfaceName() == m_interface &&
            d->type() == NetworkManager::Device::Wifi) {
            dev = d.objectCast<NetworkManager::WirelessDevice>();
            break;
        }
    }

    if (!dev) {
        qWarning() << "[WifiNetworkInfo] Wi-Fi device not found:" << m_interface;
        emit changed();
        return;
    }

    switch (dev->state()) {
    case NetworkManager::Device::Activated:
        m_status = "Connected";
        break;
    case NetworkManager::Device::Disconnected:
        m_status = "Disconnected";
        break;
    case NetworkManager::Device::Unavailable:
        m_status = "Unavailable";
        break;
    case NetworkManager::Device::Failed:
        m_status = "Failed";
        break;
    default:
        m_status = "Connecting";
        break;
    }

    if (auto ap = dev->activeAccessPoint()) {
        m_mac = ap->hardwareAddress();
    }


    // -------- Security --------
    if (auto ap = dev->activeAccessPoint()) {

        const auto caps = ap->capabilities();
        const auto wpa  = ap->wpaFlags();
        const auto rsn  = ap->rsnFlags();

        // Open network (no privacy)
        if (!caps.testFlag(NetworkManager::AccessPoint::Privacy)) {
            m_security = "Open";
        }
        // Secured networks
        else if (rsn != 0) {
            m_security = "WPA2/WPA3";
        }
        else if (wpa != 0) {
            m_security = "WPA";
        }
        else {
            m_security = "Secured";
        }

    } else {
        m_security = "Unknown";
    }

    /* ---------------- SSID ---------------- */

    if (auto ap = dev->activeAccessPoint()) {
        m_ssid = ap->ssid();
    }

    /* ---------------- Determine Method (FIRST) ---------------- */

    NetworkManager::Ipv4Setting::Ptr ipv4;

    if (auto active = dev->activeConnection()) {
        if (auto conn = active->connection()) {
            auto settings = conn->settings();
            ipv4 = settings->setting(NetworkManager::Setting::Ipv4)
                       .dynamicCast<NetworkManager::Ipv4Setting>();

            if (ipv4) {
                if (ipv4->method() == NetworkManager::Ipv4Setting::Automatic)
                    m_method = "DHCP";
                else if (ipv4->method() == NetworkManager::Ipv4Setting::Manual)
                    m_method = "Static";
                else
                    m_method = "Unknown";
            }
        }
    }

    /* ---------------- DHCP PATH (runtime only) ---------------- */

    if (m_method == "DHCP") {

        NetworkManager::IpConfig ipCfg = dev->ipV4Config();

        // IP + Subnet (device-level)
        if (!ipCfg.addresses().isEmpty()) {
            const auto addr = ipCfg.addresses().first();
            m_ip = addr.ip().toString();
            m_subnet = addr.netmask().toString();
        }

        // Gateway handling
        if (!ipCfg.routes().isEmpty()) {
            // Case 1: NetworkManager provided a per-device route (rare for p2p0)
            m_gateway = ipCfg.routes().first().nextHop().toString();
        } else {
            // Case 2: Fallback to kernel routing table (DHCP hotspot case)
            m_gateway = readDefaultGateway(m_interface);  // e.g. "p2p0"
        }

        // IMPORTANT: DHCP → do NOT show DNS
        m_dns1.clear();
        m_dns2.clear();
    }

    /* ---------------- STATIC PATH (configured only) ---------------- */

    else if (m_method == "Static" && ipv4) {

        // IP + Subnet
        if (!ipv4->addresses().isEmpty()) {
            const auto addr = ipv4->addresses().first();
            m_ip = addr.ip().toString();
            m_subnet = addr.netmask().toString();
        }

        // Gateway
        m_gateway = ipv4->gateway();

        // DNS
        const auto dns = ipv4->dns();
        if (dns.size() > 0) m_dns1 = dns.at(0).toString();
        if (dns.size() > 1) m_dns2 = dns.at(1).toString();
    }

    /* ---------------- Logging ---------------- */

    constexpr int VALUE_WIDTH = 23;

    qInfo().noquote()
        << "\n+--------------------+-------------------------+"
        << "\n| Field              | Value                   |"
        << "\n+--------------------+-------------------------+"
        << "\n| Interface          | " << m_interface.leftJustified(VALUE_WIDTH, ' ') << "|"
        << "\n| Status             | " << m_status.leftJustified(VALUE_WIDTH, ' ') << "|"
        << "\n| SSID               | " << m_ssid.leftJustified(VALUE_WIDTH, ' ') << "|"
        << "\n| Security           | " << m_security.leftJustified(VALUE_WIDTH, ' ') << "|"
        << "\n| MAC Address        | " << m_mac.leftJustified(VALUE_WIDTH, ' ') << "|"
        << "\n| Method             | " << m_method.leftJustified(VALUE_WIDTH, ' ') << "|"
        << "\n| IP Address         | " << m_ip.leftJustified(VALUE_WIDTH, ' ') << "|"
        << "\n| Subnet Mask        | " << m_subnet.leftJustified(VALUE_WIDTH, ' ') << "|"
        << "\n| Gateway            | " << m_gateway.leftJustified(VALUE_WIDTH, ' ') << "|"
        << "\n| DNS 1              | " << m_dns1.leftJustified(VALUE_WIDTH, ' ') << "|"
        << "\n| DNS 2              | " << m_dns2.leftJustified(VALUE_WIDTH, ' ') << "|"
        << "\n+--------------------+-------------------------+";

    emit changed();
}
