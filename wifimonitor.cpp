#include "wifimonitor.h"
#include <QDebug>
#include <QProcess>
#include <QDBusConnection>
#include <QtConcurrent/QtConcurrent>

WifiMonitor::WifiMonitor(QObject *parent)
    : QObject(parent), m_connected(false)
{
    // Connect to NetworkManager "StateChanged" DBus signal
    bool ok = QDBusConnection::systemBus().connect(
        "org.freedesktop.NetworkManager",
        "/org/freedesktop/NetworkManager",
        "org.freedesktop.NetworkManager",
        "StateChanged",
        this,
        SLOT(nmStateChanged(uint))
        );

    if (!ok) {
        qWarning() << "[WiFiMonitor] Failed to connect to NetworkManager DBus signals.";
    } else {
        qDebug() << "[WiFiMonitor] Listening to WiFi state changes...";
    }

    (void)QtConcurrent::run([this](){
        QProcess process;
        process.start("nmcli -t -f STATE g");
        process.waitForFinished(1000);
        QString state = process.readAllStandardOutput().trimmed();
        setConnected(state == "connected");
    });
}

void WifiMonitor::initialize()
{
    QDBusInterface nm(
        "org.freedesktop.NetworkManager",
        "/org/freedesktop/NetworkManager",
        "org.freedesktop.NetworkManager",
        QDBusConnection::systemBus()
        );

    if (!nm.isValid()) {
        qWarning() << "[WiFiMonitor] Failed to access NetworkManager.";
        return;
    }

    uint state = nm.property("State").toUInt();

    qDebug() << "[WiFiMonitor] Initial NetworkManager state =" << state;

    // Use your same nmStateChanged logic:
    nmStateChanged(state);

    // Optional: print SSID if connected
    if (isConnected()) {
        QString ssid = getActiveSSID();
        qDebug() << "[WiFiMonitor] Initial WiFi connected to SSID:" << ssid;
    }
}

bool WifiMonitor::isConnected() const
{
    return m_connected;
}

void WifiMonitor::setConnected(bool connected)
{
    if (m_connected != connected) {
        m_connected = connected;
        emit connectedChanged(m_connected);
    }
}

void WifiMonitor::nmStateChanged(uint newState)
{
    QString stateName;

    switch (newState) {

    // ───────────────────────────────────────────────
    // 10 → Networking disabled
    // ───────────────────────────────────────────────
    case 10: // NM_STATE_ASLEEP
        stateName = "asleep";
        qDebug() << "[WiFiMonitor] NM_STATE_ASLEEP: Networking disabled";
        setConnected(false);
        emit wifiDisconnected();
        break;

    // ───────────────────────────────────────────────
    // 20 → Disconnected
    // ───────────────────────────────────────────────
    case 20: // NM_STATE_DISCONNECTED
        stateName = "disconnected";
        qDebug() << "[WiFiMonitor] NM_STATE_DISCONNECTED: WiFi disconnected";
        setConnected(false);
        emit wifiDisconnected();
        break;

    // ───────────────────────────────────────────────
    // 30 → Disconnecting...
    // ───────────────────────────────────────────────
    case 30: // NM_STATE_DISCONNECTING
        stateName = "disconnecting";
        qDebug() << "[WiFiMonitor] NM_STATE_DISCONNECTING: Disconnecting...";
        setConnected(false);
        break;

    // ───────────────────────────────────────────────
    // 40 → Connecting (starting)
    // ───────────────────────────────────────────────
    case 40: // NM_STATE_CONNECTING
        stateName = "connecting";
        qDebug() << "[WiFiMonitor] NM_STATE_CONNECTING: Starting connection...";
        break;

    case 50: // NM_STATE_CONNECTED_LOCAL
    {
        stateName = "connected-local";
        QString ssid = getActiveSSID();
        qDebug() << "[WiFiMonitor] NM_STATE_CONNECTED_LOCAL: Local connectivity on" << ssid;
        setConnected(true);
        emit wifiConnected(ssid);
        break;
    }

    case 60: // NM_STATE_CONNECTED_SITE
    {
        stateName = "connected-site";
        QString ssid = getActiveSSID();
        qDebug() << "[WiFiMonitor] NM_STATE_CONNECTED_SITE: Site-wide connectivity on" << ssid;
        setConnected(true);
        emit wifiConnected(ssid);
        break;
    }

    case 70: // NM_STATE_CONNECTED_GLOBAL
    {
        stateName = "connected-global";
        QString ssid = getActiveSSID();
        qDebug() << "[WiFiMonitor] NM_STATE_CONNECTED_GLOBAL: Full Internet connectivity on" << ssid;
        setConnected(true);
        emit wifiConnected(ssid);
        break;
    }

    default:
        stateName = "unknown";
        qWarning() << "[WiFiMonitor] UNKNOWN NMState:" << newState;
        break;
    }

    // Emit readable state string to UI
    emit wifiStateChanged(stateName);
}

QString WifiMonitor::getActiveSSID()
{
    QProcess p;
    p.start("sh", QStringList() << "-c" <<
                      "nmcli -t -f ACTIVE,SSID dev wifi | grep '^yes' | cut -d: -f2");
    p.waitForFinished();

    return QString(p.readAllStandardOutput()).trimmed();
}
