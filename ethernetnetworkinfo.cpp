#include "ethernetnetworkinfo.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QHostAddress>

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/IpConfig>
#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/Ipv4Setting>


static int maskToPrefix(const QString &mask)
{
    QHostAddress addr(mask);
    quint32 v = addr.toIPv4Address();

    int prefix = 0;
    while (v & 0x80000000) {
        prefix++;
        v <<= 1;
    }
    return prefix;
}

EthernetNetworkInfo::EthernetNetworkInfo(QObject *parent)
    : QObject(parent)
{
    updateEthernetNetworkInfo();
}

void EthernetNetworkInfo::clear()
{
    m_status.clear();
    m_method.clear();
    m_mac.clear();
    m_ip.clear();
    m_subnet.clear();
    m_gateway.clear();
    m_dns1.clear();
    m_dns2.clear();
}

QString EthernetNetworkInfo::readDefaultGateway(const QString &iface) const
{
    QFile file("/proc/net/route");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};

    QTextStream in(&file);
    in.readLine(); // header

    while (!in.atEnd()) {
        const QStringList parts = in.readLine().split(QRegExp("\\s+"));
        if (parts.size() < 3)
            continue;

        if (parts.at(0) == iface && parts.at(1) == "00000000") {

            bool ok = false;
            quint32 gw = parts.at(2).toUInt(&ok, 16);
            if (!ok)
                continue;

            return QString("%1.%2.%3.%4")
                .arg(gw & 0xFF)
                .arg((gw >> 8) & 0xFF)
                .arg((gw >> 16) & 0xFF)
                .arg((gw >> 24) & 0xFF);
        }
    }
    return {};
}

void EthernetNetworkInfo::updateEthernetNetworkInfo()
{
    clear();

    NetworkManager::WiredDevice::Ptr dev;

    for (const auto &d : NetworkManager::networkInterfaces()) {
        if (d->interfaceName() == m_interface &&
            d->type() == NetworkManager::Device::Ethernet) {
            dev = d.objectCast<NetworkManager::WiredDevice>();
            break;
        }
    }

    if (!dev) {
        qWarning() << "[EthernetNetworkInfo] Ethernet device not found:" << m_interface;
        emit changed();
        return;
    }

    /* Status */
    m_status = (dev->state() == NetworkManager::Device::Activated)
                   ? "Connected"
                   : "Disconnected";

    /* MAC */
    m_mac = dev->hardwareAddress();

    /* Method */
    NetworkManager::Ipv4Setting::Ptr ipv4;

    if (auto active = dev->activeConnection()) {
        if (auto conn = active->connection()) {
            auto settings = conn->settings();
            ipv4 = settings->setting(NetworkManager::Setting::Ipv4)
                       .dynamicCast<NetworkManager::Ipv4Setting>();

            if (ipv4) {
                m_method = (ipv4->method() ==
                            NetworkManager::Ipv4Setting::Automatic)
                               ? "DHCP"
                               : "Static";
            }
        }
    }

    /* DHCP path */
    if (m_method == "DHCP") {

        auto ipCfg = dev->ipV4Config();

        if (!ipCfg.addresses().isEmpty()) {
            const auto addr = ipCfg.addresses().first();
            m_ip = addr.ip().toString();
            m_subnet = addr.netmask().toString();
        }
        m_gateway = readDefaultGateway(m_interface);
    }

    /* Static path */
    else if (m_method == "Static" && ipv4) {

        if (!ipv4->addresses().isEmpty()) {
            const auto addr = ipv4->addresses().first();
            m_ip = addr.ip().toString();
            m_subnet = addr.netmask().toString();
        }

        auto ipCfg = dev->ipV4Config();
        if (ipCfg.isValid())
            m_gateway = ipCfg.gateway();

        const auto dns = ipv4->dns();
        if (dns.size() > 0) m_dns1 = dns.at(0).toString();
        if (dns.size() > 1) m_dns2 = dns.at(1).toString();
    }

    if (m_method == "DHCP") {
        auto ipCfg = dev->ipV4Config();

        const auto dns = ipCfg.nameservers();
        if (dns.size() > 0) m_dns1 = dns.at(0).toString();
        if (dns.size() > 1) m_dns2 = dns.at(1).toString();
    }

    /* Log */
    constexpr int W = 23;

    qInfo().noquote()
        << "\n+--------------------+-------------------------+"
        << "\n| Ethernet Network Info                         |"
        << "\n+--------------------+-------------------------+"
        << "\n| Interface          | " << m_interface.leftJustified(W) << "|"
        << "\n| Status             | " << m_status.leftJustified(W) << "|"
        << "\n| MAC Address        | " << m_mac.leftJustified(W) << "|"
        << "\n| Method             | " << m_method.leftJustified(W) << "|"
        << "\n| IP Address         | " << m_ip.leftJustified(W) << "|"
        << "\n| Subnet Mask        | " << m_subnet.leftJustified(W) << "|"
        << "\n| Gateway            | " << m_gateway.leftJustified(W) << "|"
        << "\n| DNS 1              | " << m_dns1.leftJustified(W) << "|"
        << "\n| DNS 2              | " << m_dns2.leftJustified(W) << "|"
        << "\n+--------------------+-------------------------+";

    emit changed();
}

