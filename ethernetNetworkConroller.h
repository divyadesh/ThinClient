#ifndef ETHERNETNETWORKCONROLLER_H
#define ETHERNETNETWORKCONROLLER_H

#include <QObject>
#include <QProcess>
#include <QTimer>

class EthernetNetworkConroller : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString interface READ interface WRITE setInterface NOTIFY interfaceChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString ipAddress READ ipAddress NOTIFY ipAddressChanged)
    Q_PROPERTY(QString linkSpeed READ linkSpeed NOTIFY linkSpeedChanged)
    Q_PROPERTY(bool dhcpRunning READ dhcpRunning NOTIFY dhcpRunningChanged)
    Q_PROPERTY(QString macAddress READ macAddress NOTIFY macAddressChanged)
    Q_PROPERTY(QString dns READ dns WRITE setDns NOTIFY dnsChanged)
    Q_PROPERTY(QString subnetMask READ subnetMask WRITE setSubnetMask NOTIFY subnetMaskChanged)
    Q_PROPERTY(QString gateway READ gateway WRITE setGateway NOTIFY gatewayChanged FINAL)
public:
    explicit EthernetNetworkConroller(QObject *parent = nullptr);

    QString interface() const { return m_interface; }
    void setInterface(const QString &ifname);

    QString status() const { return m_status; }
    QString ipAddress() const { return m_ip; }
    QString linkSpeed() const { return m_speed; }
    bool dhcpRunning() const { return m_dhcpRunning; }
    QString macAddress() const { return m_mac; }

    QString dns() const;
    void setDns(const QString &newDns);

    //void fetchDns();

    QString subnetMask() const;
    void setSubnetMask(const QString &newSubnetMask);

    QString gateway() const;
    void setGateway(const QString &newGateway);

public slots:
    void startDhcp();
    void stopDhcp();
    void refreshStatus();
    void connectClicked();
    void disconnectClicked();
    void setManualConfig(const QString &ip, int cidrMask, const QString &gateway, const QString &dns);

signals:
    void interfaceChanged();
    void statusChanged();
    void ipAddressChanged();
    void linkSpeedChanged();
    void dhcpRunningChanged();
    void logMessage(const QString &msg);
    void macAddressChanged();

    void dnsChanged();

    void subnetMaskChanged();

    void gatewayChanged();

private slots:
    void onProcessOutput();
    void onProcessError(QProcess::ProcessError err);

private:
    QString runCommandCollect(const QString &program, const QStringList &args, int timeoutMs = 3000);
    QString m_interface{"eth0"};
    QString m_status{"unknown"};
    QString m_ip;
    QString m_speed;
    bool m_dhcpRunning{false};
    QProcess m_proc;
    QTimer m_pollTimer;
    QString m_mac;
    QString m_dns;
    QString m_subnetMask;
    QString m_gateway;
};

#endif // ETHERNETNETWORKCONROLLER_H
