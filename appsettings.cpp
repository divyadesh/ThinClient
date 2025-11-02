// appsettings.cpp
#include "appsettings.h"

AppSettings::AppSettings(QObject *parent)
    : QObject(parent)
    , m_settings("MyCompany", "MyApp") // replace with your org/app name
{}

QString AppSettings::selectedLanguage() const
{
    return m_settings.value("Language", "en").toString();
}

void AppSettings::setSelectedLanguage(const QString &lang)
{
    if (lang == selectedLanguage())
        return;
    m_settings.setValue("Language", lang);
    emit selectedLanguageChanged();
}

QString AppSettings::selectedTimeZone() const
{
    return m_settings.value("TimeZone","").toString();
}

void AppSettings::setSelectedTimeZone(const QString &tz)
{
    if (tz == selectedTimeZone())
        return;
    m_settings.setValue("TimeZone", tz);
    emit selectedTimeZoneChanged();
}