void EthernetNetworkInfo::switchToDhcpAsync()
{
    using namespace NetworkManager;

    emit operationStarted();
    setIsBusy(true);

    WiredDevice::Ptr dev;

    for (const Device::Ptr &d : NetworkManager::networkInterfaces()) {
        if (d->interfaceName() == m_interface &&
            d->type() == Device::Ethernet) {
            dev = d.dynamicCast<WiredDevice>();
            break;
        }
    }

    if (!dev) {
        setIsBusy(false);
        emit operationFinished(false,
                               tr("Ethernet device not found"));
        return;
    }

    ActiveConnection::Ptr active = dev->activeConnection();
    if (!active) {
        setIsBusy(false);
        emit operationFinished(false,
                               tr("No active Ethernet connection"));
        return;
    }

    Connection::Ptr con = active->connection();
    if (!con) {
        setIsBusy(false);
        emit operationFinished(false,
                               tr("Failed to resolve network connection"));
        return;
    }

    ConnectionSettings::Ptr settings = con->settings();
    Ipv4Setting::Ptr ipv4 =
        settings->setting(Setting::Ipv4).staticCast<Ipv4Setting>();

    if (!ipv4) {
        setIsBusy(false);
        emit operationFinished(false,
                               tr("IPv4 configuration not available"));
        return;
    }

    /* Switch to DHCP */
    ipv4->setMethod(Ipv4Setting::Automatic);
    ipv4->setAddresses({});
    ipv4->setGateway(QString());
    ipv4->setDns({});

    con->update(settings->toMap());
    con->save();

    /* Deactivate using DBus path */
    NetworkManager::deactivateConnection(active->path());

    connect(active.data(), &ActiveConnection::stateChanged,
            this, [this, con, dev](ActiveConnection::State state) {

                if (state == ActiveConnection::Deactivated) {
                    NetworkManager::activateConnection(
                        con->path(),
                        dev->uni(),
                        QString()
                        );

                }
            });

    connect(dev.data(), &Device::stateChanged,
            this, [this](Device::State state) {
                setIsBusy(false);
                if (state == Device::Activated) {
                    updateEthernetNetworkInfo();
                    emit operationFinished(true,
                                           tr("Switched to DHCP successfully"));
                }
                else if (state == Device::Failed) {
                    emit operationFinished(false,
                                           tr("Failed to obtain DHCP lease"));
                }
            });
}

void EthernetNetworkInfo::switchToStaticAsync(const QString &ip,
                                              const QString &subnetMask,
                                              const QString &gateway,
                                              const QString &dns1,
                                              const QString &dns2)
{
    using namespace NetworkManager;

    emit operationStarted();
    setIsBusy(true);

    WiredDevice::Ptr dev;

    for (const Device::Ptr &d : NetworkManager::networkInterfaces()) {
        if (d->interfaceName() == m_interface &&
            d->type() == Device::Ethernet) {
            dev = d.dynamicCast<WiredDevice>();
            break;
        }
    }

    if (!dev) {
        setIsBusy(false);
        emit operationFinished(false, tr("Ethernet device not found"));
        return;
    }

    ActiveConnection::Ptr active = dev->activeConnection();
    if (!active) {
        setIsBusy(false);
        emit operationFinished(false, tr("No active Ethernet connection"));
        return;
    }

    Connection::Ptr con = active->connection();
    if (!con) {
        setIsBusy(false);
        emit operationFinished(false, tr("Failed to resolve network connection"));
        return;
    }

    ConnectionSettings::Ptr settings = con->settings();
    Ipv4Setting::Ptr ipv4 =
        settings->setting(Setting::Ipv4).staticCast<Ipv4Setting>();

    if (!ipv4) {
        setIsBusy(false);
        emit operationFinished(false, tr("IPv4 configuration not available"));
        return;
    }

    /* ---- Build static address ---- */
    int prefix = maskToPrefix(subnetMask);

    NetworkManager::IpAddress addr;
    addr.setIp(QHostAddress(ip));
    addr.setPrefixLength(prefix);

    QList<NetworkManager::IpAddress> addresses;
    addresses << addr;

    /* ---- DNS ---- */
    QList<QHostAddress> dns;
    if (!dns1.isEmpty())
        dns << QHostAddress(dns1);
    if (!dns2.isEmpty())
        dns << QHostAddress(dns2);

    /* ---- Apply static config ---- */
    ipv4->setMethod(NetworkManager::Ipv4Setting::Manual);
    ipv4->setAddresses(addresses);
    ipv4->setGateway(gateway);
    ipv4->setDns(dns);

    /* CRITICAL FIX */
    ipv4->setNeverDefault(false);      // <-- REQUIRED
    ipv4->setRouteMetric(100);         // <-- RECOMMENDED

    con->update(settings->toMap());
    con->save();

    /* ---- Reactivate connection ---- */
    NetworkManager::deactivateConnection(active->path());

    connect(active.data(), &ActiveConnection::stateChanged,
            this, [this, con, dev](ActiveConnection::State state) {

                if (state == ActiveConnection::Deactivated) {
                    NetworkManager::activateConnection(
                        con->path(),
                        dev->uni(),
                        QString()
                        );
                }
            });

    connect(dev.data(), &Device::stateChanged,
            this, [this](Device::State state) {

                setIsBusy(false);

                if (state == Device::Activated) {
                    updateEthernetNetworkInfo();
                    emit operationFinished(true,
                                           tr("Static IP configuration applied"));
                }
                else if (state == Device::Failed) {
                    emit operationFinished(false,
                                           tr("Failed to apply static IP configuration"));
                }
            });
}

bool EthernetNetworkInfo::isBusy() const
{
    return m_isBusy;
}

void EthernetNetworkInfo::setIsBusy(bool newIsBusy)
{
    if (m_isBusy == newIsBusy)
        return;
    m_isBusy = newIsBusy;
    emit isBusyChanged();
}
