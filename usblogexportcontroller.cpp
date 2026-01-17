#include "usblogexportcontroller.h"

#include <QFile>
#include <QDebug>
#include "Application.h"
#include "UdevMonitor.h"
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QDebug>

#include <sys/mount.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

static bool waitForBlockReady(const QString &device, int timeoutMs = 3000)
{
    const int step = 100;
    int waited = 0;

    while (waited < timeoutMs) {
        if (QFile::exists(device))
            return true;

        QThread::msleep(step);
        waited += step;
    }
    return false;
}

// ---- filesystem detection helper (C++ only) ----
static QString detectFsType(const QString &device)
{
    QProcess p;
    p.start("blkid", {"-o", "value", "-s", "TYPE", device});
    if (!p.waitForFinished(1500))
        return QString();

    return QString::fromUtf8(p.readAllStandardOutput()).trimmed();
}

UsbLogExportController::UsbLogExportController(QObject *parent)
    : QObject(parent)
{
}

bool UsbLogExportController::isBusy() const
{
    return m_busy;
}

void UsbLogExportController::setBusy(bool busy)
{
    if (m_busy == busy)
        return;
    m_busy = busy;
    emit isBusyChanged();
}

void UsbLogExportController::cancel()
{
    m_cancel.store(true, std::memory_order_relaxed);
}

void UsbLogExportController::exportLogs()
{
    if (m_busy)
        return;

    const QString device = Application::usbMonitor()->usbStoragePort(); // /dev/sdb1

    if (device.isEmpty()) {
        emit error("No USB storage device detected.");
        return;
    }

    if (!waitForBlockReady(device)) {
        emit error("USB device not ready. Please try again.");
        return;
    }

    setBusy(true);
    m_cancel.store(false);

    QString err;

    emit progressChanged("Mounting USB drive…");

    if (!mountDevice(device, m_mountPoint, err)) {
        setBusy(false);
        emit error(err);
        return;
    }

    emit progressChanged("Copying logs to USB drive…");

    if (!copyLogs(m_mountPoint, err)) {
        unmountDevice(m_mountPoint, err);
        setBusy(false);
        emit error(err);
        return;
    }

    emit progressChanged("Finalizing…");

    unmountDevice(m_mountPoint, err);

    setBusy(false);
    emit success("Logs exported successfully.");
}

bool UsbLogExportController::mountDevice(const QString &device,
                                         const QString &mountPoint,
                                         QString &err)
{
    QDir().mkpath(mountPoint);

    const QString fsType = detectFsType(device);
    if (fsType.isEmpty()) {
        err = "Unable to detect filesystem on USB device.";
        return false;
    }

    qInfo() << "Detected filesystem:" << fsType;

    QByteArray dev = device.toUtf8();
    QByteArray mnt = mountPoint.toUtf8();
    QByteArray fs  = fsType.toUtf8();
    QByteArray opts;

    unsigned long flags = MS_RELATIME;   // RW is default

    if (fsType == "vfat" || fsType == "fat" || fsType == "msdos") {
        // IMPORTANT: NO "rw" HERE
        opts = "uid=0,gid=0,fmask=0000,dmask=0000";
    } else if (fsType == "ext4" || fsType == "ext3" || fsType == "ext2") {
        opts.clear(); // no data options needed
    } else {
        err = QString("Unsupported filesystem: %1").arg(fsType);
        return false;
    }

    if (::mount(dev.constData(),
                mnt.constData(),
                fs.constData(),
                flags,
                opts.isEmpty() ? nullptr : opts.constData()) != 0) {

        err = QString("Mount failed (%1): %2")
        .arg(fsType)
            .arg(strerror(errno));
        return false;
    }

    return true;
}

bool UsbLogExportController::unmountDevice(const QString &mountPoint,
                                           QString &err)
{
    if (::umount(mountPoint.toUtf8().constData()) != 0) {
        err = QString("Unmount failed: %1").arg(strerror(errno));
        return false;
    }
    return true;
}

bool UsbLogExportController::copyLogs(const QString &mountPoint,
                                      QString &err)
{
    const QString targetDir = mountPoint + "/thinclient-logs";
    QDir().mkpath(targetDir);

    const QString ts =
        QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");

    // ------------------------------------------------------------
    // Text log files only
    // ------------------------------------------------------------
    const QStringList logFiles{
        // Application logs
        "/var/log/thinclient.log",
        "/var/log/thinclient-updater.log",
        "/var/log/rdp_launcher.log",

        // System logs
        "/var/log/messages",
        "/var/log/syslog"
    };

    for (const QString &src : logFiles) {
        if (!QFile::exists(src))
            continue;

        QFile in(src);
        const QString dst =
            targetDir + "/" +
            QFileInfo(src).completeBaseName() + "_" + ts + ".log";

        QFile out(dst);

        if (!in.open(QIODevice::ReadOnly) ||
            !out.open(QIODevice::WriteOnly)) {
            qWarning() << "Failed to copy" << src;
            continue;   // do not abort export
        }

        out.write(in.readAll());
        out.close();
        in.close();
    }

    // ------------------------------------------------------------
    // Kernel ring buffer (dmesg)
    // ------------------------------------------------------------
    {
        QProcess p;
        p.start("dmesg");
        p.waitForFinished(1500);

        QFile f(targetDir + "/dmesg_" + ts + ".log");
        if (f.open(QIODevice::WriteOnly)) {
            f.write(p.readAllStandardOutput());
            f.close();
        }
    }

    // ------------------------------------------------------------
    // systemd journal (if available)
    // ------------------------------------------------------------
    if (QFile::exists("/bin/journalctl") ||
        QFile::exists("/usr/bin/journalctl")) {

        QProcess j;
        j.start("journalctl", {"-b"});
        j.waitForFinished(3000);

        QFile f(targetDir + "/journal_" + ts + ".log");
        if (f.open(QIODevice::WriteOnly)) {
            f.write(j.readAllStandardOutput());
            f.close();
        }
    }

    ::sync();
    return true;
}
