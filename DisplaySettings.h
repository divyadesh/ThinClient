#ifndef DISPLAYSETTINGS_H
#define DISPLAYSETTINGS_H

#include <QObject>
#include <QStringList>

/*!
 * \brief DisplaySettings backend for Weston-based kiosk.
 *
 * This class:
 *  - Reads current settings from /etc/xdg/weston/weston.ini
 *  - Exposes them as Q_PROPERTY for QML:
 *      * resolution        ("1920x1080")
 *      * orientation       ("normal", "rotate-90", ...)
 *      * idleTimeSeconds   (0 = never blank)
 *      * touchEnabled      (true/false)
 *  - Provides convenience methods to:
 *      * list available resolutions and orientations
 *      * build "WIDTHxHEIGHT" strings
 *      * apply new settings and restart Weston
 *      * shutdown the device
 */
class DisplaySettings : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString resolution
                   READ resolution
                       WRITE setResolution
                           NOTIFY resolutionChanged)

    Q_PROPERTY(QString orientation
                   READ orientation
                       WRITE setOrientation
                           NOTIFY orientationChanged)

    Q_PROPERTY(int idleTimeSeconds
                   READ idleTimeSeconds
                       WRITE setIdleTimeSeconds
                           NOTIFY idleTimeSecondsChanged)

    Q_PROPERTY(bool touchEnabled
                   READ touchEnabled
                       WRITE setTouchEnabled
                           NOTIFY touchEnabledChanged)

    Q_PROPERTY(int displayIdleMinutes READ displayIdleMinutes WRITE setDisplayIdleMinutes NOTIFY displayIdleMinutesChanged FINAL)
    Q_PROPERTY(int deviceAutoPowerOffHours READ deviceAutoPowerOffHours WRITE setDeviceAutoPowerOffHours NOTIFY deviceAutoPowerOffHoursChanged FINAL)

public:
    explicit DisplaySettings(QObject *parent = nullptr);

    // --- Q_PROPERTY getters ---
    QString resolution() const          { return m_resolution; }
    QString orientation() const         { return m_orientation; }
    int idleTimeSeconds() const         { return m_idleTimeSeconds; }
    bool touchEnabled() const           { return m_touchEnabled; }

    // --- Q_PROPERTY setters (do NOT write config, just update state) ---
    void setResolution(const QString &res);
    void setOrientation(const QString &ori);
    void setIdleTimeSeconds(int seconds);
    void setTouchEnabled(bool enabled);

    // --- Helper APIs for QML ---

    //! List of common resolutions ("WIDTHxHEIGHT").
    Q_INVOKABLE QStringList availableResolutions() const;

    //! Valid orientation values for Weston: "normal", "rotate-90", "rotate-180", "rotate-270".
    Q_INVOKABLE QStringList availableOrientations() const;

    //! Helper to build "WIDTHxHEIGHT" string from ints.
    Q_INVOKABLE QString makeModeString(int width, int height) const;

    /*!
     * \brief Apply all current settings (properties) to weston.ini and restart Weston.
     *
     * Uses the current values of:
     *  - resolution()
     *  - orientation()
     *  - idleTimeSeconds()
     *  - touchEnabled()
     */
    Q_INVOKABLE bool applyCurrentSettings();

    /*!
     * \brief Apply display settings with explicit parameters.
     *
     * Also updates the Q_PROPERTY values and emits change signals.
     */
    Q_INVOKABLE bool applyDisplaySettings(const QString &resolution,
                                          const QString &orientation,
                                          int idleSeconds,
                                          bool enableTouch);

    //! Reload properties from /etc/xdg/weston/weston.ini.
    Q_INVOKABLE bool reloadFromConfig();

    //! Restart weston.service.
    Q_INVOKABLE bool restartWeston();

    //! Power off device using systemctl poweroff.
    Q_INVOKABLE bool shutdownDevice();

    int displayIdleMinutes() const;
    void setDisplayIdleMinutes(int newDisplayIdleMinutes);

    int deviceAutoPowerOffHours() const;
    void setDeviceAutoPowerOffHours(int newDeviceAutoPowerOffHours);

signals:
    void resolutionChanged();
    void orientationChanged();
    void idleTimeSecondsChanged();
    void touchEnabledChanged();

    void displayIdleMinutesChanged();

    void deviceAutoPowerOffHoursChanged();

private:
    // Lower level helpers used internally
    bool applyOnlyOutput(const QString &resolution,
                         const QString &orientation);

    bool applyIdleTimeout(int seconds);
    bool enableTouchpad(bool enable);

    bool loadFromWestonIni();  // internal; called by ctor and reloadFromConfig()
    QString detectCurrentResolution() const;
    void normalizeWestonIniSpacing();

private:
    QString m_resolution;
    QString m_orientation;
    int     m_idleTimeSeconds = 0;
    bool    m_touchEnabled    = false;
    int m_displayIdleMinutes;
    int m_deviceAutoPowerOffHours;
};

#endif // DISPLAYSETTINGS_H
