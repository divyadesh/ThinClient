/*!
 *  \file WifiNetworkDetails.cpp
 *  \brief Implementation of WifiNetworkDetails, representing a single Wi-Fi entry.
 */

#include "WifiNetworkDetails.h"
#include <QDebug>

/* -------------------------------------------------------------------------- */
/*                               CONSTRUCTOR                                  */
/* -------------------------------------------------------------------------- */

/*!
 *  \brief Constructs a WifiNetworkDetails object with the given properties.
 *
 *  \param parent   Parent QObject.
 *  \param active   "yes" if active, otherwise "no".
 *  \param ssid     Wi-Fi SSID.
 *  \param bars     Numeric signal strength.
 *  \param security True if network is secured (WPA/WPA2).
 *  \param bssid    Access Point MAC address.
 *  \param chan     Channel number.
 *  \param rate     Connection rate (e.g., "130 Mbit/s").
 */
WifiNetworkDetails::WifiNetworkDetails(QObject *parent,
                                       QString active,
                                       QString ssid,
                                       int bars,
                                       bool security,
                                       QString bssid,
                                       int chan,
                                       QString rate)
    : QObject{parent}
{
    setActive(active);
    setSsid(ssid);
    setBars(bars);
    setSecurity(security);
    setBssid(bssid);
    setChan(chan);
    setRate(rate);
}

/* -------------------------------------------------------------------------- */
/*                               ACTIVE PROPERTY                              */
/* -------------------------------------------------------------------------- */

/*!
 *  \brief Returns whether this Wi-Fi entry is active ("yes"/"no").
 */
QString WifiNetworkDetails::active() const
{
    return m_active;
}

/*!
 *  \brief Sets the active state of this Wi-Fi entry.
 *  \param newActive "yes" or "no".
 */
void WifiNetworkDetails::setActive(const QString &newActive)
{
    if (m_active == newActive)
        return;

    m_active = newActive;
    emit sigActiveChanged(m_active);
}

/* -------------------------------------------------------------------------- */
/*                                SSID PROPERTY                               */
/* -------------------------------------------------------------------------- */

/*!
 *  \brief Returns the SSID of this Wi-Fi network.
 */
QString WifiNetworkDetails::ssid() const
{
    return m_ssid;
}

/*!
 *  \brief Sets the SSID of the Wi-Fi network.
 *  \param newSsid New SSID value.
 */
void WifiNetworkDetails::setSsid(const QString &newSsid)
{
    if (m_ssid == newSsid)
        return;

    m_ssid = newSsid;
    emit sigSsidChanged(m_ssid);
}

/* -------------------------------------------------------------------------- */
/*                             SIGNAL BARS PROPERTY                            */
/* -------------------------------------------------------------------------- */

/*!
 *  \brief Returns the numeric signal-bar strength of the Wi-Fi network.
 */
int WifiNetworkDetails::bars() const
{
    return m_bars;
}

/*!
 *  \brief Sets the signal-bar value for the Wi-Fi network.
 *  \param newBars New signal-bar strength.
 */
void WifiNetworkDetails::setBars(const int &newBars)
{
    if (m_bars == newBars)
        return;

    m_bars = newBars;
    emit sigBarsChanged(m_bars);
}

/* -------------------------------------------------------------------------- */
/*                            SECURITY PROPERTY                               */
/* -------------------------------------------------------------------------- */

/*!
 *  \brief Returns whether the Wi-Fi network is secured.
 */
bool WifiNetworkDetails::security() const
{
    return m_security;
}

/*!
 *  \brief Sets the security flag.
 *  \param newSecurity True if WPA/WPA2 secured.
 */
void WifiNetworkDetails::setSecurity(bool newSecurity)
{
    if (m_security == newSecurity)
        return;

    m_security = newSecurity;
    emit sigSecurityChanged();
}

/* -------------------------------------------------------------------------- */
/*                               BSSID PROPERTY                               */
/* -------------------------------------------------------------------------- */

/*!
 *  \brief Returns the BSSID (MAC address) of the access point.
 */
QString WifiNetworkDetails::bssid() const
{
    return m_bssid;
}

/*!
 *  \brief Sets the BSSID value.
 *  \param newBssid Access point MAC address.
 */
void WifiNetworkDetails::setBssid(const QString &newBssid)
{
    if (m_bssid == newBssid)
        return;

    m_bssid = newBssid;
    emit sigBssidChanged();
}

/* -------------------------------------------------------------------------- */
/*                               CHANNEL PROPERTY                             */
/* -------------------------------------------------------------------------- */

/*!
 *  \brief Returns the channel number of the Wi-Fi network.
 */
int WifiNetworkDetails::chan() const
{
    return m_chan;
}

/*!
 *  \brief Sets the Wi-Fi channel.
 *  \param newChan Channel number.
 */
void WifiNetworkDetails::setChan(int newChan)
{
    if (m_chan == newChan)
        return;

    m_chan = newChan;
    emit sigChanChanged();
}

/* -------------------------------------------------------------------------- */
/*                                RATE PROPERTY                               */
/* -------------------------------------------------------------------------- */

/*!
 *  \brief Returns the connection rate (e.g., "130 Mbit/s").
 */
QString WifiNetworkDetails::rate() const
{
    return m_rate;
}

/*!
 *  \brief Sets the connection rate string.
 *  \param newRate New bitrate string.
 */
void WifiNetworkDetails::setRate(const QString &newRate)
{
    if (m_rate == newRate)
        return;

    m_rate = newRate;
    emit sigRateChanged();
}
