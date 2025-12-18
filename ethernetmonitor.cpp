#include "ethernetmonitor.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QProcess>

EthernetMonitor::EthernetMonitor(QObject *parent)
    : QObject(parent)
{
}

bool EthernetMonitor::isConnected() const
{
    return m_connected;
}

void EthernetMonitor::setIsConnected(bool newConnected)
{
    if (m_connected == newConnected)
        return;
    m_connected = newConnected;
    emit connectedChanged();

    if(m_connected) {
        QProcess::execute("sh", QStringList() << "-c" << "nmcli radio wifi off");
    }else {
        QProcess::execute("sh", QStringList() << "-c" << "nmcli radio wifi on");
    }
}
