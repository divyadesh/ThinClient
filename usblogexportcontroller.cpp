#include "usblogexportcontroller.h"

#include <QFile>
#include <QDebug>
#include "Application.h"
#include "UdevMonitor.h"

UsbLogExportController::UsbLogExportController(QObject *parent)
    : QObject(parent)
{
    connect(&m_process,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            [this](int exitCode, QProcess::ExitStatus status) {

                setBusy(false);

                if (m_cancelRequested.load(std::memory_order_relaxed)) {
                    emit error("Log export cancelled.");
                    return;
                }

                if (status == QProcess::NormalExit && exitCode == 0) {
                    emit success("Logs exported successfully.");
                } else {
                    const QString stderrOutput =
                        QString::fromUtf8(m_process.readAllStandardError());
                    qWarning() << "Export failed:" << stderrOutput;
                    emit error("Failed to export logs to USB drive.");
                }
            });
}

UsbLogExportController::~UsbLogExportController()
{
    cancel();
}

bool UsbLogExportController::isBusy() const
{
    return m_isBusy;
}

void UsbLogExportController::setBusy(bool busy)
{
    if (m_isBusy == busy)
        return;

    m_isBusy = busy;
    emit isBusyChanged();
}

void UsbLogExportController::cancel()
{
    m_cancelRequested.store(true, std::memory_order_relaxed);

    if (m_process.state() == QProcess::Running) {
        m_process.kill();
    }
}

void UsbLogExportController::exportLogs()
{
    if (m_isBusy) {
        qWarning() << "USB log export already in progress";
        return;
    }

    const QString devicePath =
        Application::usbMonitor()->usbStoragePort(); // e.g. /dev/sdb1

    if (devicePath.isEmpty()) {
        emit error("No USB storage device detected.");
        return;
    }

    if (!QFile::exists(m_exportScript)) {
        emit error("Log export script not found.");
        return;
    }

    m_cancelRequested.store(false, std::memory_order_relaxed);
    setBusy(true);

    emit progressChanged("Starting log export…");

    QStringList args;
    args << m_exportScript << devicePath;

    m_process.start("sh", args);

    if (!m_process.waitForStarted(2000)) {
        setBusy(false);
        emit error("Failed to start log export process.");
        return;
    }

    emit progressChanged("Exporting logs to USB drive… Please do not remove the USB device.");
}
