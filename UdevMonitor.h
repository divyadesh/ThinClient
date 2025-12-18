#pragma once
#include <QObject>
#include <QSocketNotifier>
#include <QString>

class UdevMonitor : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool usbConnected READ usbConnected NOTIFY usbConnectedChanged)
    Q_PROPERTY(QString usbStoragePort READ usbStoragePort WRITE setUsbStoragePort NOTIFY usbStoragePortChanged FINAL)

public:
    explicit UdevMonitor(QObject *parent = nullptr);
    bool usbConnected() const { return m_connected; }

    QString usbStoragePort() const;
    void setUsbStoragePort(QString newUsbStoragePort);

signals:
    void usbConnectedChanged();
    void usbEvent(const QString &action, const QString &device);
    void usbStoragePortChanged();

public slots:
    void handleEthernetEvent(const QString &interfaceName,
                       const QString &state,
                       const QString &action);


private:
    QSocketNotifier *notifier = nullptr;
    bool m_connected = false;
    QString m_usbStoragePort;
};
