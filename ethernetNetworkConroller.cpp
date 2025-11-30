#include "ethernetNetworkConroller.h"
#include <QRegularExpression>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QBitArray>

EthernetNetworkConroller::EthernetNetworkConroller(QObject *parent)
    : QObject(parent)
{
    connect(&m_proc, &QProcess::readyReadStandardOutput, this, &EthernetNetworkConroller::onProcessOutput);
    connect(&m_proc, &QProcess::errorOccurred, this, &EthernetNetworkConroller::onProcessError);

    m_pollTimer.setInterval(3000);
    connect(&m_pollTimer, &QTimer::timeout, this, &EthernetNetworkConroller::refreshStatus);
    m_pollTimer.start();
}

void EthernetNetworkConroller::setInterface(const QString &ifname) {
    if (ifname == m_interface) return;
    m_interface = ifname;
    emit interfaceChanged();
    emit logMessage(QStringLiteral("Interface set to %1").arg(ifname));
    refreshStatus();
}

QString EthernetNetworkConroller::runCommandCollect(const QString &program, const QStringList &args, int timeoutMs) {
    QProcess proc;
    proc.start(program, args);
    if (!proc.waitForFinished(timeoutMs)) {
        proc.kill();
        emit logMessage(QStringLiteral("Command timed out: %1 %2").arg(program, args.join(' ')));
        return QString();
    }
    return QString::fromLocal8Bit(proc.readAllStandardOutput()).trimmed();
}

QString cidrToNetmask(int cidrMask)
{
    if ((cidrMask < 0) || (cidrMask > 32))
        return QString(); // invalid

    quint32 mask = cidrMask == 0 ? 0 : (~0u << (32 - cidrMask));

    return QString("%1.%2.%3.%4")
        .arg((mask >> 24) & 0xFF)
        .arg((mask >> 16) & 0xFF)
        .arg((mask >> 8) & 0xFF)
        .arg(mask & 0xFF);
}

void EthernetNetworkConroller::refreshStatus()
{
    emit logMessage("─────────────── Ethernet Refresh Status ───────────────");
    emit logMessage(QString("Interface: %1").arg(m_interface));

    // ---------------------------------------------------------
    // 1. Read IP Address & Prefix
    // ---------------------------------------------------------
    QString ipOut = runCommandCollect("ip", { "-4", "addr", "show", "dev", m_interface });
    emit logMessage("Raw IP output:");
    emit logMessage(ipOut);

    QRegularExpression rxIp(R"(\binet\s+([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)/([0-9]+))");
    auto m = rxIp.match(ipOut);

    if (m.hasMatch()) {
        QString ip = m.captured(1);
        QString cidrStr = m.captured(2);
        int cidr = cidrStr.toInt();

        QString mask = cidrToNetmask(cidr);

        emit logMessage(QString("IP Address Found: %1").arg(ip));
        emit logMessage(QString("CIDR: %1 → Subnet Mask: %2").arg(cidr).arg(mask));

        setIpAddress(ip);
        setSubnetMask(mask);
    } else {
        emit logMessage("❗ IP address not found!");
    }

    // ---------------------------------------------------------
    // 2. Read Gateway
    // ---------------------------------------------------------
    emit logMessage("---------------------------------------------------------");
    QString routeOut = runCommandCollect("ip", {
                                                   "route", "show", "default", "dev", m_interface
                                               });

    emit logMessage("Raw Route output:");
    emit logMessage(routeOut);

    QRegularExpression rxGw(R"(default via ([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+))");
    auto gwMatch = rxGw.match(routeOut);

    if (gwMatch.hasMatch()) {
        QString gw = gwMatch.captured(1);
        emit logMessage(QString("Gateway Found: %1").arg(gw));
        setGateway(gw);
    } else {
        emit logMessage("❗ Gateway not found!");
    }

    // ---------------------------------------------------------
    // 3. Read DNS Servers
    // ---------------------------------------------------------
    emit logMessage("---------------------------------------------------------");
    emit logMessage("Reading /etc/resolv.conf:");

    QStringList dnsRecordsList;
    QFile file("/etc/resolv.conf");

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {

        QTextStream in(&file);
        QRegularExpression re(
            "^\\s*nameserver\\s+([^#\\s]+)",
            QRegularExpression::CaseInsensitiveOption
            );

        while (!in.atEnd()) {
            QString line = in.readLine();
            emit logMessage("resolv.conf line: " + line);

            auto match = re.match(line);
            if (match.hasMatch()) {
                dnsRecordsList << match.captured(1);
            }
        }

        file.close();
    }

    if (!dnsRecordsList.isEmpty()) {
        emit logMessage("DNS Servers Found:");
        for (int i = 0; i < dnsRecordsList.size(); i++)
            emit logMessage(QString("  DNS%1: %2").arg(i + 1).arg(dnsRecordsList[i]));
    } else {
        emit logMessage("❗ No DNS servers found!");
    }

    setDnsRecords(dnsRecordsList);

    // ---------------------------------------------------------
    // 4. Read Link Speed
    // ---------------------------------------------------------
    emit logMessage("---------------------------------------------------------");
    QString speed = runCommandCollect("cat", {
        QString("/sys/class/net/%1/speed").arg(m_interface)
    });

    if (!speed.isEmpty()) {
        m_speed = speed.trimmed() + " Mb/s";
        emit linkSpeedChanged();
        emit logMessage(QString("Link Speed: %1").arg(m_speed));
    } else {
        emit logMessage("❗ Link speed unavailable");
    }

    // ---------------------------------------------------------
    // 5. Read Link Status
    // ---------------------------------------------------------
    QString linkState = runCommandCollect("cat", {
        QString("/sys/class/net/%1/operstate").arg(m_interface)
    });

    QString newStatus = linkState.trimmed().isEmpty()
                            ? "unknown"
                            : linkState.trimmed();

    if (newStatus != m_status) {
        m_status = newStatus;
        emit statusChanged();
    }

    emit logMessage(QString("Link Status: %1").arg(m_status));

    // ---------------------------------------------------------
    // 6. Read MAC Address
    // ---------------------------------------------------------
    QString mac = runCommandCollect("cat", {
        QString("/sys/class/net/%1/address").arg(m_interface)
    });

    if (!mac.isEmpty()) {
        m_mac = mac.trimmed();
        emit macAddressChanged();
        emit logMessage(QString("MAC Address: %1").arg(m_mac));
    } else {
        emit logMessage("❗ MAC address not found");
    }

    emit logMessage("──────────────── End Refresh Status ────────────────");
}

