#ifndef WIFINETWORKDETAILS_H
#define WIFINETWORKDETAILS_H

#include <QObject>

class WifiNetworkDetails : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString active READ active WRITE setActive NOTIFY sigActiveChanged FINAL)
    Q_PROPERTY(QString ssid READ ssid WRITE setSsid NOTIFY sigSsidChanged FINAL)
    Q_PROPERTY(QString bars READ bars WRITE setBars NOTIFY sigBarsChanged FINAL)

    QString m_active;
    QString m_ssid;
    QString m_bars;
public:
    explicit WifiNetworkDetails(QObject *parent = nullptr, QString active="", QString ssid="", QString bars="");

    QString active() const;
    void setActive(const QString &newActive);

    QString ssid() const;
    void setSsid(const QString &newSsid);

    QString bars() const;
    void setBars(const QString &newBars);

signals:
    void sigActiveChanged(QString active);
    void sigSsidChanged(QString ssid);
    void sigBarsChanged(QString bars);
};

#endif // WIFINETWORKDETAILS_H
