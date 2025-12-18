#pragma once

#include <QObject>
#include <QString>

class EthernetNetworkInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString interface READ interface NOTIFY changed)
    Q_PROPERTY(QString status    READ status    NOTIFY changed)
    Q_PROPERTY(QString method    READ method    NOTIFY changed)
    Q_PROPERTY(QString mac       READ mac       NOTIFY changed)
    Q_PROPERTY(QString ipAddress READ ipAddress NOTIFY changed)
    Q_PROPERTY(QString subnet    READ subnet    NOTIFY changed)
    Q_PROPERTY(QString gateway   READ gateway   NOTIFY changed)
    Q_PROPERTY(QString dns1      READ dns1      NOTIFY changed)
    Q_PROPERTY(QString dns2      READ dns2      NOTIFY changed)
    Q_PROPERTY(bool isBusy READ isBusy WRITE setIsBusy NOTIFY isBusyChanged FINAL)

public:
    explicit EthernetNetworkInfo(QObject *parent = nullptr);

    Q_INVOKABLE void updateEthernetNetworkInfo();
    Q_INVOKABLE void switchToDhcpAsync();

    Q_INVOKABLE void switchToStaticAsync(const QString &ip,
                                         const QString &subnetMask,
                                         const QString &gateway,
                                         const QString &dns1,
                                         const QString &dns2);

    QString interface() const { return m_interface; }
    QString status() const    { return m_status; }
    QString method() const    { return m_method; }
    QString mac() const       { return m_mac; }
    QString ipAddress() const { return m_ip; }
    QString subnet() const    { return m_subnet; }
    QString gateway() const   { return m_gateway; }
    QString dns1() const      { return m_dns1; }
    QString dns2() const      { return m_dns2; }

    bool isBusy() const;
    void setIsBusy(bool newIsBusy);

signals:
    void changed();
    void operationStarted();
    void operationFinished(bool success, const QString &message);

    void isBusyChanged();

private:
    void clear();
    QString readDefaultGateway(const QString &iface) const;

private:
    QString m_interface{"eth0"};

    QString m_status;
    QString m_method;
    QString m_mac;
    QString m_ip;
    QString m_subnet;
    QString m_gateway;
    QString m_dns1;
    QString m_dns2;
    bool m_isBusy{false};
};
