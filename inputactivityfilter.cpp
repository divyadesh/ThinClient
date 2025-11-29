/**
 * @file inputactivityfilter.cpp
 * @brief Implementation of InputActivityFilter, which monitors user input,
 *        turns the display off (with dimming), and triggers system suspend
 *        after inactivity.
 */

#include "inputactivityfilter.h"

#include <QDebug>
#include <QCoreApplication>
#include <QEvent>
#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QStringList>
#include <stdlib.h>

/**
 * @brief Constructor. Initializes the idle and display-off timers
 *        and starts them if their configured timeouts are > 0.
 *
 *  - m_idleTimeoutMs comes from PersistData::deviceOff()  → system suspend
 *  - m_displayOffTimeoutMs comes from PersistData::displayOff() → screen off
 *
 * If a timeout is 0, that specific feature is disabled.
 */
InputActivityFilter::InputActivityFilter(QObject *parent)
    : QObject(parent)
{
    // System suspend timeout (device off)
    m_idleTimeoutMs = Application::instance()->persistData()->deviceOff();

    // Display off timeout (screen blank + dim)
    m_displayOffTimeoutMs = Application::instance()->persistData()->displayOff();

    // Connect timeout handlers
    connect(&m_idleTimer, &QTimer::timeout,
            this, &InputActivityFilter::onIdleTimeout);

    connect(&m_displayOffTimer, &QTimer::timeout,
            this, &InputActivityFilter::onDisplayOffTimeout);

    // Configure display-off timer
    if (m_displayOffTimeoutMs > 0) {
        m_displayOffTimer.setInterval(m_displayOffTimeoutMs);
        m_displayOffTimer.setSingleShot(true);
        m_displayOffTimer.start();
        log(QString("Display-Off timer started (%1 ms).")
                .arg(m_displayOffTimeoutMs));
    }

    // Configure idle (suspend) timer
    if (m_idleTimeoutMs > 0) {
        m_idleTimer.setInterval(m_idleTimeoutMs);
        m_idleTimer.setSingleShot(true);
        m_idleTimer.start();
        log(QString("Idle timer started at app launch (%1 ms).")
                .arg(m_idleTimeoutMs));
    } else {
        log("Idle timeout = 0 → idle monitoring disabled at startup.");
    }

    // Discover backlight device, if any (for dimming)
    initBacklight();
}

/**
 * @brief Global Qt event filter that detects keyboard, mouse, and touch activity.
 *
 * When real activity is detected:
 *   - If the display was OFF → wake it, restore brightness, emit displayOn()
 *   - Restart display-off timer (if enabled)
 *   - Restart idle (suspend) timer (if enabled)
 *   - Emit userActivity()
 *
 * If both timeouts are 0, this filter effectively does nothing.
 */
bool InputActivityFilter::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);

    // If both idle and display-off are disabled, skip all logic
    if (m_idleTimeoutMs <= 0 && m_displayOffTimeoutMs <= 0)
        return QObject::eventFilter(obj, event);

    bool isRealActivity = false;

    switch (event->type()) {

    // Strong, explicit user input
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::TouchBegin:
    case QEvent::TouchEnd:
        isRealActivity = true;
        break;

    // Continuous events – only considered if display is OFF
    case QEvent::MouseMove:
    case QEvent::TouchUpdate:
        if (m_displayIsOff)
            isRealActivity = true;
        break;

    default:
        break;
    }

    if (!isRealActivity)
        return QObject::eventFilter(obj, event);

    // -------------------------------------------------
    // DISPLAY WAKE LOGIC — only once per OFF cycle
    // -------------------------------------------------
    if (m_displayIsOff) {

        const qint64 now = QDateTime::currentMSecsSinceEpoch();

        // Avoid spamming DPMS: enforce minimum 200 ms gap
        if (now - m_lastWakeTimeMs > 200) {

            // Stop any ongoing dimming (if we woke during fade)
            m_dimTimer.stop();

            // Turn the screen back ON
            system("xset dpms force on");

            // Restore original brightness (if we captured it earlier)
            if (m_originalBrightness > 0) {
                writeBrightness(m_originalBrightness);
            }

            emit displayOn();
            log("Display ON (restored brightness due to user activity).");
            m_displayIsOff = false;
        }

        m_lastWakeTimeMs = now;
    }

    // -------------------------------------------------
    // Restart timers based on current configuration
    // -------------------------------------------------

    // Restart display-off timer (only if feature enabled)
    if (m_displayOffTimeoutMs > 0)
        m_displayOffTimer.start();

    // Restart system idle (suspend) timer — device-off logic untouched
    if (m_idleTimeoutMs > 0)
        m_idleTimer.start();

    emit userActivity();
    return QObject::eventFilter(obj, event);
}

