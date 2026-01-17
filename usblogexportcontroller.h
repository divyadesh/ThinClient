#pragma once

#include <QObject>
#include <atomic>

class UsbLogExportController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)

public:
    explicit UsbLogExportController(QObject *parent = nullptr);

    Q_INVOKABLE void exportLogs();
    Q_INVOKABLE void cancel();

    bool isBusy() const;

signals:
    void isBusyChanged();
    void progressChanged(const QString &msg);
    void success(const QString &msg);
    void error(const QString &msg);

private:
    bool mountDevice(const QString &device, const QString &mountPoint, QString &err);
    bool unmountDevice(const QString &mountPoint, QString &err);
    bool copyLogs(const QString &mountPoint, QString &err);

    void setBusy(bool busy);

private:
    std::atomic_bool m_cancel{false};
    bool m_busy{false};

    const QString m_mountPoint{"/mnt/usb"};
};
