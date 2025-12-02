/**
 * @file PersistData.cpp
 * @brief Implementation of PersistData, a wrapper around QSettings with
 *        default loading and unified logging.
 */

#include "PersistData.h"
#include <QDateTime>
#include <QFile>
#include <unistd.h>

/**
 * @brief Constructor. Loads settings from disk or initializes defaults.
 */
PersistData::PersistData(QObject *parent)
    : QObject(parent),
    m_setting(QSettings::IniFormat, QSettings::UserScope,
              "G1 Thin Client PC", "G1ThinClientPC")
{
    m_setting.beginGroup(m_group);

    if (!m_setting.contains("TimeZone")) {
        const QString defaultTimeZone = "Asia/Kolkata";
        if(setSystemTimezone(defaultTimeZone)) {
            m_setting.setValue("TimeZone", defaultTimeZone);
        }
    }
    // keep it simple: store audio as int (0 Jack, 1 USB, 2 HDMI)
    loadOrDefault("Audio",                 m_audio,               0);
    loadOrDefault("TimeZone",              m_timeZone,            "Asia/Kolkata");
    loadOrDefault("EnableOnScreenKeyboard",m_enableOnScreenKeyboard, false);
    loadOrDefault("EnableTouchScreen",     m_enableTouchScreen,   false);
    loadOrDefault("Resolution",            m_resolution,          "1600 x 900");
    loadOrDefault("Ethernet",              m_ethernet,            "DHCP");
    loadOrDefault("Network",               m_network,             "Ethernet");
    loadOrDefault("Orientation",           m_orientation,         "normal");
    loadOrDefault("DeviceOff",             m_deviceOff,           0);
    loadOrDefault("DisplayOff",            m_displayOff,          0);
    loadOrDefault("Language",              m_language,            "en_US");
    loadOrDefault("GFXMode",               m_gfxMode,             "auto");
    loadOrDefault("MenuAnimation",         m_menuAnimation,       false);

    m_setting.endGroup();
    m_setting.sync();
}

bool PersistData::setSystemTimezone(const QString &tzId)
{
    const QString zoneInfoPath = "/usr/share/zoneinfo/" + tzId;
    const QString localTimePath = "/etc/localtime";
    const QString timezoneFile  = "/etc/timezone";

    qInfo() << "[PersistData] Requested timezone change to:" << tzId;

    // Check if zoneinfo file exists
    if (!QFile::exists(zoneInfoPath)) {
        qWarning() << "[PersistData] ERROR: Zoneinfo file not found:" << zoneInfoPath;
        return false;
    }

    // Remove old /etc/localtime
    if (QFile::exists(localTimePath)) {
        if (!QFile::remove(localTimePath)) {
            qWarning() << "[PersistData] ERROR: Could not remove old /etc/localtime";
            return false;
        }
    }

    // Create symlink
    if (::symlink(zoneInfoPath.toUtf8().constData(), localTimePath.toUtf8().constData()) != 0) {
        qWarning() << "[PersistData] ERROR: Failed to create symlink to" << zoneInfoPath;
        return false;
    }

    qInfo() << "[PersistData] Symlink created:" << localTimePath << "->" << zoneInfoPath;

    // Update /etc/timezone (optional)
    QFile file(timezoneFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(tzId.toUtf8());
        file.close();
        qInfo() << "[PersistData] Updated /etc/timezone with" << tzId;
    }

    qInfo() << "[PersistData] System timezone successfully set to:" << tzId;
    return true;
}

PersistData::~PersistData() {}

/**
 * @brief Load value from settings or set default if missing.
 */
void PersistData::loadOrDefault(const QString &key, QString &var, const QString &def)
{
    if (!m_setting.contains(key)) {
        var = def;
        m_setting.setValue(key, def);
    } else {
        var = m_setting.value(key).toString();
    }
}

/**
 * @brief Overload for boolean settings.
 */
void PersistData::loadOrDefault(const QString &key, bool &var, bool def)
{
    if (!m_setting.contains(key)) {
        var = def;
        m_setting.setValue(key, def);
    } else {
        var = m_setting.value(key).toBool();
    }
}

/**
 * @brief Overload for integer settings.
 */
void PersistData::loadOrDefault(const QString &key, int &var, int def)
{
    if (!m_setting.contains(key)) {
        var = def;
        m_setting.setValue(key, def);
    } else {
        var = m_setting.value(key).toInt();
    }
}

/**
 * @brief Save a key/value pair to QSettings.
 */
void PersistData::saveData(const QString &key, const QVariant &value)
{
    m_setting.beginGroup(m_group);
    m_setting.setValue(key, value);
    m_setting.endGroup();
    m_setting.sync();

    logChange(key, value);
}

