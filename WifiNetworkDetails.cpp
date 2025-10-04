#include "WifiNetworkDetails.h"
#include "QDebug"

WifiNetworkDetails::WifiNetworkDetails(QObject *parent, QString active, QString ssid, int bars, bool security, QString bssid, int chan, QString rate)
    : QObject{parent} {
    setActive(active);
    setSsid(ssid);
    setBars(bars);
    setSecurity(security);
    setBssid(bssid);
    setChan(chan);
    setRate(rate);
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

int WifiNetworkDetails::bars() const {
    return m_bars;
}

void WifiNetworkDetails::setBars(const int &newBars) {
    if (m_bars == newBars)
        return;
    m_bars = newBars;
    emit sigBarsChanged(m_bars);
}

bool WifiNetworkDetails::security() const {
    return m_security;
}

void WifiNetworkDetails::setSecurity(bool newSecurity) {
    if (m_security == newSecurity)
        return;
    m_security = newSecurity;
    emit sigSecurityChanged();
}

QString WifiNetworkDetails::bssid() const {
    return m_bssid;
}

void WifiNetworkDetails::setBssid(const QString &newBssid) {
    if (m_bssid == newBssid)
        return;
    m_bssid = newBssid;
    emit sigBssidChanged();
}

int WifiNetworkDetails::chan() const {
    return m_chan;
}

void WifiNetworkDetails::setChan(int newChan) {
    if (m_chan == newChan)
        return;
    m_chan = newChan;
    emit sigChanChanged();
}

QString WifiNetworkDetails::rate() const {
    return m_rate;
}

void WifiNetworkDetails::setRate(const QString &newRate) {
    if (m_rate == newRate)
        return;
    m_rate = newRate;
    emit sigRateChanged();
}
