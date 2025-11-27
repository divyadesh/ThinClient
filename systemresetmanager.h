#ifndef SYSTEMRESETMANAGER_H
#define SYSTEMRESETMANAGER_H

#include <QObject>
#include <QProcess>

class SystemResetManager : public QObject
{
    Q_OBJECT

public:
    explicit SystemResetManager(QObject *parent = nullptr);

    Q_INVOKABLE bool reboot();
    Q_INVOKABLE bool shutdown();
    Q_INVOKABLE bool factoryReset(const QString &resetScript = "/usr/bin/factory_reset.sh");
    static void resetNetwork();

signals:
    void commandExecuted(const QString &command, bool success);

private:
    bool runCommand(const QString &cmd);
};

#endif // SYSTEMRESETMANAGER_H
