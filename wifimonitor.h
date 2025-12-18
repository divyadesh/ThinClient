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
    void deviceStateChanged(uint newState, uint oldState, uint reason);

signals:
    void connectedChanged(bool connected);
    void wifiConnected(QString ssid);
    void wifiDisconnected();
    void wifiStateChanged(QString state);

    void infoMessage(const QString &msg);
    void warningMessage(const QString &msg);
    void errorMessage(const QString &msg);
    void successMessage(const QString &msg);

    void wifiError(const QString &errorCode);      // e.g. "auth-failed", "ssid-not-found"
    void wifiAuthFailed();                         // wrong/invalid password or auth issue

private slots:
    void nmStateChanged(uint newState);

private:
    bool m_connected;
    QString m_wifiDevicePath;
    QString getActiveSSID();
    bool isAuthError(uint reason) const;
    bool isSsidNotFound(uint reason) const;
    QString reasonToString(uint reason) const;
    QString reasonToUserMessage(uint reason) const;

    void handleActivated(uint reason);
    void handleDisconnected(uint reason);
    void handleFailed(uint reason);

    bool isUserInitiatedDisconnect(uint reason) const;

};
