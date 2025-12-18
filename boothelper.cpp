#include "boothelper.h"
#include <QFile>
#include <QDebug>

BootHelper::BootHelper(QObject *parent)
    : QObject(parent),
    bootFlagPath("/tmp/app_boot_flag")
{
}

bool BootHelper::shouldShowRebootDialog()
{
    // If file exists → already checked after this reboot
    if (QFile::exists(bootFlagPath)) {
        qDebug() << "[BootHelper] Flag exists → not first launch after reboot";
        return false;
    }

    // Create flag file
    QFile file(bootFlagPath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write("booted");
        file.close();
        qDebug() << "[BootHelper] First launch after reboot → Flag created";
    } else {
        qWarning() << "[BootHelper] Failed to create flag file";
    }

    return true;  // Show dialog
}

