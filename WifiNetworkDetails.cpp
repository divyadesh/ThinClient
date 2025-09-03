#include "WifiNetworkDetails.h"
#include "QDebug"

WifiNetworkDetails::WifiNetworkDetails(QObject *parent, QString active, QString ssid, QString bars)
    : QObject{parent} {
    setActive(active);
    setSsid(ssid);
    setBars(bars);
}


QString WifiNetworkDetails::active() const {
    return m_active;
}

void WifiNetworkDetails::setActive(const QString &newActive) {
    if (m_active == newActive)
        return;
    m_active = newActive;
    emit sigActiveChanged(m_active);
}

QString WifiNetworkDetails::ssid() const {
    return m_ssid;
}

void WifiNetworkDetails::setSsid(const QString &newSsid) {
    if (m_ssid == newSsid)
        return;
    m_ssid = newSsid;
    emit sigSsidChanged(m_ssid);
}

QString WifiNetworkDetails::bars() const {
    return m_bars;
}

void WifiNetworkDetails::setBars(const QString &newBars) {
    if (m_bars == newBars)
        return;
    m_bars = newBars;
    emit sigBarsChanged(m_bars);
}
