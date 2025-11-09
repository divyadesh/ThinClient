#include "imageupdater.h"
#include <QDebug>
#include <QFileInfo>
#include <QProcess>
#include <QTimer>
#include <QProcess>

ImageUpdater::ImageUpdater(QObject *parent)
    : QObject(parent)
    , m_targetPath("/home/root/Connect")
{}

bool ImageUpdater::isUpdating() const
{
    return m_updating;
}

void ImageUpdater::startUpdate(const QString &usbPortPath)
{
    QString scriptPath = "/usr/bin/update_from_usb.sh";

    if (m_updating) {
        emit errorOccurred("Update already in progress");
        return;
    }

    // Check if the script exists before executing
    if (!QFile::exists(scriptPath)) {
        qWarning() << "❌ Update script not found:" << scriptPath;
        return;
    }

    // Check if argument provided
    if (usbPortPath.isEmpty()) {
        qWarning() << "⚠️ No USB path provided to updater (e.g. /dev/sda1)";
        return;
    }

    // Prepare arguments for QProcess
    QStringList arguments;
    arguments << usbPortPath;

    m_updating = true;
    emit updatingChanged();
    emit progressChanged("Removing old file...", 0);


    // Start the update script as detached (so it runs independently)
    bool success = QProcess::startDetached(scriptPath, arguments);

    if (success) {
        qDebug() << "✅ Started update script with device:" << usbPortPath;
    } else {
        qWarning() << "❌ Failed to start update script!";
    }

    m_updating = false;
    emit updatingChanged();
    emit progressChanged("Update Successfully!", 100);
}

void ImageUpdater::handleDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesTotal > 0) {
        int percent = static_cast<int>((bytesReceived * 100) / bytesTotal);
        emit progressChanged("Downloading...", percent);
    }
}

void ImageUpdater::handleDownloadFinished()
{
    if (!m_reply || m_reply->error() != QNetworkReply::NoError) {
        return; // Already handled in error slot
    }

    m_file.write(m_reply->readAll());
    m_file.close();
    m_reply->deleteLater();
    m_reply = nullptr;

    emit progressChanged("Setting permissions...", 100);

    if (!QFile::setPermissions(m_targetPath,
                               QFile::ExeOwner | QFile::ReadOwner | QFile::WriteOwner)) {
        emit errorOccurred("Failed to set executable permissions.");
        resetState();
        return;
    }

    emit progressChanged("Update complete. Rebooting in 5 seconds...", 100);

    startRebootCountdown();
}

void ImageUpdater::handleDownloadError(QNetworkReply::NetworkError code)
{
    Q_UNUSED(code);
    emit errorOccurred("Download failed: " + m_reply->errorString());
    m_reply->deleteLater();
    m_reply = nullptr;
    m_file.close();
    QFile::remove(m_targetPath);
    resetState();
}

void ImageUpdater::startRebootCountdown()
{
    for (int i = 5; i >= 1; --i) {
        QTimer::singleShot((6 - i) * 1000, this, [this, i]() { emit rebootCountdown(i); });
    }

    QTimer::singleShot(6000, this, []() { QProcess::execute("reboot"); });

    resetState(); // Safe to reset internal flags now
}

void ImageUpdater::cancelUpdate()
{
    if (m_reply) {
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = nullptr;
    }
    if (m_file.isOpen()) {
        m_file.close();
    }
    QFile::remove(m_targetPath);
    resetState();
    emit errorOccurred("Update canceled.");
}

void ImageUpdater::resetState()
{
    m_updating = false;
    emit updatingChanged();
}
