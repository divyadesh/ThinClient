#include "wifimanager.h"
#include <QProcess>
#include <QRegularExpression>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QFutureWatcher>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

static QString runProcess(const QString &cmd, const QStringList &args)
{
    QProcess p;
    p.setProgram(cmd);
    p.setArguments(args);

    p.start();
    if (!p.waitForStarted(3000))
        return "";

    if (!p.waitForFinished(15000)) {
        p.kill();
        p.waitForFinished();
        return "";
    }

    return QString::fromLocal8Bit(p.readAllStandardOutput());
}

WiFiManager::WiFiManager(QObject *parent)
    : QObject(parent),
    m_signalStrength(0)
{
    connect(&m_timer, &QTimer::timeout, this, &WiFiManager::refresh);
    m_timer.setInterval(20000); // 20 seconds
}

void WiFiManager::runAsync(
    const QString &cmd,
    const QStringList &args,
    std::function<void(QString)> callback)
{
    auto future = QtConcurrent::run([cmd, args]() {
        return runProcess(cmd, args);
    });

    // When done, call callback on the UI thread
    auto watcher = new QFutureWatcher<QString>();
    connect(watcher, &QFutureWatcher<QString>::finished, this, [watcher, callback]() {
        QString result = watcher->result();
        watcher->deleteLater();
        callback(result);   // return output to caller
    });

    watcher->setFuture(future);
}

QString WiFiManager::run(const QString &cmd, const QStringList &args) const
{
    QProcess p;
    p.setProgram(cmd);
    p.setArguments(args);

    p.start();
    if (!p.waitForStarted(3000)) {
        return "";
    }

    // Wait up to 15 seconds (nmcli down/up can take long)
    if (!p.waitForFinished(15000)) {

        p.kill();
        p.waitForFinished();
        return "";
    }

    return QString::fromLocal8Bit(p.readAllStandardOutput());
}

