#include "deviceinfosettings.h"
#include <QTextStream>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>


DeviceInfoSettings::DeviceInfoSettings(QObject *parent)
    : QObject(parent)
{}

bool DeviceInfoSettings::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject())
        return false;

    QJsonObject obj = doc.object();

    m_developedBy = obj.value("developedBy").toString();
    m_model = obj.value("model").toString();
    m_firmwareVersion = obj.value("firmwareVersion").toString();
    m_madeIn = obj.value("madeIn").toString();

    // Read board info automatically
    readBoardInfo();

    emit infoChanged();
    return true;
}

void DeviceInfoSettings::readBoardInfo()
{
    // CPU and GPU (hardcoded for RK3328 board)
    m_cpu = "ARM Cortex-A53 quad-core";
    m_gpu = "Mali-450MP2";

    // Run board info gathering in background thread
    QFuture<void> future = QtConcurrent::run([this]() {
        // --- RAM ---
        QString ramInfo = execCommand("free -h | grep Mem | awk '{print $2}'");
        m_ram = ramInfo.isEmpty() ? "Unknown RAM" : ramInfo.trimmed();

        // --- Ethernet ---
        QString ethInterface = execCommand("ip link | grep -E 'eth|en' | awk -F: '{print $2}' | head -n1").trimmed();
        if (!ethInterface.isEmpty()) {
            QString ethSpeed = execCommand(QString("cat /sys/class/net/%1/speed 2>/dev/null").arg(ethInterface)).trimmed();
            if (ethSpeed.isEmpty()) ethSpeed = "Unknown speed";
            m_ethernet = QString("%1 (%2 Mb/s)").arg(ethInterface).arg(ethSpeed);
        } else {
            m_ethernet = "No Ethernet";
        }

        // --- Wi-Fi ---
        QString wifiInterface = execCommand("ip link | grep -E 'wlan|wl' | awk -F: '{print $2}' | head -n1").trimmed();
        if (!wifiInterface.isEmpty()) {
            // Get driver
            QString driverPath = QString("/sys/class/net/%1/device/driver").arg(wifiInterface);
            QString wifiDriver;
            QFileInfo fi(driverPath);
            if (fi.exists())
                wifiDriver = fi.symLinkTarget().split("/").last();
            else
                wifiDriver = "Unknown driver";

            // Get bus info
            QString busInfo = execCommand(QString("basename $(readlink /sys/class/net/%1/device)").arg(wifiInterface)).trimmed();
            if (busInfo.isEmpty()) busInfo = "Unknown bus";

            m_wifi = QString("%1 (%2, bus %3)").arg(wifiInterface).arg(wifiDriver).arg(busInfo);
        } else {
            m_wifi = "No Wi-Fi";
        }
    });

    // Notify GUI when done
    static QFutureWatcher<void> watcher;
    connect(&watcher, &QFutureWatcher<void>::finished, this, [this]() {
        emit infoChanged();
    });
    watcher.setFuture(future);
}

QString DeviceInfoSettings::execCommand(const QString &cmd)
{
    QProcess process;
    process.start("/bin/sh", QStringList() << "-c" << cmd);
    process.waitForFinished(2000); // 2s timeout
    QString output = process.readAllStandardOutput();
    return output;
}
