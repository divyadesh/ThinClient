// appsettings.h
#pragma once

#include <QObject>
#include <QSettings>

class AppSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString selectedLanguage READ selectedLanguage WRITE setSelectedLanguage NOTIFY
                   selectedLanguageChanged)
    Q_PROPERTY(QString selectedTimeZone READ selectedTimeZone WRITE setSelectedTimeZone NOTIFY
                   selectedTimeZoneChanged)

public:
    explicit AppSettings(QObject *parent = nullptr);

    QString selectedLanguage() const;
    void setSelectedLanguage(const QString &lang);

    QString selectedTimeZone() const;
    void setSelectedTimeZone(const QString &tz);

signals:
    void selectedLanguageChanged();
    void selectedTimeZoneChanged();

private:
    QSettings m_settings;
};
