#pragma once
#include <QObject>
#include <QSocketNotifier>

class UdevMonitor : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool usbConnected READ usbConnected NOTIFY usbConnectedChanged)

public:
    explicit UdevMonitor(QObject *parent = nullptr);
    bool usbConnected() const { return m_connected; }

signals:
    void usbConnectedChanged();
    void usbEvent(const QString &action, const QString &device);

private:
    QSocketNotifier *notifier = nullptr;
    bool m_connected = false;
};