void EthernetNetworkConroller::startDhcp() {
    emit logMessage(QStringLiteral("Starting DHCP client on %1").arg(m_interface));
    if (m_dhcpRunning) {
        emit logMessage("DHCP already running");
        return;
    }

    QStringList args = {"-i", m_interface, "-b", "-p", QString("/var/run/udhcpc.%1.pid").arg(m_interface), "-s", "/usr/share/udhcpc/default.script"};
    m_proc.start("udhcpc", args);
    if (!m_proc.waitForStarted(2000)) {
        emit logMessage(QStringLiteral("Failed to start udhcpc"));
        return;
    }
    m_dhcpRunning = true;
    emit dhcpRunningChanged();
    emit logMessage("DHCP client started");
    refreshStatus();
}

void EthernetNetworkConroller::enableDhcp()
{
    emit logMessage("Switching interface to DHCP mode");

    // 0. Stop any running DHCP
    stopDhcp();

    // 1. Remove static IP configuration
    runCommandCollect("ip", {"addr", "flush", "dev", m_interface});
    runCommandCollect("ip", {"route", "flush", "dev", m_interface});

    // 2. Bring interface UP
    runCommandCollect("ip", {"link", "set", m_interface, "up"});

    // 3. Start DHCP client
    startDhcp();

    // 4. Update UI
    refreshStatus();
}

void EthernetNetworkConroller::stopDhcp() {
    emit logMessage(QStringLiteral("Stopping DHCP client on %1").arg(m_interface));
    if (!m_dhcpRunning) {
        // Attempt to release/kill anyway
        QString pidfile = QString("/var/run/udhcpc.%1.pid").arg(m_interface);
        QString pid = runCommandCollect("sh", {"-c", QString("cat %1 2>/dev/null || true").arg(pidfile)});
        if (!pid.isEmpty()) {
            runCommandCollect("kill", {pid});
            runCommandCollect("rm", {pidfile});
            emit logMessage(QStringLiteral("Sent kill to pid %1").arg(pid));
        }
        return;
    }

    // Try graceful release
    runCommandCollect("udhcpc", {"-i", m_interface, "-x", "release"});
    QString pidfile = QString("/var/run/udhcpc.%1.pid").arg(m_interface);
    QString pid = runCommandCollect("sh", {"-c", QString("cat %1 2>/dev/null || true").arg(pidfile)});
    if (!pid.isEmpty()) {
        runCommandCollect("kill", {pid});
        runCommandCollect("rm", {pidfile});
        emit logMessage(QStringLiteral("Sent kill to pid %1").arg(pid));
    }
    m_dhcpRunning = false;
    emit dhcpRunningChanged();
    refreshStatus();
}

