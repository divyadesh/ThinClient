#include "wifimonitor.h"
#include <QDebug>
#include <QProcess>
#include <QDBusConnection>
#include <QtConcurrent/QtConcurrent>
#include <QDBusReply>

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

    // --- NEW: find Wi-Fi device and connect to its StateChanged(new, old, reason) ---

    QDBusInterface nm(
        "org.freedesktop.NetworkManager",
        "/org/freedesktop/NetworkManager",
        "org.freedesktop.NetworkManager",
        QDBusConnection::systemBus()
        );

    QDBusReply<QList<QDBusObjectPath>> devicesReply = nm.call("GetDevices");
    if (!devicesReply.isValid()) {
        qWarning() << "[WiFiMonitor] GetDevices failed:" << devicesReply.error().message();
    } else {
        const auto devices = devicesReply.value();
        for (const QDBusObjectPath &path : devices) {
            QDBusInterface dev(
                "org.freedesktop.NetworkManager",
                path.path(),
                "org.freedesktop.NetworkManager.Device",
                QDBusConnection::systemBus()
                );

            if (!dev.isValid())
                continue;

            // DeviceType: 2 == NM_DEVICE_TYPE_WIFI
            uint devType = dev.property("DeviceType").toUInt();
            if (devType == 2) {
                m_wifiDevicePath = path.path();
                qDebug() << "[WiFiMonitor] Using WiFi device at" << m_wifiDevicePath;

                bool devOk = QDBusConnection::systemBus().connect(
                    "org.freedesktop.NetworkManager",
                    m_wifiDevicePath,
                    "org.freedesktop.NetworkManager.Device",
                    "StateChanged",
                    this,
                    SLOT(deviceStateChanged(uint,uint,uint))
                    );

                if (!devOk) {
                    qWarning() << "[WiFiMonitor] Failed to connect Device.StateChanged";
                }
                break;
            }
        }
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

void WifiMonitor::deviceStateChanged(uint newState, uint oldState, uint reason)
{
    Q_UNUSED(oldState);

    QString userMessage = reasonToUserMessage(reason);
    QString reasonStr = reasonToString(reason);

    qDebug() << "[WiFiMonitor] Device state change:"
             << "newState =" << newState
             << "reason =" << reason
             << "(" << reasonStr << ")"
             << "Message:" << userMessage;

    emit infoMessage(QString("WiFi State changed: %1 (%2)").arg(reasonStr, userMessage));

    if (newState == 120) { // NM_DEVICE_STATE_FAILED

        if (isAuthError(reason)) {
            qWarning() << "[WiFiMonitor] Authentication failed.";
            emit errorMessage("Authentication failed. Please check your Wi-Fi password.");
            emit wifiAuthFailed();
            return;
        }

        if (isSsidNotFound(reason)) {
            qWarning() << "[WiFiMonitor] SSID not found.";
            emit errorMessage("Wi-Fi network not found.");
            return;
        }

        // Generic error
        emit errorMessage("Wi-Fi connection failed: " + userMessage);
        return;
    }

    // Successful connection
    if (newState == 100) { // NM_DEVICE_STATE_ACTIVATED
        emit successMessage("Connected to Wi-Fi successfully.");
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

bool WifiMonitor::isAuthError(uint reason) const
{
    return (
        reason == 7  || // no-secrets (wrong password)
        reason == 9  ||
        reason == 10 ||
        reason == 11 ||
        reason == 13 ||
        reason == 14
        );
}

bool WifiMonitor::isSsidNotFound(uint reason) const
{
    // AP not visible
    return (reason == 53);  // NM_DEVICE_STATE_REASON_SSID_NOT_FOUND
}

QString WifiMonitor::reasonToString(uint reason) const
{
    switch (reason) {
    case 7:  return "no-secrets";                 // Missing/Wrong password
    case 8:  return "supplicant-disconnect";
    case 9:  return "supplicant-config-failed";
    case 10: return "supplicant-failed";
    case 11: return "supplicant-timeout";
    case 53: return "ssid-not-found";
    default: return "other";
    }
}

QString WifiMonitor::reasonToUserMessage(uint reason) const
{
    switch (reason) {

    case 0:  return "No reason provided.";
    case 1:  return "Unknown internal error.";
    case 2:  return "Device is now performing an assumption change.";
    case 3:  return "NetworkManager configuration changed.";
    case 4:  return "The device was now unmanaged.";
    case 5:  return "The device was unavailable.";
    case 6:  return "The device was disconnected.";

    case 7:  return "Authentication required or incorrect Wi-Fi password.";
    case 8:  return "Wi-Fi authentication service disconnected unexpectedly.";
    case 9:  return "Wi-Fi authentication configuration failed.";
    case 10: return "Wi-Fi authentication failed.";
    case 11: return "Wi-Fi authentication timed out.";

    case 12: return "802.1X certification invalid or missing.";
    case 13: return "802.1X authentication failed.";
    case 14: return "802.1X authentication timed out.";

    case 15: return "PPP connection failed.";
    case 16: return "PPP disconnected.";
    case 17: return "PPP failed to start.";

    case 18: return "DHCP negotiation failed.";
    case 19: return "DHCP client failed.";
    case 20: return "DHCP timeout.";

    case 21: return "Shared connection service failed.";
    case 22: return "Shared connection service could not start.";

    case 23: return "AutoIP configuration failed.";
    case 24: return "AutoIP negotiation failed.";
    case 25: return "AutoIP timeout.";

    case 26: return "WiMAX scan failed.";
    case 27: return "WiMAX network not found.";
    case 28: return "WiMAX connection failed.";

    case 29: return "Wi-Fi scan failed.";
    case 30: return "Wi-Fi authentication service unavailable.";
    case 31: return "Wi-Fi hardware radio is disabled.";
    case 32: return "Wi-Fi network out of range.";
    case 33: return "Wi-Fi association failed.";
    case 34: return "Wi-Fi handshake failed.";
    case 35: return "Wi-Fi configuration error.";
    case 36: return "Wi-Fi connection blocked by RF kill switch.";

    case 37: return "Network stopped unexpectedly.";
    case 38: return "Network failed due to dependency failure.";

    case 39: return "Modem not found.";
    case 40: return "Modem busy.";
    case 41: return "Modem connection failed.";

    case 42: return "Bluetooth authentication failed.";
    case 43: return "Bluetooth connection failed.";

    case 44: return "SIM card missing.";
    case 45: return "SIM card failure.";
    case 46: return "SIM PIN required.";
    case 47: return "SIM PUK required.";
    case 48: return "SIM locked.";

    case 49: return "GSM registration failed.";
    case 50: return "GSM signal lost.";
    case 51: return "GSM authentication failed.";

    case 52: return "Connection disabled.";
    case 53: return "Wi-Fi network (SSID) not found.";

    case 54: return "Connection activation failed.";
    case 55: return "Connection attempt cancelled by user.";

    case 56: return "Connection invalid.";
    case 57: return "Connection requires secrets (passwords).";
    case 58: return "VPN service failed.";
    case 59: return "VPN connection failed.";
    case 60: return "VPN disconnection.";
    case 61: return "VPN timeout.";

    case 62: return "Connection failed due to modem firmware issue.";
    case 63: return "Modem not initialized.";

    case 64: return "IP configuration failed.";
    case 65: return "IP configuration not available.";
    case 66: return "IP addressing error.";

    case 67: return "Host unreachable.";
    case 68: return "Connection exceeded retry limits.";

    default:
        return "Unknown error occurred.";
    }
}


