#pragma once
#include <QObject>
#include <QProcess>
#include <QString>

class LogExporter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)

public:
    explicit LogExporter(QObject *parent = nullptr);

    Q_INVOKABLE void exportLogs(const QString &path);  // Trigger from QML
    bool busy() const { return m_busy; }
    QString statusMessage() const { return m_statusMessage; }

signals:
    void busyChanged();
    void statusMessageChanged();
    void exportFinished(bool success, const QString &message);

private slots:
    void onExportFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    void setBusy(bool busy);
    void setStatusMessage(const QString &msg);

    QProcess m_process;
    bool m_busy = false;
    QString m_statusMessage;
};
