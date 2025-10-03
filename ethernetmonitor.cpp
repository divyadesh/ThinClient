#include "ethernetmonitor.h"

EthernetMonitor::EthernetMonitor(QObject *parent)
    : QObject(parent), m_connected(false)
{}

bool EthernetMonitor::isConnected() const
{
    return m_connected;
}

void EthernetMonitor::setConnected(bool connected)
{
    if (m_connected != connected) {
        m_connected = connected;
        emit connectedChanged(m_connected);
    }
}

