#include "ethernetmonitor.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

EthernetMonitor::EthernetMonitor(QObject *parent)
    : QObject(parent)
{
    m_carrierFile = carrierPath();
}

/**
 * @brief Start monitoring both sysfs + NetworkManager
 */
void EthernetMonitor::start()
{
    qDebug() << "[EthernetMonitor] Starting unified Ethernet monitor...";

    // ------------------------------
    // 1) SYSFS CARRIER MONITOR
    // ------------------------------
    if (QFile::exists(m_carrierFile)) {
        m_carrierWatcher.addPath(m_carrierFile);
        connect(&m_carrierWatcher, &QFileSystemWatcher::fileChanged,
                this, &EthernetMonitor::checkCarrier);
        checkCarrier();
    } else {
        qWarning() << "[EthernetMonitor] Carrier file missing:" << m_carrierFile;
    }

    // ------------------------------
    // 2) NETWORKMANAGER MONITOR
    // ------------------------------
    if (isNmManaged()) {
        qDebug() << "[EthernetMonitor] NetworkManager manages" << m_iface;

        bool ok = QDBusConnection::systemBus().connect(
            "org.freedesktop.NetworkManager",
            "/org/freedesktop/NetworkManager",
            "org.freedesktop.NetworkManager",
            "StateChanged",
            this,
            SLOT(nmStateChanged(uint))
            );

        if (!ok)
            qWarning() << "[EthernetMonitor] Failed to subscribe to NM StateChanged!";

        fetchNmIp();
    }
    else {
        qWarning() << "[EthernetMonitor] NM does NOT manage" << m_iface
                   << "- sysfs-only mode active.";
    }
}

/**
 * @brief Read PHY cable state from sysfs
 */
void EthernetMonitor::checkCarrier()
{
    QFile f(m_carrierFile);
    if (!f.open(QIODevice::ReadOnly))
        return;

    QString line = QTextStream(&f).readLine().trimmed();
    f.close();

    bool newState = (line == "1");

    if (newState != m_linkUp) {
        m_linkUp = newState;

        if (m_linkUp) {
            qDebug() << "[EthernetMonitor] Cable Plugged (carrier=1)";
            emit cablePlugged();
            emit stateChanged("link-up");
        } else {
            qDebug() << "[EthernetMonitor] Cable Unplugged (carrier=0)";
            emit cableUnplugged();
            emit ethernetDisconnected();
            emit connectedChanged(false);
            emit stateChanged("link-down");

            m_connected = false;
            m_ipAddress.clear();
        }
    }

    // Re-add path (some kernels remove the carrier node temporarily)
    if (!m_carrierWatcher.files().contains(m_carrierFile))
        m_carrierWatcher.addPath(m_carrierFile);
}

/**
 * @brief Check if NetworkManager manages this interface
 */
bool EthernetMonitor::isNmManaged()
{
    QDBusInterface devIface(
        "org.freedesktop.NetworkManager",
        "/org/freedesktop/NetworkManager/Devices/2",   // eth0 index on many systems
        "org.freedesktop.NetworkManager.Device",
        QDBusConnection::systemBus()
        );

    if (!devIface.isValid())
        return false;

    bool managed = devIface.property("Managed").toBool();
    qDebug() << "[EthernetMonitor] NM Managed property:" << managed;

    return managed;
}

/**
 * @brief Handle NetworkManager global state
 */
void EthernetMonitor::nmStateChanged(uint newState)
{
    qDebug() << "[EthernetMonitor] NM StateChanged =" << newState;

    switch (newState) {
    case 20: // NM_STATE_DISCONNECTED
        qDebug() << "[EthernetMonitor] NM reports disconnected.";
        m_connected = false;
        emit connectedChanged(false);
        emit ethernetDisconnected();
        emit stateChanged("disconnected");
        break;

    case 70: // IP_READY
    case 100: // ACTIVATED
        fetchNmIp();
        break;

    default:
        break;
    }
}

/**
 * @brief Fetch IPv4 from NetworkManager
 */
void EthernetMonitor::fetchNmIp()
{
    QDBusInterface dev(
        "org.freedesktop.NetworkManager",
        "/org/freedesktop/NetworkManager/Devices/2",
        "org.freedesktop.NetworkManager.Device",
        QDBusConnection::systemBus()
        );

    QDBusObjectPath ip4Path = dev.property("Ip4Config").value<QDBusObjectPath>();
    if (ip4Path.path().isEmpty())
        return;

    QDBusInterface ipIface(
        "org.freedesktop.NetworkManager",
        ip4Path.path(),
        "org.freedesktop.NetworkManager.IP4Config",
        QDBusConnection::systemBus()
        );

    auto list = ipIface.property("AddressData").toList();
    if (list.isEmpty())
        return;

    QString ip = list[0].toMap().value("address").toString();
    if (ip.isEmpty())
        return;

    m_connected = true;
    m_ipAddress = ip;

    emit ipAddressChanged(ip);
    emit connectedChanged(true);
    emit ethernetConnected(ip);
    emit stateChanged("connected");

    qDebug() << "[EthernetMonitor] NM IP acquired:" << ip;
}

/**
 * @brief Path to sysfs carrier file
 */
QString EthernetMonitor::carrierPath() const
{
    return QString("/sys/class/net/%1/carrier").arg(m_iface);
}
