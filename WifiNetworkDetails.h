#ifndef WIFINETWORKDETAILS_H
#define WIFINETWORKDETAILS_H

#include <QObject>

/*!
 *  \class WifiNetworkDetails
 *  \brief Represents a single Wi-Fi network entry.
 *
 *  This class stores all information parsed from `nmcli device wifi list`.
 *  Each instance corresponds to a single Wi-Fi network visible in the scan.
 *
 *  Exposed QML properties:
 *    - active  → Whether this network is currently active ("yes"/"no")
 *    - ssid    → Network SSID
 *    - bars    → Signal strength (mapped to numeric strength levels)
 *    - security → True if the network is secured (WPA/WPA2)
 *    - bssid   → Access point MAC address
 *    - chan    → Wi-Fi channel number
 *    - rate    → Connection rate (bitrate)
 */
class WifiNetworkDetails : public QObject
{
    Q_OBJECT

    /*! \property QString active
        \brief Whether this Wi-Fi network is currently active ("yes" / "no"). */
    Q_PROPERTY(QString active READ active WRITE setActive NOTIFY sigActiveChanged FINAL)

    /*! \property QString ssid
        \brief SSID (name) of the Wi-Fi network. */
    Q_PROPERTY(QString ssid READ ssid WRITE setSsid NOTIFY sigSsidChanged FINAL)

    /*! \property int bars
        \brief Signal strength represented as numeric levels. */
    Q_PROPERTY(int bars READ bars WRITE setBars NOTIFY sigBarsChanged FINAL)

    /*! \property bool security
        \brief Whether the network is secured (WPA/WPA2). */
    Q_PROPERTY(bool security READ security WRITE setSecurity NOTIFY sigSecurityChanged FINAL)

    /*! \property QString bssid
        \brief Access point MAC address (BSSID). */
    Q_PROPERTY(QString bssid READ bssid WRITE setBssid NOTIFY sigBssidChanged FINAL)

    /*! \property int chan
        \brief Wi-Fi channel number. */
    Q_PROPERTY(int chan READ chan WRITE setChan NOTIFY sigChanChanged FINAL)

    /*! \property QString rate
        \brief Connection rate / bitrate (e.g., "130 Mbit/s"). */
    Q_PROPERTY(QString rate READ rate WRITE setRate NOTIFY sigRateChanged FINAL)

public:
    /*!
     *  \brief Constructs a WifiNetworkDetails object.
     *
     *  \param parent  Parent QObject.
     *  \param active  "yes" if active, otherwise "no".
     *  \param ssid    Wi-Fi SSID.
     *  \param bars    Signal strength bars (interpreted numerically).
     *  \param security True if secured (WPA/WPA2).
     *  \param bssid   MAC of access point.
     *  \param chan    Channel number.
     *  \param rate    Bitrate string.
     */
    explicit WifiNetworkDetails(QObject *parent = nullptr,
                                QString active = "",
                                QString ssid = "",
                                int bars = -1,
                                bool security = false,
                                QString bssid = "",
                                int chan = -1,
                                QString rate = "");

    //! \return "yes" or "no", indicating whether this network is active.
    QString active() const;
    //! Sets whether the network is currently active.
    void setActive(const QString &newActive);

    //! \return The SSID of the Wi-Fi network.
    QString ssid() const;
    //! Updates the SSID.
    void setSsid(const QString &newSsid);

    //! \return Numeric signal-bar representation.
    int bars() const;
    //! Updates the signal strength.
    void setBars(const int &newBars);

    //! \return True if the network is WPA secured.
    bool security() const;
    //! Updates the security state.
    void setSecurity(bool newSecurity);

    //! \return BSSID (MAC address of the access point).
    QString bssid() const;
    //! Updates the BSSID.
    void setBssid(const QString &newBssid);

    //! \return Wi-Fi channel number.
    int chan() const;
    //! Updates the channel value.
    void setChan(int newChan);

    //! \return Network bitrate string.
    QString rate() const;
    //! Updates the bitrate string.
    void setRate(const QString &newRate);

signals:
    /*! \brief Emitted when `active` property changes. */
    void sigActiveChanged(QString active);

    /*! \brief Emitted when `ssid` property changes. */
    void sigSsidChanged(QString ssid);

    /*! \brief Emitted when `bars` property changes. */
    void sigBarsChanged(int bars);

    /*! \brief Emitted when `security` property changes. */
    void sigSecurityChanged();

    /*! \brief Emitted when `bssid` property changes. */
    void sigBssidChanged();

    /*! \brief Emitted when `chan` property changes. */
    void sigChanChanged();

    /*! \brief Emitted when `rate` property changes. */
    void sigRateChanged();

private:
    QString m_active;  //!< "yes" if active, otherwise "no".
    QString m_ssid;    //!< SSID of the Wi-Fi network.
    int m_bars;        //!< Signal bars (converted to strength levels).
    bool m_security;   //!< True if the network is secured.
    QString m_bssid;   //!< MAC address of access point.
    int m_chan;        //!< Channel number.
    QString m_rate;    //!< Bitrate string.
};

#endif // WIFINETWORKDETAILS_H
