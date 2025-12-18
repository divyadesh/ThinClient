#pragma once

#include <QObject>
#include <QEvent>
#include <QTimer>

#include "PersistData.h"
#include "Application.h"

/**
 * @class InputActivityFilter
 * @brief Monitors user input (keyboard, mouse, touch) and triggers actions
 *        when the application becomes idle.
 *
 * This class installs a global Qt event filter to detect user activity
 * such as key presses, mouse movements, and touch interactions. If no
 * activity is detected for a configured timeout period, it emits signals
 * indicating that the system/application is idle and then triggers a system
 * suspend operation via `systemctl suspend`.
 *
 * ## How it works:
 * 1. The idle timeout is loaded from PersistData (`deviceOff`).
 * 2. If the timeout is *greater than zero*, a QTimer is started.
 * 3. Any input event resets the timer.
 * 4. When the timer expires:
 *      - `userIdle()` is emitted
 *      - `appInactive()` is emitted
 *      - `powerSuspend()` is emitted
 *      - The system suspends using `systemctl suspend`
 *
 * ## When the idle timeout is set to 0:
 * Idle monitoring is disabled completely — the timer does not run and
 * no suspend occurs.
 *
 * ## Usage:
 * - Create an instance early during application startup.
 * - Install it as a global event filter:
 *
 * ```cpp
 * InputActivityFilter* idle = new InputActivityFilter(this);
 * qApp->installEventFilter(idle);
 * ```
 *
 * - Allow QML/Qt UI to modify the timeout via the exposed property:
 * ```qml
 * activityMonitor.idleTimeoutMs = 120000   // 2 minutes
 * ```
 *
 * @note Wake-up behavior depends entirely on kernel and device-tree support.
 *       The Qt application cannot enable hardware wakeup sources if the
 *       kernel does not expose them.
 */
class InputActivityFilter : public QObject
{
    Q_OBJECT

    /**
     * @property idleTimeoutMs
     * @brief Idle timeout duration in milliseconds.
     *
     * When set to 0:
     * - Idle monitoring is disabled
     * - No timers run
     * - System suspend will not trigger
     *
     * When set to a value > 0:
     * - The inactivity timer starts
     * - Activity resets the timer
     * - Timeouts trigger suspend
     */
    Q_PROPERTY(int idleTimeoutMs READ idleTimeoutMs
                   WRITE setIdleTimeoutMs
                       NOTIFY idleTimeoutMsChanged
                           FINAL)
    Q_PROPERTY(int displayOffTimeoutMs READ displayOffTimeoutMs WRITE setDisplayOffTimeoutMs NOTIFY displayOffTimeoutMsChanged FINAL)

public:
    /**
     * @brief Constructs the activity filter and optionally starts
     *        idle monitoring if the configured timeout > 0.
     *
     * @param parent The parent QObject
     */
    explicit InputActivityFilter(QObject *parent = nullptr);

    /**
     * @brief Returns the current idle timeout in milliseconds.
     */
    int idleTimeoutMs() const;
    int displayOffTimeoutMs() const;
signals:
    /**
     * @brief Emitted when any user activity is detected.
     */
    void userActivity();

    /**
     * @brief Emitted when the application becomes idle.
     */
    void userIdle();

    /**
     * @brief Emitted when the application transitions back to active state
     *        after the system wakes from suspend.
     */
    void appActive();

    /**
     * @brief Emitted just before the system suspends.
     */
    void appInactive();

    /**
     * @brief Emitted before the system enters suspend mode.
     */
    void powerSuspend();

    /**
     * @brief Emitted immediately after the system wakes from suspend.
     */
    void powerResume();

    /**
     * @brief Emitted when the idle timeout value changes.
     */
    void idleTimeoutMsChanged();

    void displayOffTimeoutMsChanged();
    void displayOff();
    void displayOn();

public slots:
    /**
     * @brief Sets a new idle timeout and restarts or stops the timer.
     *
     * @param newIdleTimeoutMs Timeout in milliseconds.
     *                         - If 0 → the idle timer stops.
     *                         - If > 0 → timer restarts with the new value.
     */
    void setIdleTimeoutMs(int ms);
    void setDisplayOffTimeoutMs(int ms);
    void writeHdmiStatus(const QString &mode);
    void autoDetectHdmi();

private slots:
    /**
     * @brief Internal handler called when the idle timer expires.
     *
     * This performs:
     * - Logging
     * - Emitting idle/app inactivity signals
     * - Triggering system suspend
     */
    void onIdleTimeout();
    void onDisplayOffTimeout();

private:
    /**
     * @brief Helper for consistent formatted logging.
     * @param msg Text message to print.
     */
    void log(const QString &msg);

    QTimer m_idleTimer;
    int m_idleTimeoutMs = 0;
    bool m_wasSuspended = false;

    QTimer m_displayOffTimer;
    int m_displayOffTimeoutMs = 0;
    bool m_displayIsOff = false;

    QString m_hdmiStatusPath;
    qint64 m_lastWakeTimeMs = 0;

protected:
    /**
     * @brief Global event filter to detect keyboard/mouse/touch input.
     *
     * @param obj   The object receiving the event
     * @param event The event itself
     * @return      true if the event is handled; otherwise false
     */
    bool eventFilter(QObject *obj, QEvent *event) override;
};
