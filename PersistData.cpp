#include "PersistData.h"
#include <QTimeZone>

PersistData::PersistData(QObject *parent)
    : QObject{parent},
    m_setting(QSettings::IniFormat, QSettings::UserScope,
              "G1 Thin Client PC", "G1ThinClientPC")
{
    // Load all settings once at startup
    m_setting.beginGroup(m_group);

    m_audio = m_setting.value("Audio", "0").toString();
    m_timeZone = m_setting.value("TimeZone", "").toString();
    m_enableOnScreenKeyboard = m_setting.value("EnableOnScreenKeyboard", false).toBool();
    m_enableTouchScreen = m_setting.value("EnableTouchScreen", false).toBool();
    m_resolution = m_setting.value("Resolution", "1600 x 900").toString();
    m_ethernet = m_setting.value("Ethernet", "DHCP").toString();
    m_network = m_setting.value("Network", "Ethernet").toString();
    m_orientation = m_setting.value("Orientation", "0").toString();
    m_deviceOff = m_setting.value("DeviceOff", "0").toString();
    m_displayOff = m_setting.value("DisplayOff", "0").toString();

    m_setting.endGroup();
}

PersistData::~PersistData() {}

SystemSettings PersistData::systemSettings() const
{
    SystemSettings cfg;

    cfg.audio = m_setting.value("Audio", "0").toString();

    // Try to load timezone from settings
    cfg.timeZone = m_setting.value("TimeZone", "").toString();

    // ✅ If no timezone is saved, use the system’s timezone ID
    if (cfg.timeZone.isEmpty()) {
        QByteArray systemZone = QTimeZone::systemTimeZoneId();
        cfg.timeZone = QString::fromUtf8(systemZone);

        qDebug() << "System timezone detected:" << cfg.timeZone;
    }

    cfg.enableOnScreenKeyboard = m_setting.value("EnableOnScreenKeyboard", false).toBool();
    cfg.enableTouchScreen = m_setting.value("EnableTouchScreen", false).toBool();
    cfg.resolution = m_setting.value("Resolution", "Auto").toString();
    cfg.ethernet = m_setting.value("Ethernet", "DHCP").toString();
    cfg.network = m_setting.value("Network", "Ethernet").toString();
    cfg.orientation = m_setting.value("Orientation", 0).toInt();
    cfg.deviceOff = m_setting.value("DeviceOff", 0).toInt();
    cfg.displayOff = m_setting.value("DisplayOff", 0).toInt();

    return cfg;
}

// Generic save function
void PersistData::saveData(const QString &key, const QString &value) {
    m_setting.beginGroup(m_group);
    m_setting.setValue(key, value);
    m_setting.endGroup();
    m_setting.sync();
    logChange(key, value);
}

// Generic get function
QString PersistData::getData(const QString &key) {
    m_setting.beginGroup(m_group);
    QString value = m_setting.value(key).toString();
    m_setting.endGroup();
    return value;
}

// Logging helper
void PersistData::logChange(const QString &key, const QString &value) {
    qDebug().noquote() << QString("::::> QSettings File: \"%1\", key=\"%2\", value=\"%3\"")
    .arg(m_setting.fileName(), key, value);
}

/* -----------------------------
 * Property accessors (inline)
 * ----------------------------- */

QString PersistData::audio() const { return m_audio; }
void PersistData::setAudio(const QString &value) {
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
    saveData("EnableOnScreenKeyboard", value ? "true" : "false");
    emit enableOnScreenKeyboardChanged();
}

bool PersistData::enableTouchScreen() const { return m_enableTouchScreen; }
void PersistData::setEnableTouchScreen(bool value) {
    if (m_enableTouchScreen == value) return;
    m_enableTouchScreen = value;
    saveData("EnableTouchScreen", value ? "true" : "false");
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

QString PersistData::deviceOff() const { return m_deviceOff; }
void PersistData::setDeviceOff(const QString &value) {
    if (m_deviceOff == value) return;
    m_deviceOff = value;
    saveData("DeviceOff", value);
    emit deviceOffChanged();
}

QString PersistData::displayOff() const { return m_displayOff; }
void PersistData::setDisplayOff(const QString &value) {
    if (m_displayOff == value) return;
    m_displayOff = value;
    saveData("DisplayOff", value);
    emit displayOffChanged();
}
