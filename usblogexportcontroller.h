#pragma once

#include <QObject>
#include <QProcess>
#include <atomic>

class UsbLogExportController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)

public:
    explicit UsbLogExportController(QObject *parent = nullptr);
    ~UsbLogExportController();

    Q_INVOKABLE void exportLogs();
    Q_INVOKABLE void cancel();

    bool isBusy() const;

signals:
    void isBusyChanged();
    void progressChanged(const QString &message);
    void success(const QString &message);
    void error(const QString &message);

private:
    void setBusy(bool busy);

private:
    QProcess m_process;
    std::atomic_bool m_cancelRequested{false};
    bool m_isBusy{false};

    const QString m_exportScript{"/usr/bin/export_log_file.sh"};
};
