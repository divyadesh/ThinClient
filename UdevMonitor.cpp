#include "UdevMonitor.h"
#include <libudev.h>
#include <QDebug>
#include <QDir>

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
            setUsbStoragePort(devNode);
            emit usbConnectedChanged();
        }

        emit usbEvent(action, devNode);
        udev_device_unref(dev);
    });

    // ✅ Initial state detection — check if any /dev/sd* devices already exist
    QDir devDir("/dev");
    QStringList devs = devDir.entryList(QStringList() << "sd*", QDir::System);
    m_connected = !devs.isEmpty();
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
