#pragma once
#include <QObject>
#include <QString>
#include <QStringList>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>

class DNSNetworkInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString ipAddress READ ipAddress NOTIFY infoChanged)
    Q_PROPERTY(QString netmask READ netmask NOTIFY infoChanged)
    Q_PROPERTY(QString gateway READ gateway NOTIFY infoChanged)
    Q_PROPERTY(QString broadcast READ broadcast NOTIFY infoChanged)
    Q_PROPERTY(QStringList dnsServers READ dnsServers NOTIFY infoChanged)

public:
    explicit DNSNetworkInfo(QObject *parent = nullptr, const QString &iface = "eth0");

    QString ipAddress() const { return m_ipAddress; }
    QString netmask() const { return m_netmask; }
    QString gateway() const { return m_gateway; }
    QString broadcast() const { return m_broadcast; }
    QStringList dnsServers() const { return m_dnsServers; }

public slots:
    void updateInfo();  // trigger refresh (non-blocking)

signals:
    void infoChanged();

private:
    QString m_iface;
    QString m_ipAddress;
    QString m_netmask;
    QString m_gateway;
    QString m_broadcast;
    QStringList m_dnsServers;

    struct NetData {
        QString ip;
        QString mask;
        QString gateway;
        QString broadcast;
        QStringList dns;
    };

    NetData collectInfo();  // runs in background
};
