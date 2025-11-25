#ifndef PERSISTDATA_H
#define PERSISTDATA_H

#include <QObject>
#include <QSettings>
#include <QDebug>
#include "SystemSettings.h"

class PersistData : public QObject {
    Q_OBJECT

    // --- Persistent properties exposed to QML ---
    Q_PROPERTY(QString audio READ audio WRITE setAudio NOTIFY audioChanged)
    Q_PROPERTY(QString timeZone READ timeZone WRITE setTimeZone NOTIFY timeZoneChanged)
    Q_PROPERTY(bool enableOnScreenKeyboard READ enableOnScreenKeyboard WRITE setEnableOnScreenKeyboard NOTIFY enableOnScreenKeyboardChanged)
    Q_PROPERTY(bool enableTouchScreen READ enableTouchScreen WRITE setEnableTouchScreen NOTIFY enableTouchScreenChanged)
    Q_PROPERTY(QString resolution READ resolution WRITE setResolution NOTIFY resolutionChanged)
    Q_PROPERTY(QString ethernet READ ethernet WRITE setEthernet NOTIFY ethernetChanged)
    Q_PROPERTY(QString network READ network WRITE setNetwork NOTIFY networkChanged)
    Q_PROPERTY(QString orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(QString deviceOff READ deviceOff WRITE setDeviceOff NOTIFY deviceOffChanged)
    Q_PROPERTY(QString displayOff READ displayOff WRITE setDisplayOff NOTIFY displayOffChanged)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged FINAL)

public:
    explicit PersistData(QObject *parent = nullptr);
    ~PersistData();

    SystemSettings systemSettings() const;  // 👈 New method

    // Generic methods for QML access
    Q_INVOKABLE void saveData(const QString &key, const QString &value);
    Q_INVOKABLE QString getData(const QString &key);

    // --- Individual getters/setters ---
    QString audio() const;
    void setAudio(const QString &value);

    QString timeZone() const;
    void setTimeZone(const QString &value);

    bool enableOnScreenKeyboard() const;
    void setEnableOnScreenKeyboard(bool value);

    bool enableTouchScreen() const;
    void setEnableTouchScreen(bool value);

    QString resolution() const;
    void setResolution(const QString &value);

    QString ethernet() const;
    void setEthernet(const QString &value);

    QString network() const;
    void setNetwork(const QString &value);

    QString orientation() const;
    void setOrientation(const QString &value);

    QString deviceOff() const;
    void setDeviceOff(const QString &value);

    QString displayOff() const;
    void setDisplayOff(const QString &value);

    QString language() const;
    void setLanguage(const QString &newLanguage);

signals:
    void audioChanged();
    void timeZoneChanged();
    void enableOnScreenKeyboardChanged();
    void enableTouchScreenChanged();
    void resolutionChanged();
    void ethernetChanged();
    void networkChanged();
    void orientationChanged();
    void deviceOffChanged();
    void displayOffChanged();

    void languageChanged();

private:
    QString getSystemTimezone();
    QSettings m_setting;
    QString m_group = "ThinClient";

    // cached values
    QString m_audio;
    QString m_timeZone;
    bool m_enableOnScreenKeyboard = false;
    bool m_enableTouchScreen = false;
    QString m_resolution;
    QString m_ethernet;
    QString m_network;
    QString m_orientation;
    QString m_deviceOff;
    QString m_displayOff;

    void logChange(const QString &key, const QString &value);
    QString m_language;
};

#endif // PERSISTDATA_H
