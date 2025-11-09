#include "logexporter.h"
#include <QDebug>
#include <QFile>
#include <QProcess>

LogExporter::LogExporter(QObject *parent)
    : QObject(parent)
{
    connect(&m_process,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            &LogExporter::onExportFinished);
}

void LogExporter::exportLogs(const QString &devicePath)
{
    if (m_busy) {
        qWarning() << "Export already running.";
        return;
    }

    const QString scriptPath = "/usr/bin/export_log_file.sh";

    if (!QFile::exists(scriptPath)) {
        setStatusMessage("Export script not found.");
        emit exportFinished(false, m_statusMessage);
        return;
    }

    if (devicePath.isEmpty()) {
        setStatusMessage("No device path provided (e.g. /dev/sdb1).");
        emit exportFinished(false, m_statusMessage);
        return;
    }

    setBusy(true);
    setStatusMessage("🔄 Starting log export...");

    // --- Pass the device path as argument to the script ---
    QStringList args;
    args << scriptPath << devicePath;

    m_process.start("sh", args);

    if (!m_process.waitForStarted(2000)) {
        setStatusMessage("Failed to start export process.");
        setBusy(false);
        emit exportFinished(false, m_statusMessage);
        return;
    }

    // Optional: connect to process finished signal
    connect(&m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this](int exitCode, QProcess::ExitStatus status) {
                Q_UNUSED(status)
                bool success = (exitCode == 0);
                setBusy(false);
                setStatusMessage(success ? "Log export completed successfully." : "Log export failed.");
                emit exportFinished(success, m_statusMessage);
            });
}

void LogExporter::onExportFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    bool success = (exitStatus == QProcess::NormalExit && exitCode == 0);

    if (success) {
        setStatusMessage("✅ Log export completed successfully.");
    } else {
        setStatusMessage("❌ Log export failed.");
    }

    setBusy(false);
    emit exportFinished(success, m_statusMessage);
}

void LogExporter::setBusy(bool busy)
{
    if (m_busy != busy) {
        m_busy = busy;
        emit busyChanged();
    }
}

void LogExporter::setStatusMessage(const QString &msg)
{
    if (m_statusMessage != msg) {
        m_statusMessage = msg;
        emit statusMessageChanged();
    }
}
