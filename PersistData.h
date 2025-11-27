#ifndef PERSISTDATA_H
#define PERSISTDATA_H

#include <QObject>
#include <QSettings>
#include <QDebug>
#include <QVariant>
#include <QString>


/**
 * @class PersistData
 * @brief Centralized persistent storage for system/user settings.
 *
 * This class wraps QSettings and exposes application settings to QML.
 * All values are cached in memory and written to disk automatically.
 *
 * Settings are stored under:
 *   ~/.config/G1 Thin Client PC/G1ThinClientPC.ini
 *
 * The class guarantees:
 *  - Automatic default initialization
 *  - Type-safe read/write
 *  - Unified logging
 *  - QML-friendly property interface
 */
class PersistData : public QObject {
    Q_OBJECT

    // --- Persistent QML properties ---
    Q_PROPERTY(int audio READ audio WRITE setAudio NOTIFY audioChanged)
    Q_PROPERTY(QString timeZone READ timeZone WRITE setTimeZone NOTIFY timeZoneChanged)
    Q_PROPERTY(bool enableOnScreenKeyboard READ enableOnScreenKeyboard WRITE setEnableOnScreenKeyboard NOTIFY enableOnScreenKeyboardChanged)
    Q_PROPERTY(bool enableTouchScreen READ enableTouchScreen WRITE setEnableTouchScreen NOTIFY enableTouchScreenChanged)
    Q_PROPERTY(QString resolution READ resolution WRITE setResolution NOTIFY resolutionChanged)
    Q_PROPERTY(QString ethernet READ ethernet WRITE setEthernet NOTIFY ethernetChanged)
    Q_PROPERTY(QString network READ network WRITE setNetwork NOTIFY networkChanged)
    Q_PROPERTY(QString orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(int deviceOff READ deviceOff WRITE setDeviceOff NOTIFY deviceOffChanged)
    Q_PROPERTY(int displayOff READ displayOff WRITE setDisplayOff NOTIFY displayOffChanged)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged FINAL)

public:
    explicit PersistData(QObject *parent = nullptr);
    ~PersistData();

    /// Save arbitrary key/value pair
    Q_INVOKABLE void saveData(const QString &key, const QVariant &value);

    /// Read a saved setting
    Q_INVOKABLE QString getData(const QString &key);
    bool resetSettings();

    // ---- Accessors ----
    int audio() const;                        void setAudio(const int &value);
    QString timeZone() const;                 void setTimeZone(const QString &value);
    bool enableOnScreenKeyboard() const;      void setEnableOnScreenKeyboard(bool value);
    bool enableTouchScreen() const;           void setEnableTouchScreen(bool value);
    QString resolution() const;               void setResolution(const QString &value);
    QString ethernet() const;                 void setEthernet(const QString &value);
    QString network() const;                  void setNetwork(const QString &value);
    QString orientation() const;              void setOrientation(const QString &value);
    int deviceOff() const;                    void setDeviceOff(const int &value);
    int displayOff() const;                   void setDisplayOff(const int &value);
    QString language() const;                 void setLanguage(const QString &value);

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
    void loadOrDefault(const QString &key, QString &var, const QString &def);
    void loadOrDefault(const QString &key, bool &var, bool def);
    void loadOrDefault(const QString &key, int &var, int def);
    bool setSystemTimezone(const QString &tzId);

    void logChange(const QString &key, const QVariant &value);

    QSettings m_setting;
    QString m_group = "ThinClient";

    // Cached settings
    int m_audio;
    QString m_timeZone;
    bool m_enableOnScreenKeyboard = false;
    bool m_enableTouchScreen = false;
    QString m_resolution;
    QString m_ethernet;
    QString m_network;
    QString m_orientation;
    int m_deviceOff = 0;
    int m_displayOff = 0;
    QString m_language;
};

#endif // PERSISTDATA_H
