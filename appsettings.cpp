// appsettings.cpp
#include "appsettings.h"

AppSettings::AppSettings(QObject *parent)
    : QObject(parent)
    , m_settings("MyCompany", "MyApp") // replace with your org/app name
{}

QString AppSettings::selectedLanguage() const
{
    return m_settings.value("language", "en").toString();
}

void AppSettings::setSelectedLanguage(const QString &lang)
{
    if (lang == selectedLanguage())
        return;
    m_settings.setValue("language", lang);
    emit selectedLanguageChanged();
}

QString AppSettings::selectedTimeZone() const
{
    return m_settings.value("timezone","").toString();
}

void AppSettings::setSelectedTimeZone(const QString &tz)
{
    if (tz == selectedTimeZone())
        return;
    m_settings.setValue("timezone", tz);
    emit selectedTimeZoneChanged();
}
