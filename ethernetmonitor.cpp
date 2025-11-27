#include "ethernetmonitor.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

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
}