void EthernetNetworkConroller::connectClicked() {
    startDhcp();
}

void EthernetNetworkConroller::disconnectClicked() {
    stopDhcp();
}

QString EthernetNetworkConroller::getNmConnectionForInterface(const QString &ifName)
{
    // nmcli -t -f NAME,DEVICE connection show --active
    const QString output = runCommandCollect(
        QStringLiteral("nmcli"),
        { "-t", "-f", "NAME,DEVICE", "connection", "show", "--active" }
        );

    if (output.isEmpty()) {
        emit logMessage(QStringLiteral("[Ethernet] No active NetworkManager connections found."));
        return {};
    }

    const QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    for (const QString &line : lines) {
        const QStringList parts = line.split(':');
        if (parts.size() < 2)
            continue;

        const QString name   = parts[0];
        const QString device = parts[1];

        if (device == ifName) {
            emit logMessage(QStringLiteral("[Ethernet] Using NM connection \"%1\" for interface %2.")
                                .arg(name, ifName));
            return name;
        }
    }

    emit logMessage(QStringLiteral("[Ethernet] No NM connection bound to interface %1.").arg(ifName));
    return {};
}

void EthernetNetworkConroller::applyStaticConfig(
    const QString &ip,
    int cidrMask,
    const QString &gateway,
    const QString &dns1,
    const QString &dns2)
{
    emit logMessage(QStringLiteral(
                        "[Ethernet] Applying static IPv4 configuration %1/%2, gateway %3 on %4")
                        .arg(ip)
                        .arg(cidrMask)
                        .arg(gateway)
                        .arg(m_interface));

    // 1. Stop any external DHCP client (if you have one running outside NM)
    stopDhcp();

    // 2. Determine the NetworkManager connection name for this interface
    const QString connName = getNmConnectionForInterface(m_interface);
    if (connName.isEmpty()) {
        emit logMessage(QStringLiteral(
                            "[Ethernet] Cannot apply static config: no NetworkManager connection for %1.")
                            .arg(m_interface));
        return;
    }

    // 3. Build CIDR and DNS string
    const QString cidr = QStringLiteral("%1/%2").arg(ip).arg(cidrMask);
    QStringList dnsList;
    if (!dns1.trimmed().isEmpty())
        dnsList << dns1.trimmed();
    if (!dns2.trimmed().isEmpty())
        dnsList << dns2.trimmed();
    const QString dnsStr = dnsList.join(' ');

    // 4. Configure IPv4 method/manual + address + gateway
    emit logMessage(QStringLiteral(
                        "[Ethernet] Configuring NM connection \"%1\" with %2, gateway %3.")
                        .arg(connName, cidr, gateway));

    runCommandCollect(QStringLiteral("nmcli"),
                      { "connection", "modify", connName,
                       "ipv4.method", "manual",
                       "ipv4.addresses", cidr,
                       "ipv4.gateway", gateway });

    // 5. Configure DNS via NetworkManager (persistent)
    if (!dnsStr.isEmpty()) {
        emit logMessage(QStringLiteral(
                            "[Ethernet] Setting DNS for \"%1\" to: %2")
                            .arg(connName, dnsStr));
        runCommandCollect(QStringLiteral("nmcli"),
                          { "connection", "modify", connName,
                           "ipv4.dns", dnsStr,
                           "ipv4.ignore-auto-dns", "yes" });
    } else {
        emit logMessage(QStringLiteral(
                            "[Ethernet] No DNS provided. Clearing manual DNS for \"%1\".")
                            .arg(connName));
        runCommandCollect(QStringLiteral("nmcli"),
                          { "connection", "modify", connName,
                           "ipv4.dns", "",
                           "ipv4.ignore-auto-dns", "no" });
    }

    // 6. Make sure Ethernet prefers to auto-connect over Wi-Fi
    emit logMessage(QStringLiteral(
                        "[Ethernet] Enabling autoconnect and setting high priority for \"%1\".")
                        .arg(connName));

    runCommandCollect(QStringLiteral("nmcli"),
                      { "connection", "modify", connName,
                       "connection.autoconnect", "yes",
                       "connection.autoconnect-priority", "999" });

    // (You can set Wi-Fi connection.autoconnect-priority to a lower value
    // in your Wi-Fi controller code, e.g. -999, as we discussed.)

    // 7. Bring the connection down and up to apply changes immediately
    emit logMessage(QStringLiteral("[Ethernet] Restarting NM connection \"%1\" to apply changes.")
                        .arg(connName));

    runCommandCollect(QStringLiteral("nmcli"),
                      { "connection", "down", connName });
    runCommandCollect(QStringLiteral("nmcli"),
                      { "connection", "up", connName });

    // 8. Refresh cached status / UI
    refreshStatus();

    emit logMessage(QStringLiteral(
                        "[Ethernet] Static IP configuration applied and persisted for \"%1\".")
                        .arg(connName));
}

