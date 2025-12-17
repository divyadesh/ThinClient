#pragma once

#include <QObject>
#include <QString>

class WifiNetworkInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString interface READ interface NOTIFY changed)
    Q_PROPERTY(QString ssid READ ssid NOTIFY changed)
    Q_PROPERTY(QString method READ method NOTIFY changed)        // "DHCP" or "Static"
    Q_PROPERTY(QString ipAddress READ ipAddress NOTIFY changed)
    Q_PROPERTY(QString subnet READ subnet NOTIFY changed)
    Q_PROPERTY(QString gateway READ gateway NOTIFY changed)
    Q_PROPERTY(QString dns1 READ dns1 NOTIFY changed)
    Q_PROPERTY(QString dns2 READ dns2 NOTIFY changed)

    Q_PROPERTY(QString status   READ status   NOTIFY changed)
    Q_PROPERTY(QString security READ security NOTIFY changed)
    Q_PROPERTY(QString mac      READ mac      NOTIFY changed)

public:
    explicit WifiNetworkInfo(QObject *parent = nullptr);

    Q_INVOKABLE void updateWifiNetworkInfo();

    QString interface() const { return m_interface; }
    QString ssid() const { return m_ssid; }
    QString method() const { return m_method; }
    QString ipAddress() const { return m_ip; }
    QString subnet() const { return m_subnet; }
    QString gateway() const { return m_gateway; }
    QString dns1() const { return m_dns1; }
    QString dns2() const { return m_dns2; }

    QString status() const   { return m_status; }
    QString security() const { return m_security; }
    QString mac() const      { return m_mac; }

signals:
    void changed();

private:
    void clear();
    QString cidrToNetmask(int prefix) const;

private:

    QString readDefaultGateway(const QString &iface) const;

    QString m_interface{"p2p0"};
    QString m_ssid;
    QString m_method;
    QString m_ip;
    QString m_subnet;
    QString m_gateway;
    QString m_dns1;
    QString m_dns2;

    QString m_status;
    QString m_security;
    QString m_mac;
};
