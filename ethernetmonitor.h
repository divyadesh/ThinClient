#pragma once

#include <QObject>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusObjectPath>
#include <QDBusReply>

/**
 * @brief Unified Ethernet Monitor
 *
 * Monitors Ethernet using:
 *   1) Sysfs link detection (/sys/class/net/<iface>/carrier)
 *   2) NetworkManager DBus (if NM manages eth0)
 *
 * Exposes:
 *   Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
 *
 * Emits:
 *   - cablePlugged()
 *   - cableUnplugged()
 *   - ethernetConnected(ip)
 *   - ethernetDisconnected()
 *   - stateChanged("connected"/"disconnected"/"link-up"/"link-down")
 */
class EthernetMonitor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
    Q_PROPERTY(QString ipAddress READ ipAddress NOTIFY ipAddressChanged)

public:
    explicit EthernetMonitor(QObject *parent = nullptr);

    bool isConnected() const { return m_connected; }
    QString ipAddress() const { return m_ipAddress; }

public slots:
    void start();
    void checkCarrier();
    void nmStateChanged(uint newState);
    void fetchNmIp();

signals:
    void connectedChanged(bool connected);
    void ipAddressChanged(QString ip);

    void cablePlugged();
    void cableUnplugged();
    void ethernetConnected(QString ip);
    void ethernetDisconnected();
    void stateChanged(QString state);

private:
    bool isNmManaged();
    QString carrierPath() const;

private:
    QString m_iface = "eth0";
    QString m_carrierFile;

    QFileSystemWatcher m_carrierWatcher;

    bool m_linkUp = false;
    bool m_connected = false;
    QString m_ipAddress;
};
