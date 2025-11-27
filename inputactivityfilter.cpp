/**
 * @file inputactivityfilter.cpp
 * @brief Implementation of InputActivityFilter, which monitors user input
 *        events and triggers system suspend after inactivity.
 */

#include "inputactivityfilter.h"

#include <QDebug>
#include <QCoreApplication>
#include <QEvent>
#include <stdlib.h>

/**
 * @brief Constructor. Initializes the idle timeout and starts the timer
 *        if the configured timeout is greater than zero.
 *
 * The timeout value is retrieved from PersistData via Application::instance().
 * If the timeout is 0, inactivity monitoring is disabled.
 *
 * @param parent Parent QObject
 */
InputActivityFilter::InputActivityFilter(QObject *parent)
    : QObject(parent)
{
    m_idleTimeoutMs = Application::instance()->persistData()->deviceOff();

    // Connect timeout handler only once
    connect(&m_idleTimer, &QTimer::timeout,
            this, &InputActivityFilter::onIdleTimeout);

    if (m_idleTimeoutMs > 0) {
        m_idleTimer.setInterval(m_idleTimeoutMs);
        m_idleTimer.setSingleShot(true);
        m_idleTimer.start();
        log(QString("Idle timer started at app launch (%1 ms).").arg(m_idleTimeoutMs));
    } else {
        log("Idle timeout = 0 → idle monitoring disabled at startup.");
    }
}

/**
 * @brief Global Qt event filter that detects keyboard, mouse, and touch activity.
 *
 * When activity is detected:
 *   - The idle timer is restarted.
 *   - The userActivity() signal is emitted.
 *
 * When idle monitoring is disabled (timeout = 0), the event is ignored.
 *
 * @param obj   Destination QObject of the event
 * @param event Event received
 * @return Always returns QObject::eventFilter() unless timer is disabled.
 */
bool InputActivityFilter::eventFilter(QObject *obj, QEvent *event)
{
    // If idle timeout is disabled, skip all logic
    if (m_idleTimeoutMs <= 0)
        return QObject::eventFilter(obj, event);

    switch (event->type()) {

    // Full activity set
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::MouseMove:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:

        // Restart idle timer
        m_idleTimer.start();
        emit userActivity();
        break;

    default:
        break;
    }

    return QObject::eventFilter(obj, event);
}

/**
 * @brief Slot executed when the idle timeout is reached.
 *
 * This means no input activity was detected within the configured time.
 * The method:
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
 * @brief Getter for the idle timeout duration.
 *
 * @return Current timeout in milliseconds
 */
int InputActivityFilter::idleTimeoutMs() const
{
    return m_idleTimeoutMs;
}

/**
 * @brief Updates the idle timeout value and adjusts timer behavior.
 *
 * Behavior:
 *   - If `newIdleTimeoutMs <= 0`: timer stops, monitoring disabled.
 *   - If `newIdleTimeoutMs > 0`: timer restarts with new interval.
 *
 * Emits idleTimeoutMsChanged() when updated.
 *
 * @param newIdleTimeoutMs Idle timeout in milliseconds.
 */
void InputActivityFilter::setIdleTimeoutMs(int newIdleTimeoutMs)
{
    // No change
    if (m_idleTimeoutMs == newIdleTimeoutMs)
        return;

    m_idleTimeoutMs = newIdleTimeoutMs;
    emit idleTimeoutMsChanged();

    if (m_idleTimeoutMs <= 0) {
        // Disable timer completely
        m_idleTimer.stop();
        log("Idle timeout set to 0 → idle monitoring disabled.");
        return;
    }

    // Enable timer with new timeout
    m_idleTimer.setInterval(m_idleTimeoutMs);
    m_idleTimer.setSingleShot(true);
    m_idleTimer.start();

    log(QString("Idle timeout updated to %1 ms and restarted.").arg(m_idleTimeoutMs));
}

/**
 * @brief Helper for formatted debug logging.
 *
 * Outputs messages in the form:
 *   [ActivityMonitor] message
 *
 * @param msg The message to print
 */
void InputActivityFilter::log(const QString &msg)
{
    qDebug().noquote() << QString("[ActivityMonitor] %1").arg(msg);
}
