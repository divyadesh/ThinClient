#include "systemresetmanager.h"
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

void SystemResetManager::resetNetwork()
{
    qInfo() << "[FactoryReset] Resetting network configuration...";

    // Remove all stored WiFi credentials (NetworkManager)
    system("nmcli connection delete id WiFi || true");
    system("nmcli connection delete Wi-Fi || true");
    system("nmcli connection delete \"Wired connection 1\" || true");

    // Disable Hotspot
    system("nmcli radio wifi off");
    system("nmcli radio wifi on");

    // Remove DHCP leases
    system("rm -f /var/lib/NetworkManager/*");
    system("rm -f /var/lib/dhcp/*");

    // Restart network
    system("systemctl restart NetworkManager");

    qInfo() << "[FactoryReset] Network reset complete.";
}

