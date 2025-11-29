/**
 * @file inputactivityfilter.cpp
 * @brief HDMI-only display off/on + inactivity suspend logic.
 */

#include "inputactivityfilter.h"

#include <QDebug>
#include <QEvent>
#include <QDir>
#include <QFile>
#include <QDateTime>
#include <stdlib.h>

/* ============================================================
 * Constructor
 * ============================================================ */
InputActivityFilter::InputActivityFilter(QObject *parent)
    : QObject(parent)
{
    m_idleTimeoutMs        = Application::instance()->persistData()->deviceOff();
    m_displayOffTimeoutMs  = Application::instance()->persistData()->displayOff();

    connect(&m_idleTimer,        &QTimer::timeout, this, &InputActivityFilter::onIdleTimeout);
    connect(&m_displayOffTimer,  &QTimer::timeout, this, &InputActivityFilter::onDisplayOffTimeout);

    // Start timers if enabled
    if (m_displayOffTimeoutMs > 0) {
        m_displayOffTimer.setInterval(m_displayOffTimeoutMs);
        m_displayOffTimer.setSingleShot(true);
        m_displayOffTimer.start();
        log(QString("Display-Off timer started (%1 ms)").arg(m_displayOffTimeoutMs));
    }

    if (m_idleTimeoutMs > 0) {
        m_idleTimer.setInterval(m_idleTimeoutMs);
        m_idleTimer.setSingleShot(true);
        m_idleTimer.start();
        log(QString("Idle timer started (%1 ms)").arg(m_idleTimeoutMs));
    }

    autoDetectHdmi();
}

/* ============================================================
 * HDMI AUTO-DETECT
 * ============================================================ */
void InputActivityFilter::autoDetectHdmi()
{
    QDir dir("/sys/class/drm");
    QStringList entries = dir.entryList(QStringList() << "card*-*-*", QDir::Dirs);

    foreach (QString e, entries) {
        QString statusFile = "/sys/class/drm/" + e + "/status";
        if (QFile::exists(statusFile)) {
            m_hdmiStatusPath = statusFile;
            log("HDMI connector found: " + statusFile);
            return;
        }
    }

    log("ERROR: No HDMI connector found under /sys/class/drm/");
}

/* ============================================================
 * EVENT FILTER
 * ============================================================ */
bool InputActivityFilter::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);

    bool isRealActivity = false;

    switch (event->type()) {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::TouchBegin:
    case QEvent::TouchEnd:
        isRealActivity = true;
        break;

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

    /* Wake HDMI if it is OFF */
    if (m_displayIsOff) {

        qint64 now = QDateTime::currentMSecsSinceEpoch();
        if (now - m_lastWakeTimeMs > 200) {

            writeHdmiStatus("on");

            m_displayIsOff = false;
            emit displayOn();
            log("HDMI ON (wake by activity)");
        }
        m_lastWakeTimeMs = now;
    }

    if (m_displayOffTimeoutMs > 0)
        m_displayOffTimer.start();

    if (m_idleTimeoutMs > 0)
        m_idleTimer.start();

    emit userActivity();
    return QObject::eventFilter(obj, event);
}

/* ============================================================
 * SYSTEM SUSPEND TIMEOUT
 * ============================================================ */
void InputActivityFilter::onIdleTimeout()
{
    if (m_idleTimeoutMs <= 0)
        return;

    log(QString("Idle timeout reached (%1 ms)").arg(m_idleTimeoutMs));

    emit userIdle();
    emit appInactive();
    emit powerSuspend();

    system("systemctl suspend");
}

/* ============================================================
 * DISPLAY OFF TIMEOUT (HDMI ONLY)
 * ============================================================ */
void InputActivityFilter::onDisplayOffTimeout()
{
    if (m_displayOffTimeoutMs <= 0)
        return;

    if (!m_hdmiStatusPath.isEmpty()) {
        writeHdmiStatus("off");
        m_displayIsOff = true;
        emit displayOff();
        log("HDMI OFF (timeout)");
        return;
    }

    log("ERROR: Unable to turn off display (no HDMI status file)");
}

/* ============================================================
 * HDMI STATUS WRITE
 * ============================================================ */
void InputActivityFilter::writeHdmiStatus(const QString &mode)
{
    if (m_hdmiStatusPath.isEmpty())
        return;

    QFile f(m_hdmiStatusPath);
    if (!f.open(QIODevice::WriteOnly))
        return;

    f.write(mode.toUtf8());
}

/* ============================================================
 * LOGGING
 * ============================================================ */
void InputActivityFilter::log(const QString &msg)
{
    qDebug().noquote() << "[ActivityMonitor]" << msg;
}

/* ============================================================
 * PROPERTY SETTERS (UNCHANGED)
 * ============================================================ */
int InputActivityFilter::idleTimeoutMs() const { return m_idleTimeoutMs; }
int InputActivityFilter::displayOffTimeoutMs() const { return m_displayOffTimeoutMs; }

void InputActivityFilter::setIdleTimeoutMs(int ms)
{
    if (m_idleTimeoutMs == ms) return;
    m_idleTimeoutMs = ms;
    emit idleTimeoutMsChanged();

    if (ms <= 0) { m_idleTimer.stop(); return; }

    m_idleTimer.setInterval(ms);
    m_idleTimer.start();
}

void InputActivityFilter::setDisplayOffTimeoutMs(int ms)
{
    if (m_displayOffTimeoutMs == ms) return;
    m_displayOffTimeoutMs = ms;
    emit displayOffTimeoutMsChanged();

    if (ms <= 0) { m_displayOffTimer.stop(); return; }

    m_displayOffTimer.setInterval(ms);
    m_displayOffTimer.start();
}