/**
 * @brief Slot executed when the idle timeout is reached.
 *
 * This means no input activity was detected within the configured
 * idle timeout. The method:
 *   - Logs the event
 *   - Emits userIdle(), appInactive(), and powerSuspend()
 *   - Invokes system suspend using `systemctl suspend`
 *
 * If idleTimeoutMs is 0, this function exits immediately.
 */
void InputActivityFilter::onIdleTimeout()
{
    log(QString("Idle timeout reached (%1 ms) → application inactive.")
            .arg(m_idleTimeoutMs));

    if (m_idleTimeoutMs <= 0)
        return;

    log("Application inactive → Idle timeout reached.");

    emit userIdle();
    emit appInactive();
    emit powerSuspend();

    log("Suspending system...");
    system("systemctl suspend");
}

/**
 * @brief Slot executed when the display-off timeout is reached.
 *
 * This means no input activity was detected within the configured
 * displayOff timeout. Instead of turning off the display immediately,
 * we first:
 *   - Start a smooth backlight dimming animation
 *   - At the end of dimming, DPMS is forced OFF and displayOff() is emitted
 */
void InputActivityFilter::onDisplayOffTimeout()
{
    if (m_displayOffTimeoutMs <= 0)
        return;

    log(QString("No activity for %1 ms → Starting dimming before display OFF.")
            .arg(m_displayOffTimeoutMs));

    startDimming();
}

/**
 * @brief Getter for the idle timeout duration.
 *
 * @return Current idle timeout in milliseconds.
 */
int InputActivityFilter::idleTimeoutMs() const
{
    return m_idleTimeoutMs;
}

/**
 * @brief Updates the idle timeout value and adjusts idle timer behavior.
 *
 * Behavior:
 *   - If `newIdleTimeoutMs <= 0`:
 *       - Idle timer stops
 *       - Idle monitoring (suspend) is disabled
 *   - If `newIdleTimeoutMs > 0`:
 *       - Idle timer restarts with the new interval
 *
 * Emits idleTimeoutMsChanged() when updated.
 */
void InputActivityFilter::setIdleTimeoutMs(int newIdleTimeoutMs)
{
    // No change
    if (m_idleTimeoutMs == newIdleTimeoutMs)
        return;

    m_idleTimeoutMs = newIdleTimeoutMs;
    emit idleTimeoutMsChanged();

    if (m_idleTimeoutMs <= 0) {
        // Disable idle-based suspend
        m_idleTimer.stop();
        log("Idle timeout set to 0 → idle (suspend) monitoring disabled.");
        return;
    }

    // Enable idle timer with new timeout
    m_idleTimer.setInterval(m_idleTimeoutMs);
    m_idleTimer.setSingleShot(true);
    m_idleTimer.start();

    log(QString("Idle timeout updated to %1 ms and restarted.")
            .arg(m_idleTimeoutMs));
}

/**
 * @brief Debug logger helper.
 *
 * Outputs messages prefixed with [ActivityMonitor].
 */
void InputActivityFilter::log(const QString &msg)
{
    qDebug().noquote() << QString("[ActivityMonitor] %1").arg(msg);
}

/**
 * @brief Getter for display-off timeout duration.
 *
 * @return Current display-off timeout in milliseconds.
 */
int InputActivityFilter::displayOffTimeoutMs() const
{
    return m_displayOffTimeoutMs;
}

