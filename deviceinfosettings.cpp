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

        // --- Ethernet MAC ---
        QString ethMac;
        {
            QFile f("/sys/class/net/eth0/address");
            if (f.exists() && f.open(QIODevice::ReadOnly)) {
                ethMac = QString(f.readAll()).trimmed();
                f.close();
            }
        }

        if (!ethMac.isEmpty())
            m_ethernet = QString("%1").arg(ethMac);
        else
            m_ethernet = "No Ethernet";


        // --- Wi-Fi MAC ---
        QString wifiMac;
        {
            QFile f("/sys/class/net/wlan0/address");
            if (f.exists() && f.open(QIODevice::ReadOnly)) {
                wifiMac = QString(f.readAll()).trimmed();
                f.close();
            }
        }

        if (!wifiMac.isEmpty())
            m_wifi = QString("%1").arg(wifiMac);
        else
            m_wifi = "No Wi-Fi";
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
