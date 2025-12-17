#pragma once

#include <QObject>
#include <QString>

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/WirelessDevice>

class WifiConfigManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)

public:
    explicit WifiConfigManager(QObject *parent = nullptr);

    Q_INVOKABLE bool setDhcp();
    Q_INVOKABLE bool setStatic(const QString &ip,
                               const QString &subnet,
                               const QString &gateway,
                               const QString &dns1 = QString(),
                               const QString &dns2 = QString());

    // Wi-Fi control
    Q_INVOKABLE bool connectToSsid(const QString &ssid,
                                   const QString &password = QString());
    Q_INVOKABLE bool disconnect();
    Q_INVOKABLE bool forget();


    bool isBusy() const { return m_isBusy; }
    QString lastError() const { return m_lastError; }

signals:
    void isBusyChanged();
    void lastErrorChanged();
    void success();
    void failed(const QString &error);

private:
    QString m_interface{"p2p0"};
    bool m_isBusy{false};
    QString m_lastError;

    NetworkManager::WirelessDevice::Ptr wifiDevice();
    bool updateConnection(NetworkManager::Connection::Ptr &conn);
    void setBusy(bool busy);
    void setError(const QString &error);
};
