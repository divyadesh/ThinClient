#include "SystemResetManager.h"
#include <QDebug>

SystemResetManager::SystemResetManager(QObject *parent)
    : QObject(parent)
{}

bool SystemResetManager::runCommand(const QString &cmd)
{
    int exitCode = QProcess::execute(cmd);
    bool success = (exitCode == 0);

    emit commandExecuted(cmd, success);

    if (!success)
        qWarning() << "Command failed:" << cmd << " Exit code:" << exitCode;

    return success;
}

bool SystemResetManager::reboot()
{
    // Typical Yocto/Linux reboot
    return runCommand("reboot");
}

bool SystemResetManager::shutdown()
{
    // Shutdown the system
    return runCommand("poweroff");
}

bool SystemResetManager::factoryReset(const QString &resetScript)
{
    // Run your custom factory reset script
    return runCommand(resetScript);
}