bool WiFiManager::runBool(const QString &cmd, const QStringList &args) const
{
    QProcess p;
    p.setProgram(cmd);
    p.setArguments(args);

    // 1. Start command
    p.start();
    if (!p.waitForStarted(3000)) {
        qWarning() << "[WiFi] Failed to start:" << cmd << args;
        return false;
    }

    // 2. Wait for completion (up to 15 seconds)
    if (!p.waitForFinished(15000)) {
        qWarning() << "[WiFi] Command timed out:" << cmd << args;
        p.kill();
        p.waitForFinished();
        return false;
    }

    // 3. Check exit code
    if (p.exitCode() != 0) {
        QString stderrOut = QString::fromLocal8Bit(p.readAllStandardError()).trimmed();
        qWarning() << "[WiFi] Command failed with exit code"
                   << p.exitCode() << ":" << cmd << args
                   << "Error:" << stderrOut;
        return false;
    }

    return true;
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

// bool WiFiManager::setDhcpWorker()
// {
//     emit logMessage("========== WiFi: Set DHCP ==========");

//     // 1) Get current Wi-Fi connection name
//     QString conName = getCurrentConnectionName();
//     if (conName.isEmpty()) {
//         emit logMessage("[WiFi] ERROR: No active WiFi connection found!");
//         return false;
//     }

//     QString iface = m_activeInterface.isEmpty() ? "wlan0" : m_activeInterface;
//     emit logMessage("[WiFi] Active interface = " + iface);
//     emit logMessage("[WiFi] Switching IPv4 mode to DHCP...");

//     // -------------------------------------------
//     // STEP 1 — CLEAR gateway FIRST (NM requirement)
//     // -------------------------------------------
//     if (!runBool("nmcli", {"connection", "modify", conName, "ipv4.gateway", ""})) {
//         emit logMessage("[WiFi] WARN: Could not clear ipv4.gateway (continuing)");
//     }

//     // -------------------------------------------
//     // STEP 2 — Clear IP addresses
//     // -------------------------------------------
//     if (!runBool("nmcli", {"connection", "modify", conName, "ipv4.addresses", ""})) {
//         emit logMessage("[WiFi] WARN: Could not clear ipv4.addresses (continuing)");
//     }

//     // -------------------------------------------
//     // STEP 3 — Clear DNS
//     // -------------------------------------------
//     if (!runBool("nmcli", {"connection", "modify", conName, "ipv4.dns", ""})) {
//         emit logMessage("[WiFi] WARN: Could not clear ipv4.dns (continuing)");
//     }

//     // -------------------------------------------
//     // STEP 4 — Set DHCP mode
//     // -------------------------------------------
//     if (!runBool("nmcli", {"connection", "modify", conName, "ipv4.method", "auto"})) {
//         emit logMessage("[WiFi] ERROR: Failed to set ipv4.method=auto");
//         return false;
//     }

//     // 3) Flush all IPv4 addresses from the interface (important!)
//     emit logMessage("[WiFi] Flushing IPs on device: " + iface);
//     runBool("ip", {"addr", "flush", "dev", iface});

//     // ---------------------------------------------------
//     // STEP 5 — Safe reconnect (device disconnect + up)
//     // ---------------------------------------------------
//     emit logMessage("[WiFi] Disconnecting device " + iface + "...");
//     runBool("nmcli", {"device", "disconnect", iface});

//     emit logMessage("[WiFi] Reconnecting to " + conName + "...");
//     if (!runBool("nmcli", {"connection", "up", conName})) {
//         emit logMessage("[WiFi] ERROR: Failed to reconnect");
//         return false;
//     }

//     emit logMessage("[WiFi] DHCP mode enabled successfully.");
//     return true;
// }

bool WiFiManager::setDhcpWorker()
{
    emit logMessage("========== WiFi: Set DHCP ==========");

    QString conName = getCurrentConnectionName();
    if (conName.isEmpty()) {
        emit logMessage("[WiFi] ERROR: No active WiFi connection found!");
        return false;
    }

    QString iface = m_activeInterface.isEmpty() ? "p2p0" : m_activeInterface;
    emit logMessage("[WiFi] Active interface = " + iface);

    emit logMessage("[WiFi] Switching IPv4 mode to DHCP...");

    // 1) Set DHCP method FIRST
    if (!runBool("nmcli", {"connection", "modify", conName, "ipv4.method", "auto"})) {
        emit logMessage("[WiFi] ERROR: Failed to set ipv4.method=auto");
        return false;
    }

    // 2) Disconnect device (this removes runtime static IPv4)
    emit logMessage("[WiFi] Disconnecting device...");
    runBool("nmcli", {"device", "disconnect", iface});

    // 3) Now safe to clear static fields from profile
    runBool("nmcli", {"connection", "modify", conName, "ipv4.gateway", ""});
    runBool("nmcli", {"connection", "modify", conName, "ipv4.addresses", ""});
    runBool("nmcli", {"connection", "modify", conName, "ipv4.dns", ""});

    // 4) Flush leftover kernel IPs (NOW this is guaranteed to work)
    emit logMessage("[WiFi] Flushing runtime IPs...");
    runBool("ip", {"addr", "flush", "dev", iface});

    // 5) Reconnect using DHCP
    emit logMessage("[WiFi] Reconnecting...");
    if (!runBool("nmcli", {"connection", "up", conName})) {
        emit logMessage("[WiFi] ERROR: Failed to reconnect");
        return false;
    }

    emit logMessage("[WiFi] DHCP mode enabled successfully.");
    return true;
}

void WiFiManager::setDhcpAsync()
{
    setBusy(true);
    emit processStarted();   // For UI BusyIndicator

    QtConcurrent::run([=]() {
        bool success = setDhcpWorker();

        QString message = success
                              ? "WiFi DHCP mode enabled successfully."
                              : "Failed to enable WiFi DHCP mode.";

        // Must invoke back on main thread:
        QMetaObject::invokeMethod(this, [this, success, message]() {
            emit connectionCompleted(success, message);
            setBusy(false);
        });
    });
}

void WiFiManager::setStaticIpAsync(const QString &ip,
                                   const QString &subnetMask,
                                   const QString &gateway,
                                   const QStringList &dns)
{
    setBusy(true);
    emit processStarted();

    QtConcurrent::run([=]() {

        bool success = setStaticIpWorker(ip, subnetMask, gateway, dns);

        QString message = success
                              ? "WiFi static IP applied successfully."
                              : "Failed to apply WiFi static IP.";

        QMetaObject::invokeMethod(this, [=]() {
            emit connectionCompleted(success, message);
            setBusy(false);
        });
    });
}

bool WiFiManager::setStaticIpWorker(const QString &ip,
                                    const QString &subnetMask,
                                    const QString &gateway,
                                    const QStringList &dns)
{
    emit logMessage("========== WiFi: Set Static IP ==========");
    emit logMessage("[WiFi] Fetching active connection list...");

    QString conList = run("nmcli", { "-t", "-f", "NAME,DEVICE", "connection", "show" }).trimmed();
    emit logMessage("[WiFi] Connection list:\n" + conList);

    QString conName;
    for (const QString &line : conList.split("\n")) {
        if (line.contains("wlan0") || line.contains("p2p0")) {
            conName = line.split(":")[0];
            emit logMessage("[WiFi] Active WiFi connection detected: " + conName);
            break;
        }
    }

    if (conName.isEmpty()) {
        emit logMessage("[WiFi] ERROR: No active WiFi connection found!");
        return false;
    }

    // Convert subnet mask → CIDR
    emit logMessage("[WiFi] Converting subnet mask to CIDR: " + subnetMask);
    QStringList parts = subnetMask.split(".");
    if (parts.size() != 4) {
        emit logMessage("[WiFi] ERROR: Invalid subnet mask!");
        return false;
    }

    quint32 mask =
        (parts[0].toInt() << 24) |
        (parts[1].toInt() << 16) |
        (parts[2].toInt() << 8)  |
        (parts[3].toInt());

    int cidr = 0;
    while (mask & 0x80000000) {
        cidr++;
        mask <<= 1;
    }

    QString cidrIp = ip + "/" + QString::number(cidr);
    emit logMessage("[WiFi] Calculated CIDR IP: " + cidrIp);

    // Apply static IP settings
    if (!runBool("nmcli", { "connection", "modify", conName,
                           "ipv4.addresses", cidrIp,
                           "ipv4.gateway",   gateway,
                           "ipv4.method",    "manual" }))
        return false;

    // DNS settings
    if (!dns.isEmpty()) {
        QString dnsCombined = dns.join(",");
        emit logMessage("[WiFi] Setting DNS: " + dnsCombined);

        if (!runBool("nmcli", { "connection", "modify", conName,
                               "ipv4.dns", dnsCombined }))
            return false;
    }

    emit logMessage("[WiFi] Restarting connection...");

    if (!runBool("nmcli", { "connection", "down", conName }))
        return false;

    if (!runBool("nmcli", { "connection", "up", conName }))
        return false;

    emit logMessage("[WiFi] Static IP applied successfully.");
    return true;
}

bool WiFiManager::updateIpModeWorker()
{
    emit logMessage("========== WiFi: Update IP Mode ==========");
    qInfo() << "[WiFiManager] Updating IP mode...";

    QString conName = getCurrentConnectionName();
    if (conName.isEmpty()) {
        qWarning() << "[WiFiManager] Cannot update IP mode — no connection name.";
        return false;
    }

    qDebug() << "[WiFiManager] Querying IPv4 method for connection:" << conName;

    QString modeOut = run("nmcli", {
                                       "-t", "-f", "ipv4.method", "connection", "show", conName
                                   }).trimmed();

    if (modeOut.isEmpty()) {
        qWarning() << "[WiFiManager] ERROR: Failed to read IPv4 method.";
        return false;
    }

    qInfo() << "[WiFiManager] ipv4.method =" << modeOut;

    // Expecting something like: "ipv4.method:manual"
    QStringList parts = modeOut.split(":");
    QString mode = (parts.size() >= 2 ? parts[1].trimmed() : modeOut);

    bool isStatic = (mode == "manual");
    setIsStaticIp(isStatic);

    qInfo() << "[WiFiManager] Current IP mode:"
            << (isStatic ? "Static (manual)" : "DHCP (auto)");

    emit logMessage("========== End Update IP Mode ==========");

    return true;
}

void WiFiManager::updateIpModeAsync()
{
    setBusy(true);
    emit processStarted();

    QtConcurrent::run([=]() {

        bool success = updateIpModeWorker();

        QString message = success
                              ? "WiFi IP mode refreshed successfully."
                              : "Failed to refresh WiFi IP mode.";

        QMetaObject::invokeMethod(this, [=]() {
            emit connectionCompleted(success, message);
            setBusy(false);
            emit ipModeChanged();
        });
    });
}


bool WiFiManager::setDhcp()
{
    emit processStarted();
    emit logMessage("========== WiFi: Set DHCP ==========");

    emit logMessage("[WiFi] Fetching active connection list...");
    QString conList = run("nmcli", {"-t", "-f", "NAME,DEVICE", "connection", "show"});
    emit logMessage("[WiFi] Connection list:\n" + conList.trimmed());

    QString conName;

    // Identify correct WiFi connection
    for (const QString &line : conList.split("\n")) {
        if (line.contains("wlan0") || line.contains("p2p0")) {
            conName = line.split(":")[0];
            emit logMessage("[WiFi] Active WiFi connection detected: " + conName);
            break;
        }
    }

    if (conName.isEmpty()) {
        emit logMessage("[WiFi] ERROR: No active WiFi connection found!");
        emit logMessage("========== End Set DHCP ==========");
        return false;
    }

    emit logMessage("[WiFi] Switching IPv4 mode to DHCP for connection: " + conName);

    // ---------- Clear static settings ----------
    emit logMessage("[WiFi] Clearing IPv4 static config...");

    QString out1 = run("nmcli", {"connection", "modify", conName, "ipv4.addresses", ""});
    emit logMessage("[WiFi] ipv4.addresses cleared: " + out1.trimmed());

    QString out2 = run("nmcli", {"connection", "modify", conName, "ipv4.gateway", ""});
    emit logMessage("[WiFi] ipv4.gateway cleared: " + out2.trimmed());

    QString out3 = run("nmcli", {"connection", "modify", conName, "ipv4.dns", ""});
    emit logMessage("[WiFi] ipv4.dns cleared: " + out3.trimmed());

    // ---------- Set IPv4 mode to AUTO ----------
    emit logMessage("[WiFi] Setting ipv4.method = auto...");
    QString out4 = run("nmcli", {"connection", "modify", conName, "ipv4.method", "auto"});
    emit logMessage("[WiFi] ipv4.method set to auto: " + out4.trimmed());

    // ---------- Restart connection ----------
    emit logMessage("[WiFi] Restarting connection...");

    QString outDown = run("nmcli", {"connection", "down", conName});
    emit logMessage("[WiFi] Connection down:\n" + outDown.trimmed());

    QString outUp = run("nmcli", {"connection", "up", conName});
    emit logMessage("[WiFi] Connection up:\n" + outUp.trimmed());

    emit logMessage("[WiFi] Successfully restored DHCP mode.");
    emit logMessage("========== End Set DHCP ==========");

    emit ipModeChanged();
    emit processEnded();
    return true;
}

bool WiFiManager::setStaticIp(const QString &ip,
                              const QString &subnetMask,
                              const QString &gateway,
                              const QStringList &dns)
{
    emit processStarted();
    emit logMessage("========== WiFi: Set Static IP ==========");

    emit logMessage("[WiFi] Fetching active connection list...");
    QString conList = run("nmcli", {"-t", "-f", "NAME,DEVICE", "connection", "show"});
    emit logMessage("[WiFi] Connection list:\n" + conList.trimmed());

    QString conName;

    // Identify correct WiFi connection (wlan0 or p2p0)
    for (const QString &line : conList.split("\n")) {
        if (line.contains("wlan0") || line.contains("p2p0")) {
            conName = line.split(":")[0];
            emit logMessage("[WiFi] Active WiFi connection detected: " + conName);
            break;
        }
    }

    if (conName.isEmpty()) {
        emit logMessage("[WiFi] ERROR: No active WiFi connection found!");
        emit logMessage("========== End Set Static IP ==========");
        return false;
    }

    emit logMessage("[WiFi] Configuring static IP for: " + conName);

    // ---------- Subnet Mask → CIDR ----------
    emit logMessage("[WiFi] Converting subnet mask to CIDR: " + subnetMask);

    QStringList parts = subnetMask.split(".");
    if (parts.size() != 4) {
        emit logMessage("[WiFi] ERROR: Invalid subnet mask!");
        emit logMessage("========== End Set Static IP ==========");
        return false;
    }

    quint32 mask =
        (parts[0].toInt() << 24) |
        (parts[1].toInt() << 16) |
        (parts[2].toInt() << 8)  |
        (parts[3].toInt());

    int cidr = 0;
    while (mask & 0x80000000) {
        cidr++;
        mask <<= 1;
    }

    QString cidrIp = ip + "/" + QString::number(cidr);
    emit logMessage("[WiFi] Calculated CIDR IP: " + cidrIp);

    // ---------- Apply Static IPv4 ----------
    emit logMessage("[WiFi] Applying static IPv4 settings...");
    QString outStatic = run("nmcli", {
                                         "connection", "modify", conName,
                                         "ipv4.addresses", cidrIp,
                                         "ipv4.gateway",   gateway,
                                         "ipv4.method",    "manual"
                                     });
    emit logMessage("[WiFi] IPv4 modify result:\n" + outStatic.trimmed());

    // ---------- Apply DNS ----------
    if (!dns.isEmpty()) {
        QString dnsCombined = dns.join(",");
        emit logMessage("[WiFi] Setting DNS: " + dnsCombined);

        QString outDns = run("nmcli", {
                                          "connection", "modify", conName,
                                          "ipv4.dns", dnsCombined
                                      });
        emit logMessage("[WiFi] DNS modify result:\n" + outDns.trimmed());
    } else {
        emit logMessage("[WiFi] No DNS provided — skipping.");
    }

    // ---------- Restart the Connection ----------
    emit logMessage("[WiFi] Restarting WiFi connection...");

    QString outDown = run("nmcli", {"connection", "down", conName});
    emit logMessage("[WiFi] Connection down:\n" + outDown.trimmed());

    QString outUp = run("nmcli", {"connection", "up", conName});
    emit logMessage("[WiFi] Connection up:\n" + outUp.trimmed());

    emit logMessage("[WiFi] Static IP applied successfully.");
    emit logMessage("========== End Set Static IP ==========");

    emit ipModeChanged();
    emit processEnded();
    return true;
}

void WiFiManager::refresh()
{
    emit logMessage("=========== WiFi Refresh ===========");

    QString iface = "wlan0";
    emit logMessage("[WiFi] Checking active interface...");

    // Detect if p2p0 has the actual WiFi IP
    QString ipCheck = run("ip", {"addr", "show", "dev", "wlan0"});
    emit logMessage("[WiFi] wlan0 IP check: " + ipCheck.trimmed());

    if (!ipCheck.contains("inet ")) {
        emit logMessage("[WiFi] wlan0 has no IP, checking p2p0...");

        QString ipCheckP2P = run("ip", {"addr", "show", "dev", "p2p0"});
        emit logMessage("[WiFi] p2p0 IP check: " + ipCheckP2P.trimmed());

        if (ipCheckP2P.contains("inet ")) {
            iface = "p2p0";
            emit logMessage("[WiFi] Using p2p0 as active interface.");
        }
    } else {
        emit logMessage("[WiFi] Using wlan0 as active interface.");
    }

    // ---------- SSID, SIGNAL, SPEED ----------
    emit logMessage("[WiFi] Reading link status...");
    QString iw = run("iw", {"dev", "wlan0", "link"});
    emit logMessage("[WiFi] iw output:\n" + iw.trimmed());

    if (iw.contains("Not connected")) {
        emit logMessage("[WiFi] Status: Not connected");

        m_status = "Disconnected";
        m_ssid.clear();
        m_signalStrength = 0;
        m_linkSpeed.clear();
        m_ipAddress.clear();
        m_gateway.clear();
        m_dnsServers.clear();

        emit statusChanged();
        emit ssidChanged();
        emit signalStrengthChanged();
        emit linkSpeedChanged();
        emit ipAddressChanged();
        emit gatewayChanged();
        emit dnsServersChanged();

        emit logMessage("=========== End Refresh ===========");
        return;
    }

    emit logMessage("[WiFi] Status: Connected");
    m_status = "Connected";
    emit statusChanged();

    // SSID
    QRegularExpression reSsid("SSID: (.+)");
    auto ss = reSsid.match(iw);
    if (ss.hasMatch()) {
        m_ssid = ss.captured(1).trimmed();
        emit ssidChanged();
        emit logMessage("[WiFi] SSID: " + m_ssid);
    } else {
        emit logMessage("[WiFi] SSID not found.");
    }

    // Signal strength
    QRegularExpression reSignal("signal:\\s*(-?\\d+) dBm");
    auto s = reSignal.match(iw);
    if (s.hasMatch()) {
        m_signalStrength = s.captured(1).toInt();
        emit signalStrengthChanged();
        emit logMessage("[WiFi] Signal strength: " + QString::number(m_signalStrength) + " dBm");
    }

    // Speed (bitrate)
    QRegularExpression reSpeed("tx bitrate:\\s*([0-9.]+ .+)");
    auto sp = reSpeed.match(iw);
    if (sp.hasMatch()) {
        m_linkSpeed = sp.captured(1).trimmed();
        emit linkSpeedChanged();
        emit logMessage("[WiFi] Link speed: " + m_linkSpeed);
    }

    // ---------- SECURITY ----------
    emit logMessage("[WiFi] Checking security...");
    m_security = run("nmcli",
                      {"-t", "-f", "802-11-wireless-security.key-mgmt",
                       "connection", "show", m_ssid}).trimmed();

    emit logMessage("[WiFi] Security raw from nmcli: " + m_security);
    m_security = m_security.split(":").value(1).trimmed();

    if (m_security.isEmpty()) {
        m_security = "Open";                            // No security
    }
    else if (m_security == "none") {
        m_security = "WEP";
    }
    else if (m_security == "wpa-psk") {
        m_security = "WPA2-PSK";
    }
    else if (m_security == "sae") {
        m_security = "WPA3-SAE";
    }
    else if (m_security.contains("wpa-psk") && m_security.contains("sae")) {
        m_security = "WPA2/WPA3 Mixed";
    }
    else if (m_security == "eap") {
        m_security = "WPA2-Enterprise (EAP)";
    }

    emit securityChanged();
    emit logMessage("[WiFi] Security: " + m_security);

    // ---------- MAC ADDRESS ----------
    emit logMessage("[WiFi] Reading MAC address...");

    QString mac = run("cat", {"/sys/class/net/wlan0/address"}).trimmed();
    if (mac.isEmpty()) {
        mac = run("cat", {"/sys/class/net/p2p0/address"}).trimmed();
    }

    if (!mac.isEmpty()) {
        m_macAddress = mac.trimmed();
        emit macAddressChanged();
        emit logMessage("[WiFi] MAC: " + m_macAddress);
    }

    // ---------- IP + Subnet ----------
    emit logMessage("[WiFi] Reading IP address for " + iface + "...");
    QString ipOut = run("ip", {"addr", "show", "dev", iface});
    emit logMessage("[WiFi] IP raw output:\n" + ipOut.trimmed());

    QRegularExpression reIp("inet ([0-9.]+)/([0-9]+)");
    auto m = reIp.match(ipOut);

    if (m.hasMatch()) {
        m_ipAddress = m.captured(1);
        m_subnetMask = cidrToNetmask(m.captured(2).toInt());

        emit ipAddressChanged();
        emit subnetMaskChanged();

        emit logMessage("[WiFi] IP: " + m_ipAddress);
        emit logMessage("[WiFi] Subnet mask: " + m_subnetMask);
    }

    // ---------- GATEWAY ----------
    emit logMessage("[WiFi] Reading gateway...");
    QString route = run("ip", {"route"});
    emit logMessage("[WiFi] Route table:\n" + route.trimmed());

    QRegularExpression reGw("default via ([0-9.]+)");
    auto gw = reGw.match(route);

    if (gw.hasMatch()) {
        m_gateway = gw.captured(1);
        emit gatewayChanged();
        emit logMessage("[WiFi] Gateway: " + m_gateway);
    }

    // ---------- DNS ----------
    emit logMessage("[WiFi] Reading DNS servers...");

    QStringList dns;
    QFile f("/etc/resolv.conf");

    if (f.open(QIODevice::ReadOnly)) {
        QTextStream in(&f);

        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();

            if (line.startsWith("nameserver")) {
                QString server = line.section(" ", 1, 1).trimmed();
                if (!server.isEmpty())
                    dns << server;
            }
        }
    }

    // Default DNS fallback list
    const QStringList defaultDns = { "8.8.8.8", "1.1.1.1" };

    // Normalize results
    if (dns.isEmpty()) {
        dns = defaultDns;
        emit logMessage("[WiFi] No DNS found. Using defaults: " + dns.join(", "));
    }
    else if (dns.size() == 1) {
        dns << defaultDns[1];
        emit logMessage("[WiFi] Only one DNS found. Adding fallback: " + dns.join(", "));
    }
    else {
        // optional: limit to first 2, because NM may return many IPv6 entries
        dns = dns.mid(0, 2);
        emit logMessage("[WiFi] DNS detected: " + dns.join(", "));
    }

    // Save final DNS list
    m_dnsServers = dns;
    emit dnsServersChanged();

    emit logMessage("=========== End Refresh ===========");
}