/**
 * @brief Updates display-off timeout and (optionally) restarts timer.
 *
 * Behavior:
 *   - If `newDisplayOffTimeoutMs <= 0`:
 *       - Stops the display-off timer
 *       - Disables screen-off feature
 *   - If `newDisplayOffTimeoutMs > 0`:
 *       - Restarts the timer with the new interval
 */
void InputActivityFilter::setDisplayOffTimeoutMs(int newDisplayOffTimeoutMs)
{
    if (m_displayOffTimeoutMs == newDisplayOffTimeoutMs)
        return;

    m_displayOffTimeoutMs = newDisplayOffTimeoutMs;
    emit displayOffTimeoutMsChanged();

    if (m_displayOffTimeoutMs <= 0) {
        m_displayOffTimer.stop();
        log("Display-off timeout set to 0 → display-off disabled.");
        return;
    }

    m_displayOffTimer.setInterval(m_displayOffTimeoutMs);
    m_displayOffTimer.setSingleShot(true);
    m_displayOffTimer.start();

    log(QString("Display-off timeout updated to %1 ms and restarted.")
            .arg(m_displayOffTimeoutMs));
}

/**
 * @brief Detects the first available backlight device under /sys/class/backlight
 *        and stores its path for further brightness control.
 */
void InputActivityFilter::initBacklight()
{
    QDir dir("/sys/class/backlight");
    QStringList entries = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    if (entries.isEmpty()) {
        log("No backlight device found under /sys/class/backlight.");
        return;
    }

    m_backlightPath = "/sys/class/backlight/" + entries.first() + "/";
    log("Backlight device detected at: " + m_backlightPath);
}

/**
 * @brief Reads the current brightness value from the backlight device.
 *
 * @return The brightness value (int), or -1 on failure.
 */
int InputActivityFilter::readBrightness()
{
    if (m_backlightPath.isEmpty())
        return -1;

    QFile f(m_backlightPath + "brightness");
    if (!f.open(QIODevice::ReadOnly))
        return -1;

    return QString(f.readAll()).trimmed().toInt();
}

/**
 * @brief Writes the given brightness value to the backlight device.
 *
 * @param value New brightness level to write.
 */
void InputActivityFilter::writeBrightness(int value)
{
    if (m_backlightPath.isEmpty())
        return;

    QFile f(m_backlightPath + "brightness");
    if (!f.open(QIODevice::WriteOnly))
        return;

    f.write(QString::number(value).toUtf8());
}

/**
 * @brief Starts a smooth dimming animation of the backlight, then turns
 *        the display completely OFF using DPMS.
 *
 * Steps:
 *  1. Read and store current brightness as m_originalBrightness.
 *  2. Gradually reduce brightness down to m_dimmedBrightness in 'steps'.
 *  3. After dimming completes:
 *       - Force DPMS OFF (xset dpms force off)
 *       - Emit displayOff()
 *       - Set m_displayIsOff = true
 */
void InputActivityFilter::startDimming()
{
    if (m_backlightPath.isEmpty())
        return;

    // Capture current brightness to restore later on wake
    m_originalBrightness = readBrightness();
    if (m_originalBrightness <= 0)
        return;

    // Stop any previous dimming and clear connections to avoid duplicates
    m_dimTimer.stop();
    disconnect(&m_dimTimer, nullptr, nullptr, nullptr);

    const int steps  = 10;
    const int target = m_dimmedBrightness;   // low, but still visible
    int current      = m_originalBrightness;

    if (current <= target)
        return;

    const int stepValue = (current - target) / steps;
    const int interval  = 1500 / steps;      // ~1.5s total dimming

    connect(&m_dimTimer, &QTimer::timeout, this, [=]() mutable {

        current -= stepValue;

        if (current <= target) {
            writeBrightness(target);
            m_dimTimer.stop();

            // Dimming complete → now actually turn the panel off
            system("xset dpms force off");
            emit displayOff();
            m_displayIsOff = true;

            log("Dimming complete → display turned OFF (DPMS).");
            return;
        }

        writeBrightness(current);
    });

    m_dimTimer.start(interval);
    log("Backlight dimming started.");
}
