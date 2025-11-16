#ifndef ETHERNETNETWORKCONROLLER_H
#define ETHERNETNETWORKCONROLLER_H

#include <QObject>
#include <QProcess>
#include <QTimer>
#include <QString>
#include <QStringList>

class EthernetNetworkConroller : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString interface READ interface WRITE setInterface NOTIFY interfaceChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString ipAddress READ ipAddress WRITE setIpAddress NOTIFY ipAddressChanged)
    Q_PROPERTY(QString linkSpeed READ linkSpeed NOTIFY linkSpeedChanged)
    Q_PROPERTY(bool dhcpRunning READ dhcpRunning NOTIFY dhcpRunningChanged)
    Q_PROPERTY(QString macAddress READ macAddress NOTIFY macAddressChanged)
    Q_PROPERTY(QStringList dnsRecords READ dnsRecords WRITE setDnsRecords NOTIFY dnsRecordsChanged FINAL)
    Q_PROPERTY(QString subnetMask READ subnetMask WRITE setSubnetMask NOTIFY subnetMaskChanged)
    Q_PROPERTY(QString gateway READ gateway WRITE setGateway NOTIFY gatewayChanged FINAL)
public:
    explicit EthernetNetworkConroller(QObject *parent = nullptr);

    QString interface() const { return m_interface; }
    void setInterface(const QString &ifname);

    QString status() const { return m_status; }
    QString ipAddress() const { return m_ipAddress; }
    QString linkSpeed() const { return m_speed; }
    bool dhcpRunning() const { return m_dhcpRunning; }
    QString macAddress() const { return m_mac; }

    //void fetchDns();
    Q_INVOKABLE void enableDhcp();

    QString subnetMask() const;
    void setSubnetMask(const QString &newSubnetMask);

    QString gateway() const;
    void setGateway(const QString &newGateway);

    QStringList dnsRecords() const;
    void setDnsRecords(const QStringList &newDnsRecords);

    void setIpAddress(const QString &newIpAddress);

public slots:
    void startDhcp();
    void stopDhcp();
    void refreshStatus();
    void connectClicked();
    void disconnectClicked();
    void applyStaticConfig(
        const QString &ip,
        int cidrMask,
        const QString &gateway,
        const QString &dns1,
        const QString &dns2);

    int maskToCidr(const QString &mask);


signals:
    void interfaceChanged();
    void statusChanged();
    void ipAddressChanged();
    void linkSpeedChanged();
    void dhcpRunningChanged();
    void logMessage(const QString &msg);
    void macAddressChanged();
    void subnetMaskChanged();
    void gatewayChanged();

    void dnsRecordsChanged();

private slots:
    void onProcessOutput();
    void onProcessError(QProcess::ProcessError err);

private:
    QString runCommandCollect(const QString &program, const QStringList &args, int timeoutMs = 3000);
    QString m_interface{"eth0"};
    QString m_status{"unknown"};
    QString m_speed;
    bool m_dhcpRunning{false};
    QProcess m_proc;
    QTimer m_pollTimer;
    QString m_mac;
    QString m_subnetMask;
    QString m_gateway;
    QStringList m_dnsRecords;
    QString m_ipAddress;
};

#endif // ETHERNETNETWORKCONROLLER_H
