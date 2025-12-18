#ifndef BOOTHELPER_H
#define BOOTHELPER_H

#include <QObject>
#include <QTimer>

class BootHelper : public QObject
{
    Q_OBJECT

public:
    explicit BootHelper(QObject *parent = nullptr);

    // Called from QML after 10s delay
    Q_INVOKABLE bool shouldShowRebootDialog();

private:
    QString bootFlagPath;     // Path to temp flag file
};

#endif // BOOTHELPER_H
