#ifndef DEVICESETTINGS_H
#define DEVICESETTINGS_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QDebug>

class DeviceSettings : public QObject
{
    Q_OBJECT
public:
    explicit DeviceSettings(QObject *parent = nullptr);

    Q_INVOKABLE void setResolution(const QString &output, const QString &mode);
    Q_INVOKABLE void setTimezone(const QString &timezone);
    Q_INVOKABLE void setAudioOutput(const QString &sinkName);

private:
    QString execCommand(const QString &cmd);
};

#endif // DEVICESETTINGS_H
