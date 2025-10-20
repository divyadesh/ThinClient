#include "devicesettings.h"

DeviceSettings::DeviceSettings(QObject *parent)
    : QObject(parent)
{}

// Helper to run shell commands and capture output
QString DeviceSettings::execCommand(const QString &cmd)
{
    QProcess process;
    process.start("bash", QStringList() << "-c" << cmd);
    process.waitForFinished();
    QString output = process.readAllStandardOutput().trimmed();
    QString error = process.readAllStandardError().trimmed();
    if (!error.isEmpty())
        qWarning() << "Command error:" << error;
    return output;
}

// 🖥️ Set display resolution
void DeviceSettings::setResolution(const QString &output, const QString &mode)
{
    QString command = QString("xrandr --output %1 --mode %2").arg(output, mode);
    qDebug() << "Running:" << command;
    execCommand(command);
}

// 🕒 Set timezone
void DeviceSettings::setTimezone(const QString &timezone)
{
    QString command = QString("timedatectl set-timezone %1").arg(timezone);
    qDebug() << "Running:" << command;
    execCommand(command);
}

// 🔊 Set audio output (sink)
void DeviceSettings::setAudioOutput(const QString &sinkName)
{
    qDebug() << "Setting audio output to:" << sinkName;

    QString setDefault = QString("pactl set-default-sink %1").arg(sinkName);
    execCommand(setDefault);

    // Move currently playing streams
    QString moveStreams =
        QString("for i in $(pactl list short sink-inputs | awk '{print $1}'); "
                "do pactl move-sink-input $i %1; done").arg(sinkName);
    execCommand(moveStreams);
}

// 🎧 Get available audio outputs

