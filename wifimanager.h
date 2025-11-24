#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>

class WiFiManager : public QObject
{
    Q_OBJECT

    // --------- BASIC INFO ----------
    Q_PROPERTY(QString ssid READ ssid WRITE setSsid NOTIFY ssidChanged)
    Q_PROPERTY(QString security READ security NOTIFY securityChanged)
    Q_PROPERTY(QString password READ password NOTIFY passwordChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(int signalStrength READ signalStrength NOTIFY signalStrengthChanged)
    Q_PROPERTY(QString linkSpeed READ linkSpeed NOTIFY linkSpeedChanged)
    Q_PROPERTY(QString macAddress READ macAddress NOTIFY macAddressChanged)
    Q_PROPERTY(bool isStaticIp READ isStaticIp WRITE setIsStaticIp NOTIFY isStaticIpChanged)

    // --------- IP DETAILS ----------
    Q_PROPERTY(QString ipAddress READ ipAddress NOTIFY ipAddressChanged)
    Q_PROPERTY(QString subnetMask READ subnetMask NOTIFY subnetMaskChanged)
    Q_PROPERTY(QString gateway READ gateway NOTIFY gatewayChanged)
    Q_PROPERTY(QStringList dnsServers READ dnsServers NOTIFY dnsServersChanged)

public:
    explicit WiFiManager(QObject *parent = nullptr);

    // getters
    QString ssid() const;
    QString security() const;
    QString password() const;
    QString status() const;
    int signalStrength() const;
    QString linkSpeed() const;
    QString macAddress() const;

    QString ipAddress() const;
    QString subnetMask() const;
    QString gateway() const;
    QStringList dnsServers() const;

    // setters
    void setSsid(const QString &ssid);

    bool isStaticIp() const;

    void setIsStaticIp(bool newIsStaticIp);

    void runAsync(
        const QString &cmd,
        const QStringList &args,
        std::function<void(QString)> callback);

public slots:
    void refresh();
    void startAutoRefresh();
    void stopAutoRefresh();
    bool setStaticIp(const QString &ip,
                     const QString &subnetMask,
                     const QString &gateway,
                     const QStringList &dns);
    bool setDhcp();
    void updateIpMode();

signals:
    void ipModeChanged();

    void processStarted();
    void processEnded();

    void ssidChanged();
    void securityChanged();
    void passwordChanged();
    void statusChanged();
    void signalStrengthChanged();
    void linkSpeedChanged();
    void macAddressChanged();

    void ipAddressChanged();
    void subnetMaskChanged();
    void gatewayChanged();
    void dnsServersChanged();

    void logMessage(const QString &message);

    void isStaticIpChanged();

private:
    QString run(const QString &cmd, const QStringList &args) const;
    QString cidrToNetmask(int cidr) const;

private:
    QString getCurrentConnectionName() const;
    QString m_ssid;
    QString m_security;
    QString m_password;

    QString m_status;
    int m_signalStrength;
    QString m_linkSpeed;
    QString m_macAddress;

    QString m_ipAddress;
    QString m_subnetMask;
    QString m_gateway;
    QStringList m_dnsServers;

    QTimer m_timer;
    bool m_isStaticIp;
};

#endif // WIFI_MANAGER_H
