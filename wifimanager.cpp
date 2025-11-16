#include "wifimanager.h"
#include <QProcess>
#include <QRegularExpression>
#include <QFile>
#include <QTextStream>

WiFiManager::WiFiManager(QObject *parent)
    : QObject(parent),
    m_signalStrength(0)
{
    connect(&m_timer, &QTimer::timeout, this, &WiFiManager::refresh);
    m_timer.setInterval(5000); // 5 seconds
}

QString WiFiManager::run(const QString &cmd, const QStringList &args) const
{
    QProcess p;
    p.start(cmd, args);
    p.waitForFinished();
    return QString::fromLocal8Bit(p.readAllStandardOutput());
}

QString WiFiManager::cidrToNetmask(int cidr) const
{
    quint32 mask = (cidr == 0 ? 0 : 0xFFFFFFFF << (32 - cidr));
    return QString("%1.%2.%3.%4")
        .arg((mask >> 24) & 0xFF)
        .arg((mask >> 16) & 0xFF)
        .arg((mask >> 8) & 0xFF)
        .arg(mask & 0xFF);
}

void WiFiManager::setSsid(const QString &ssid)
{
    if (m_ssid == ssid)
        return;

    m_ssid = ssid;
    emit ssidChanged();
}

void WiFiManager::startAutoRefresh()
{
    emit logMessage("[WiFi] Auto-refresh started (5s)");
    refresh();
    m_timer.start();
}

void WiFiManager::stopAutoRefresh()
{
    emit logMessage("[WiFi] Auto-refresh stopped");
    m_timer.stop();
}

void WiFiManager::refresh()
{
    emit logMessage("=========== WiFi Refresh ===========");

    // ---------- LOG SSID ----------
    emit logMessage("SSID: " + m_ssid);

    // ---------- SIGNAL STRENGTH ----------
    QString iw = run("iw", {"dev", "wlan0", "link"});
    QRegularExpression reSignal("signal:\\s*(-?\\d+) dBm");

    auto s = reSignal.match(iw);
    if (s.hasMatch()) {
        m_signalStrength = s.captured(1).toInt();
        emit signalStrengthChanged();
        emit logMessage("Signal: " + QString::number(m_signalStrength) + " dBm");
    }

    // ---------- SECURITY ----------
    m_security = "WPA2-PSK"; // placeholder
    emit securityChanged();

    // ---------- MAC ----------
    QString mac = run("cat", {"/sys/class/net/wlan0/address"});
    if (!mac.isEmpty()) {
        m_macAddress = mac.trimmed();
        emit macAddressChanged();
    }

    // ---------- LINK SPEED ----------
    QString speed = run("cat", {"/sys/class/net/wlan0/speed"});
    if (!speed.isEmpty()) {
        m_linkSpeed = speed.trimmed() + " Mbps";
        emit linkSpeedChanged();
    }

    // ---------- IP ----------
    QString ipOut = run("ip", {"addr", "show", "dev", "wlan0"});
    QRegularExpression reIp("inet ([0-9.]+)/([0-9]+)");

    auto m = reIp.match(ipOut);
    if (m.hasMatch()) {
        m_ipAddress = m.captured(1);
        m_subnetMask = cidrToNetmask(m.captured(2).toInt());

        emit ipAddressChanged();
        emit subnetMaskChanged();
    }

    // ---------- GATEWAY ----------
    QString route = run("ip", {"route", "show"});
    QRegularExpression reGw("default via ([0-9.]+)");

    auto gw = reGw.match(route);
    if (gw.hasMatch()) {
        m_gateway = gw.captured(1);
        emit gatewayChanged();
    }

    // ---------- DNS ----------
    QStringList list;
    QFile f("/etc/resolv.conf");
    if (f.open(QIODevice::ReadOnly)) {
        QTextStream in(&f);

        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("nameserver"))
                list << line.split(" ")[1].trimmed();
        }
        f.close();
    }

    if (!list.isEmpty()) {
        m_dnsServers = list;
        emit dnsServersChanged();
    }

    emit logMessage("=========== End Refresh ===========");
}

// getters
QString WiFiManager::ssid() const { return m_ssid; }
QString WiFiManager::security() const { return m_security; }
QString WiFiManager::password() const { return m_password; }
QString WiFiManager::status() const { return m_status; }
int WiFiManager::signalStrength() const { return m_signalStrength; }
QString WiFiManager::linkSpeed() const { return m_linkSpeed; }
QString WiFiManager::macAddress() const { return m_macAddress; }
QString WiFiManager::ipAddress() const { return m_ipAddress; }
QString WiFiManager::subnetMask() const { return m_subnetMask; }
QString WiFiManager::gateway() const { return m_gateway; }
QStringList WiFiManager::dnsServers() const { return m_dnsServers; }
