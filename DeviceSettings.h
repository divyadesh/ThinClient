#ifndef DEVICESETTINGS_H
#define DEVICESETTINGS_H

#include <QObject>
#include <QProcess>

class DeviceSettings : public QObject
{
    Q_OBJECT
public:
    enum Audio {
        Jack = 0,
        Usb,
        Hdmi
    };
    Q_ENUM(Audio)

    explicit DeviceSettings(QObject *parent = nullptr): QObject{parent}{}

signals:
};

#endif // DEVICESETTINGS_H
