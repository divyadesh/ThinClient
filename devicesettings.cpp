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

/*
 * setAudioOutput()
 * ----------------
 * This method switches the audio output device (JACK / HDMI / USB)
 * using your shell script:  set_audio_output.sh
 * The script updates /etc/asound.conf so ALSA sends audio
 * to the selected output.
 *
 * Example:
 *   setAudioOutput("hdmi");  -> route audio to HDMI
 *   setAudioOutput("jack");  -> route audio to headphone jack
 *   setAudioOutput("usb");   -> route audio to USB audio card
 *
 * MANUAL TESTING FROM TERMINAL:
 * -----------------------------
 * Switch to HDMI:
 *   set_audio_output.sh hdmi
 *   speaker-test -D default -c 2
 *
 * Switch to JACK:
 *   set_audio_output.sh jack
 *   speaker-test -D default -c 2
 *
 * Switch to USB:
 *   set_audio_output.sh usb
 *   speaker-test -D default -c 2
 *
 * Play WAV test file:
 *   aplay -D default /usr/share/sounds/alsa/Front_Center.wav
 *
 * NOTE:
 * QProcess is used instead of 'system()' because it is safer
 * and integrates better with Qt applications.
 */

void DeviceSettings::setAudioOutput(const QString &sinkName)
{
    qDebug() << "Setting audio output to:" << sinkName;

    // Build command: e.g. "set_audio_output.sh hdmi"
    QString command = QString("set_audio_output.sh %1").arg(sinkName);

    // Run the script using QProcess
    QProcess process;
    process.start(command);
    process.waitForFinished();   // Block until script completes

    // Optionally print script output (useful for debugging)
    QString output = process.readAll();
    qDebug() << "Script output:" << output;

    qDebug() << "Output switch completed.";
}



