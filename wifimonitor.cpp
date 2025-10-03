#include "wifimonitor.h"

WifiMonitor::WifiMonitor(QObject *parent)
    : QObject(parent), m_connected(false)
{}

bool WifiMonitor::isConnected() const
{
    return m_connected;
}

void WifiMonitor::setConnected(bool connected)
{
    if (m_connected != connected) {
        m_connected = connected;
        emit connectedChanged(m_connected);
    }
}

