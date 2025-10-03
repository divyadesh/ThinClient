// LogoLoader.cpp
#include "logoloader.h"

LogoLoader::LogoLoader(QObject *parent)
    : QObject(parent)
{
    loadLogo();
}

void LogoLoader::loadLogo()
{
    const QString filePath = "/usr/share/thinclient/logo.png";
    if (QFile::exists(filePath)) {
        m_logo = filePath;
        emit logoChanged();
    } else {
        m_logo.clear();
    }
}

QString LogoLoader::logo() const
{
    return m_logo;
}