// void EthernetNetworkConroller::applyStaticConfig(
//     const QString &ip,
//     int cidrMask,
//     const QString &gateway,
//     const QString &dns1,
//     const QString &dns2) {

//     emit logMessage(QStringLiteral("Applying static IP: %1/%2").arg(ip).arg(cidrMask));

//     // 1. Stop DHCP client
//     stopDhcp();

//     // 2. Remove any existing IPs
//     runCommandCollect("ip", {"addr", "flush", "dev", m_interface});

//     // 3. Apply new static IP
//     QString cidr = QString("%1/%2").arg(ip).arg(QString::number(cidrMask));
//     runCommandCollect("ip", {"addr", "add", cidr, "dev", m_interface});

//     // 4. Ensure interface is up
//     runCommandCollect("ip", {"link", "set", m_interface, "up"});

//     // 5. Set default route
//     runCommandCollect("ip", {"route", "flush", "dev", m_interface});
//     runCommandCollect("ip", {"route", "add", "default", "via", gateway, "dev", m_interface});

//     // 6. Rewrite resolv.conf with both DNS servers
//     QFile file("/etc/resolv.conf");
//     if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
//         QTextStream out(&file);

//         if (!dns1.trimmed().isEmpty())
//             out << "nameserver " << dns1.trimmed() << "\n";

//         if (!dns2.trimmed().isEmpty())
//             out << "nameserver " << dns2.trimmed() << "\n";

//         file.close();
//     } else {
//         emit logMessage("Failed to open /etc/resolv.conf for writing");
//     }

//     // 7. Refresh network status
//     refreshStatus();
// }

int EthernetNetworkConroller::maskToCidr(const QString &mask)
{
    QStringList parts = mask.split(".");
    if (parts.size() != 4)
        return -1;

    int cidr = 0;

    for (const QString &p : parts) {
        bool ok = false;
        int octet = p.toInt(&ok);
        if ((!ok) || (octet < 0) || (octet > 255)) {
            return -1;
        }

        // Count bits in each octet
        cidr += QBitArray::fromBits((const char *)&octet, 8).count(true);
    }
    return cidr;
}

void EthernetNetworkConroller::onProcessOutput() {
    QByteArray out = m_proc.readAllStandardOutput();
    if (!out.isEmpty())
        emit logMessage(QString::fromLocal8Bit(out));
}

void EthernetNetworkConroller::onProcessError(QProcess::ProcessError err) {
    Q_UNUSED(err);
    emit logMessage(QStringLiteral("Process error: %1").arg(m_proc.errorString()));
}

QString EthernetNetworkConroller::subnetMask() const {
    return m_subnetMask;
}

void EthernetNetworkConroller::setSubnetMask(const QString &newSubnetMask) {
    if (m_subnetMask == newSubnetMask)
        return;
    m_subnetMask = newSubnetMask;
    emit subnetMaskChanged();
}

QString EthernetNetworkConroller::gateway() const {
    return m_gateway;
}

void EthernetNetworkConroller::setGateway(const QString &newGateway) {
    if (m_gateway == newGateway)
        return;
    m_gateway = newGateway;
    emit gatewayChanged();
}

QStringList EthernetNetworkConroller::dnsRecords() const
{
    return m_dnsRecords;
}

void EthernetNetworkConroller::setDnsRecords(const QStringList &newDnsRecords)
{
    if (m_dnsRecords == newDnsRecords)
        return;
    m_dnsRecords = newDnsRecords;
    emit dnsRecordsChanged();
}

void EthernetNetworkConroller::setIpAddress(const QString &newIpAddress)
{
    if (m_ipAddress == newIpAddress)
        return;
    m_ipAddress = newIpAddress;
    emit ipAddressChanged();
}
