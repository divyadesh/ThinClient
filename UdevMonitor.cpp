#include "UdevMonitor.h"
#include <libudev.h>
#include <QObject>
#include <QSocketNotifier>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QString>
#include <QStringList>

UdevMonitor::UdevMonitor(QObject *parent)
    : QObject(parent)
{
    struct udev *udev = udev_new();
    if (!udev) {
        qWarning() << "Failed to create udev context";
        return;
    }

    struct udev_monitor *mon = udev_monitor_new_from_netlink(udev, "udev");
    if (!mon) {
        qWarning() << "Failed to create udev monitor";
        udev_unref(udev);
        return;
    }

    // ✅ Listen for both block (storage) and usb (controller) subsystems
    udev_monitor_filter_add_match_subsystem_devtype(mon, "block", nullptr);
    udev_monitor_filter_add_match_subsystem_devtype(mon, "usb", nullptr);
    // NEW — monitor all network devices (ethernet, wifi, virtual)
    udev_monitor_filter_add_match_subsystem_devtype(mon, "net", nullptr);

    udev_monitor_enable_receiving(mon);

    int fd = udev_monitor_get_fd(mon);
    notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);

    connect(notifier, &QSocketNotifier::activated, this, [=]() {
        struct udev_device *dev = udev_monitor_receive_device(mon);
        if (!dev)
            return;

        const char *actionC = udev_device_get_action(dev);
        const char *subsysC = udev_device_get_subsystem(dev);
        const char *devnodeC = udev_device_get_devnode(dev);

        QString action = QString::fromUtf8(actionC ? actionC : "");
        QString subsystem = QString::fromUtf8(subsysC ? subsysC : "");
        QString devNode = QString::fromUtf8(devnodeC ? devnodeC : "");

        qDebug() << "UDEV Event:" << action << "Subsystem:" << subsystem << "Node:" << devNode;

        // -------- NETWORK EVENTS ---------------------------------------
        if (subsystem == "net") {
            const char *iface = udev_device_get_sysname(dev);
            const char *operState = udev_device_get_sysattr_value(dev, "operstate");
            const char *carrier = udev_device_get_sysattr_value(dev, "carrier");
            const char *devType  = udev_device_get_sysattr_value(dev, "type");
            const char *speed    = udev_device_get_sysattr_value(dev, "speed");
            const char *addr     = udev_device_get_sysattr_value(dev, "address");

            qDebug() << "----------------------------------------";
            qDebug() << "NET EVENT:" << action;
            qDebug() << "Interface:" << (iface ? iface : "unknown");
            qDebug() << "OperState:" << (operState ? operState : "unknown");
            qDebug() << "Carrier:"   << (carrier ? carrier : "unknown");
            qDebug() << "Speed:"     << (speed ? speed : "unknown");
            qDebug() << "MAC:"       << (addr ? addr : "unknown");
            qDebug() << "DevType:"   << (devType ? devType : "unknown");
            qDebug() << "----------------------------------------";

            // Emit your own signals here (optional)
            handleEthernetEvent(
                QString::fromUtf8(iface ? iface : ""),
                QString::fromUtf8(operState ? operState : ""),
                action
                );
        }

        // ✅ Update connection status only for storage block devices
        if (subsystem == "block") {
            if (action == "add") {
                m_connected = true;
            } else if (action == "remove") {
                // only mark disconnected when no /dev/sd* devices remain
                QDir devDir("/dev");
                QStringList devs = devDir.entryList(QStringList() << "sd*", QDir::System);
                m_connected = !devs.isEmpty();
            }
            if (devNode.startsWith("/dev/sd") && devNode.contains(QRegExp("[0-9]$"))) {
                setUsbStoragePort(devNode);   // ONLY /dev/sda1
                emit usbConnectedChanged();
            }
        }

        emit usbEvent(action, devNode);
        udev_device_unref(dev);
    });

    QDir devDir("/dev");
    QStringList partitions = devDir.entryList(
        QStringList() << "sd*[0-9]",
        QDir::System
        );

    if (!partitions.isEmpty()) {
        m_connected = true;

        // Pick first available partition
        QString devNode = "/dev/" + partitions.first();
        setUsbStoragePort(devNode);
    } else {
        m_connected = false;
    }

    emit usbConnectedChanged();
}

QString UdevMonitor::usbStoragePort() const
{
    return m_usbStoragePort;
}

void UdevMonitor::setUsbStoragePort(QString newUsbStoragePort)
{
    if (m_usbStoragePort == newUsbStoragePort)
        return;
    m_usbStoragePort = newUsbStoragePort;
    emit usbStoragePortChanged();
}

void UdevMonitor::handleEthernetEvent(const QString &interfaceName,
                                      const QString &state,
                                      const QString &action)
{
    qDebug().noquote() << "\n===== [Ethernet Event Detected] ===========================";
    qDebug().noquote() << "Interface Name : " << interfaceName;
    qDebug().noquote() << "Udev Action    : " << action;
    qDebug().noquote() << "OperState      : " << state;
    qDebug().noquote() << "------------------------------------------------------------";

    const QString basePath = "/sys/class/net/" + interfaceName + "/";

    auto readAttr = [&](const QString &name) -> QString {
        QFile f(basePath + name);
        if (!f.open(QIODevice::ReadOnly))
            return "N/A";
        return QString::fromUtf8(f.readAll()).trimmed();
    };

    QString carrier     = readAttr("carrier");
    QString speed       = readAttr("speed");
    QString mtu         = readAttr("mtu");
    QString duplex      = readAttr("duplex");
    QString addr        = readAttr("address");
    QString devType     = readAttr("type");
    QString pciDevice   = readAttr("device/uevent");
    QString ifaceFlags  = readAttr("flags");
    QString multicast   = readAttr("multicast");
    QString queues      = readAttr("queues");

    // NIC driver info (if available)
    QString driver;
    QFile driverLink(basePath + "device/driver/module");
    if (driverLink.exists()) {
        driverLink.open(QIODevice::ReadOnly);
        driver = QString::fromUtf8(driverLink.readAll()).trimmed();
    } else {
        driver = "N/A";
    }

    // Human-readable type
    QString readableType = "Unknown";
    if (devType == "1") readableType = "Ethernet";
    else if (devType == "512") readableType = "WiFi";
    else if (devType == "772") readableType = "Loopback";
    else if (devType == "32") readableType = "IP Tunnel";

    qDebug().noquote() << "Device Type    : " << readableType << " (" << devType << ")";
    qDebug().noquote() << "MAC Address    : " << addr;
    qDebug().noquote() << "Carrier        : " << carrier;
    qDebug().noquote() << "Speed          : " << speed << " Mbps";
    qDebug().noquote() << "Duplex         : " << duplex;
    qDebug().noquote() << "MTU            : " << mtu;
    qDebug().noquote() << "Flags          : " << ifaceFlags;
    qDebug().noquote() << "Multicast      : " << multicast;
    qDebug().noquote() << "Driver Module  : " << driver;

    qDebug().noquote() << "Device Uevent  : \n" << pciDevice;

    qDebug().noquote() << "Queues Info    : \n" << queues;
    qDebug().noquote() << "------------------------------------------------------------";

    // Dump all sysfs attributes for full debugging
    QDir sysDir(basePath);
    QStringList attrs = sysDir.entryList(QDir::Files);

    qDebug().noquote() << "[All sysfs attributes for]" << interfaceName;
    for (const QString &attr : attrs) {
        QString value = readAttr(attr);
        qDebug().noquote().nospace()
            << "  - " << attr << " = " << value;
    }

    qDebug().noquote() << "============================================================\n";
}