QString WiFiManager::getCurrentConnectionName() const
{
    qInfo() << "[WiFiManager] Checking active Wi-Fi connection...";

    QString out = run("nmcli", {"-t", "-f", "NAME,TYPE", "connection", "show", "--active"});
    qDebug() << "[WiFiManager] nmcli active connection output:" << out.trimmed();

    for (const QString &line : out.split("\n")) {
        if (line.contains("802-11-wireless")) {
            QString name = line.split(":")[0];
            qInfo() << "[WiFiManager] Active Wi-Fi connection:" << name;
            return name;
        }
    }

    qWarning() << "[WiFiManager] No active Wi-Fi connection found!";
    return "";
}

void WiFiManager::updateIpMode()
{
    emit processStarted();
    qInfo() << "[WiFiManager] Updating IP mode...";

    QString conName = getCurrentConnectionName();
    if (conName.isEmpty()) {
        qWarning() << "[WiFiManager] Cannot update IP mode — no connection name.";
        return;
    }

    qDebug() << "[WiFiManager] Querying IPv4 method for connection:" << conName;

    QString mode = run("nmcli", {"-t", "-f", "ipv4.method", "connection", "show", conName}).trimmed();

    qInfo() << "[WiFiManager] ipv4.method =" << mode; //[ipv4.method:manual]

    QStringList parts = mode.split(":");

    // Safety check [ipv4.method:manual]
    if (parts.size() >= 2) {
        mode = parts[1].trimmed();
    }

    bool isStatic = (mode == "manual");
    setIsStaticIp(isStatic);

    qInfo() << "[WiFiManager] Current IP mode:"
            << (isStatic ? "Static (manual)" : "DHCP (auto)");

    emit ipModeChanged();
    emit processEnded();
    qDebug() << "[WiFiManager] ipModeChanged() signal emitted.";
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

bool WiFiManager::isStaticIp() const
{
    return m_isStaticIp;
}

void WiFiManager::setIsStaticIp(bool newIsStaticIp)
{
    if (m_isStaticIp == newIsStaticIp)
        return;
    m_isStaticIp = newIsStaticIp;
    emit isStaticIpChanged();
}

bool WiFiManager::isBusy() const
{
    return m_isBusy;
}

QString WiFiManager::activeInterface() const
{
    return m_activeInterface;
}
