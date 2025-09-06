#ifndef WIFINETWORKDETAILS_H
#define WIFINETWORKDETAILS_H

#include <QObject>

class WifiNetworkDetails : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString active READ active WRITE setActive NOTIFY sigActiveChanged FINAL)
    Q_PROPERTY(QString ssid READ ssid WRITE setSsid NOTIFY sigSsidChanged FINAL)
    Q_PROPERTY(int bars READ bars WRITE setBars NOTIFY sigBarsChanged FINAL)
    Q_PROPERTY(bool security READ security WRITE setSecurity NOTIFY sigSecurityChanged FINAL)
    Q_PROPERTY(QString bssid READ bssid WRITE setBssid NOTIFY sigBssidChanged FINAL)
    Q_PROPERTY(int chan READ chan WRITE setChan NOTIFY sigChanChanged FINAL)
    Q_PROPERTY(QString rate READ rate WRITE setRate NOTIFY sigRateChanged FINAL)

    QString m_active;
    QString m_ssid;
    int m_bars;
    bool m_security;
    QString m_bssid;
    int m_chan;
    QString m_rate;
public:
    explicit WifiNetworkDetails(QObject *parent = nullptr, QString active="", QString ssid="", int bars=-1, bool security = false,
                                QString bssid = "", int chan=-1, QString rate="");

    QString active() const;
    void setActive(const QString &newActive);

    QString ssid() const;
    void setSsid(const QString &newSsid);

    int bars() const;
    void setBars(const int &newBars);

    bool security() const;
    void setSecurity(bool newSecurity);

    QString bssid() const;
    void setBssid(const QString &newBssid);

    int chan() const;
    void setChan(int newChan);

    QString rate() const;
    void setRate(const QString &newRate);

signals:
    void sigActiveChanged(QString active);
    void sigSsidChanged(QString ssid);
    void sigBarsChanged(int bars);
    void sigSecurityChanged();
    void sigBssidChanged();
    void sigChanChanged();
    void sigRateChanged();
};

#endif // WIFINETWORKDETAILS_H
