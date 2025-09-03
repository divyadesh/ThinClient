#include "DeviceInfo.h"
#include "QDebug"
#include <QRegularExpression>

DeviceInfo::DeviceInfo(QObject *parent)
    : QObject{parent}
{}

QString DeviceInfo::cpuDetails() const
{
    return m_cpuDetails;
}

void DeviceInfo::setCpuDetails(const QString &newCpuDetails)
{
    if (m_cpuDetails == newCpuDetails)
        return;
    m_cpuDetails = newCpuDetails;
    emit sigCpuDetailsChanged(m_cpuDetails);
}

QString DeviceInfo::ramSize() const
{
    return m_ramSize;
}

void DeviceInfo::setRamSize(const QString &newRamSize)
{
    if (m_ramSize == newRamSize)
        return;
    m_ramSize = newRamSize;
    emit sigRamSizeChanged(m_ramSize);
}

void DeviceInfo::getDeviceInfoDetails() {
    m_process.start("bash", QStringList() << "-c" << "lscpu | grep 'Model name'");
    m_process.waitForFinished();

    QString output = m_process.readAllStandardOutput();
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);

    for (const QString &line : lines) {
        QStringList fields = line.split(":");
        qDebug()<<"1. "<<fields[0];
        qDebug()<<"2. "<<fields[1].trimmed();
        setCpuDetails(fields[1].trimmed());
    }
    m_process.start("bash", QStringList() << "-c" << "grep MemTotal /proc/meminfo");
    m_process.waitForFinished();

    output = m_process.readAllStandardOutput();
    QStringList parts = output.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

    if (parts.size() >= 2) {
        bool ok;
        qint64 memKb = parts[1].toLongLong(&ok);
        if (ok) {
            double memGb = memKb / 1024.0 / 1024.0;
            QString ramSize =QString::number(memGb, 'f', 2) + "GB";
            qDebug() << "Total RAM:" << QString::number(memGb, 'f', 2) << "GB";
            setRamSize(ramSize);
        }
    }
}
