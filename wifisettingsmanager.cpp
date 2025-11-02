#include "wifisettingsmanager.h"

WiFiSettingsManager::WiFiSettingsManager(QObject *parent)
    : QObject{parent},
    m_settings(QSettings::IniFormat, QSettings::UserScope,
               "G1 Thin Client PC", "G1ThinClientPC")
{
}

void WiFiSettingsManager::savePassword(const QString &ssid, const QString &password)
{
    if (ssid.isEmpty() || password.isEmpty())
        return;

    m_settings.beginGroup(m_group);
    m_settings.setValue(ssid, password);
    m_settings.endGroup();
    m_settings.sync();

    logChange(ssid, "Saved password");
}

QString WiFiSettingsManager::getPassword(const QString &ssid)
{
    m_settings.beginGroup(m_group);
    QString value = m_settings.value(ssid).toString();
    m_settings.endGroup();
    return value;
}

bool WiFiSettingsManager::hasSavedPassword(const QString &ssid)
{
    m_settings.beginGroup(m_group);
    bool exists = m_settings.contains(ssid);
    m_settings.endGroup();
    return exists;
}

void WiFiSettingsManager::clearPassword(const QString &ssid)
{
    m_settings.beginGroup(m_group);
    m_settings.remove(ssid);
    m_settings.endGroup();
    m_settings.sync();
    logChange(ssid, "Cleared password");
}

void WiFiSettingsManager::logChange(const QString &ssid, const QString &action)
{
    qDebug().noquote() << QString("::::> QSettings WiFi: \"%1\", SSID=\"%2\", Action=\"%3\"")
    .arg(m_settings.fileName(), ssid, action);
}