/**
 * @brief Return value of a stored key.
 */
QString PersistData::getData(const QString &key)
{
    m_setting.beginGroup(m_group);
    QString value = m_setting.value(key).toString();
    m_setting.endGroup();
    return value;
}

/**
 * @brief Log changes in a structured format.
 */
void PersistData::logChange(const QString &key, const QVariant &value)
{
    const QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    qDebug().noquote()
        << QString("[%1][PersistData] key='%2' value='%3' (file: %4)")
               .arg(timestamp, key, value.toString(), m_setting.fileName());
}

/* ------------------------------ */
/*        Property Accessors      */
/* ------------------------------ */

int PersistData::audio() const { return m_audio; }
void PersistData::setAudio(const int &value) {
    if (m_audio == value) return;
    m_audio = value;
    saveData("Audio", value);
    emit audioChanged();
}

QString PersistData::timeZone() const { return m_timeZone; }
void PersistData::setTimeZone(const QString &value) {
    if (m_timeZone == value) return;
    m_timeZone = value;
    saveData("TimeZone", value);
    emit timeZoneChanged();
}

bool PersistData::enableOnScreenKeyboard() const { return m_enableOnScreenKeyboard; }
void PersistData::setEnableOnScreenKeyboard(bool value) {
    if (m_enableOnScreenKeyboard == value) return;
    m_enableOnScreenKeyboard = value;
    saveData("EnableOnScreenKeyboard", value);
    emit enableOnScreenKeyboardChanged();
}

bool PersistData::enableTouchScreen() const { return m_enableTouchScreen; }
void PersistData::setEnableTouchScreen(bool value) {
    if (m_enableTouchScreen == value) return;
    m_enableTouchScreen = value;
    saveData("EnableTouchScreen", value);
    emit enableTouchScreenChanged();
}

QString PersistData::resolution() const { return m_resolution; }
void PersistData::setResolution(const QString &value) {
    if (m_resolution == value) return;
    m_resolution = value;
    saveData("Resolution", value);
    emit resolutionChanged();
}

QString PersistData::ethernet() const { return m_ethernet; }
void PersistData::setEthernet(const QString &value) {
    if (m_ethernet == value) return;
    m_ethernet = value;
    saveData("Ethernet", value);
    emit ethernetChanged();
}

QString PersistData::network() const { return m_network; }
void PersistData::setNetwork(const QString &value) {
    if (m_network == value) return;
    m_network = value;
    saveData("Network", value);
    emit networkChanged();
}

QString PersistData::orientation() const { return m_orientation; }
void PersistData::setOrientation(const QString &value) {
    if (m_orientation == value) return;
    m_orientation = value;
    saveData("Orientation", value);
    emit orientationChanged();
}

int PersistData::deviceOff() const { return m_deviceOff; }
void PersistData::setDeviceOff(const int &value) {
    if (m_deviceOff == value) return;
    m_deviceOff = value;
    saveData("DeviceOff", value);
    emit deviceOffChanged();
}

int PersistData::displayOff() const { return m_displayOff; }
void PersistData::setDisplayOff(const int &value) {
    if (m_displayOff == value) return;
    m_displayOff = value;
    saveData("DisplayOff", value);
    emit displayOffChanged();
}

QString PersistData::language() const { return m_language; }
void PersistData::setLanguage(const QString &value) {
    if (m_language == value) return;
    m_language = value;
    saveData("Language", value);
    emit languageChanged();
}

bool PersistData::resetSettings()
{
    QString path = m_setting.fileName();

    if (!QFile::exists(path)) {
        qInfo() << "[FactoryReset] Settings file not found, skipping.";
        return true;
    }

    if (!QFile::remove(path)) {
        qWarning() << "[FactoryReset] Failed to delete QSettings file:" << path;
        return false;
    }

    qInfo() << "[FactoryReset] Settings file deleted:" << path;
    return true;
}

QString PersistData::gfxMode() const
{
    return m_gfxMode;
}

void PersistData::setGfxMode(const QString &newGfxMode)
{
    if (m_gfxMode == newGfxMode)
        return;
    m_gfxMode = newGfxMode;
    saveData("GFXMode", m_gfxMode);
    emit gfxModeChanged();
}

bool PersistData::menuAnimation() const
{
    return m_menuAnimation;
}

void PersistData::setMenuAnimation(bool newMenuAnimation)
{
    if (m_menuAnimation == newMenuAnimation)
        return;
    m_menuAnimation = newMenuAnimation;
    saveData("MenuAnimation", m_menuAnimation);
    emit menuAnimationChanged();
}
