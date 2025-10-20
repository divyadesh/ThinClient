#ifndef AUDIOSETTINGSOPTIONS_H
#define AUDIOSETTINGSOPTIONS_H

#include <QObject>
#include <QProcess>

class AudioSettingsOptions : public QObject
{
    Q_OBJECT
public:
    enum Audio {
        Jack = 0,
        Usb,
        Hdmi
    };
    Q_ENUM(Audio)

    explicit AudioSettingsOptions(QObject *parent = nullptr): QObject{parent}{}

signals:
};

#endif // AUDIOSETTINGSOPTIONS_H
