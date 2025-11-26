#pragma once
#include <QObject>
#include <QObject>
#include <QDBusInterface>
#include <QDBusVariant>

class WifiMonitor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)

public:
    explicit WifiMonitor(QObject *parent = nullptr);

    bool isConnected() const;
    void initialize();

public slots:
    void setConnected(bool connected);

signals:
    void connectedChanged(bool connected);
    void wifiConnected(QString ssid);
    void wifiDisconnected();
    void wifiStateChanged(QString state);

private slots:
    void nmStateChanged(uint newState);

private:
    bool m_connected;
    QString getActiveSSID();
};
