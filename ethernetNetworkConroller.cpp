#include "ethernetNetworkConroller.h"
#include <QRegularExpression>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QBitArray>
#include <QtConcurrent/QtConcurrent>

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

bool EthernetNetworkConroller::runCommandBool(const QString &program,
                                              const QStringList &args,
                                              int timeoutMs)
{
    QProcess proc;
    proc.start(program, args);

    if (!proc.waitForStarted()) {
        emit logMessage(QStringLiteral("Failed to start command: %1 %2")
                            .arg(program, args.join(' ')));
        return false;
    }

    if (!proc.waitForFinished(timeoutMs)) {
        proc.kill();
        emit logMessage(QStringLiteral("Command timed out: %1 %2")
                            .arg(program, args.join(' ')));
        return false;
    }

    int exitCode = proc.exitCode();
    QString stdoutOut = QString::fromLocal8Bit(proc.readAllStandardOutput()).trimmed();
    QString stderrOut = QString::fromLocal8Bit(proc.readAllStandardError()).trimmed();

    emit logMessage(QStringLiteral("[CMD] %1 %2").arg(program, args.join(' ')));
    if (!stdoutOut.isEmpty())
        emit logMessage(QStringLiteral("[OUT] %1").arg(stdoutOut));
    if (!stderrOut.isEmpty())
        emit logMessage(QStringLiteral("[ERR] %1").arg(stderrOut));

    if (exitCode != 0) {
        emit logMessage(QStringLiteral("Command failed with exit code %1").arg(exitCode));
        return false;
    }

    return true;
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

bool EthernetNetworkConroller::startDhcp()
{
    emit logMessage(QStringLiteral("Starting DHCP client on %1").arg(m_interface));

    // If already running, nothing to do
    if (m_dhcpRunning) {
        emit logMessage("[DHCP] Already running, skipping start.");
        return true;
    }

    // Build udhcpc arguments
    QString pidfile = QString("/var/run/udhcpc.%1.pid").arg(m_interface);

    QStringList args = {
        "-i", m_interface,
        "-b",
        "-p", pidfile,
        "-s", "/usr/share/udhcpc/default.script"
    };

    // Start DHCP client
    m_proc.start("udhcpc", args);

    if (!m_proc.waitForStarted(2000)) {
        emit logMessage(QStringLiteral("[DHCP] ERROR: Failed to start udhcpc"));
        return false;
    }

    // Success
    m_dhcpRunning = true;
    emit dhcpRunningChanged();
    emit logMessage("[DHCP] Client started successfully.");

    refreshStatus();
    return true;
}

void EthernetNetworkConroller::enableDhcpAsync()
{
    emit operationStarted();
    setBusy(true);

    QtConcurrent::run([=]() {

        bool success = enableDhcpWorker();
        QString message = success
                              ? "DHCP mode enabled successfully."
                              : "Failed to enable DHCP mode.";

        // Deliver result back to UI thread
        QMetaObject::invokeMethod(this, [this, success, message]() {
            setBusy(false);
            emit operationFinished(success, message);
        });
    });
}

bool EthernetNetworkConroller::enableDhcpWorker()
{
    emit logMessage("[Ethernet] Switching interface to DHCP mode");

    bool ok = true;

    // 0. Stop any running DHCP client
    ok &= stopDhcp();
    if (!ok) return false;

    // 1. Flush static IP + routes
    ok &= runCommandBool("ip", {"addr", "flush", "dev", m_interface});
    if (!ok) return false;

    ok &= runCommandBool("ip", {"route", "flush", "dev", m_interface});
    if (!ok) return false;

    // 2. Bring link UP
    ok &= runCommandBool("ip", {"link", "set", m_interface, "up"});
    if (!ok) return false;

    // 3. Start DHCP client
    ok &= startDhcp();
    if (!ok) return false;

    // 4. Refresh UI data
    refreshStatus();

    emit logMessage("[Ethernet] DHCP mode enabled successfully");

    return true;
}

bool EthernetNetworkConroller::stopDhcp()
{
    emit logMessage(QStringLiteral("Stopping DHCP client on %1").arg(m_interface));
    bool ok = true;

    const QString pidfile = QString("/var/run/udhcpc.%1.pid").arg(m_interface);

    auto readPid = [&]() -> QString {
        return runCommandCollect("sh", {"-c", QString("cat %1 2>/dev/null || true").arg(pidfile)});
    };

    // If DHCP was NOT marked running, still try to kill stale client
    if (!m_dhcpRunning) {
        QString pid = readPid();
        if (!pid.isEmpty()) {
            ok &= runCommandBool("kill", { pid });
            ok &= runCommandBool("rm",   { pidfile });

            emit logMessage(QStringLiteral("Killed stale DHCP client (pid %1)").arg(pid));
        }
        return ok;
    }

    // DHCP running → try graceful release
    ok &= runCommandBool("udhcpc", { "-i", m_interface, "-x", "release" });

    // Read PID again after release
    QString pid = readPid();
    if (!pid.isEmpty()) {
        ok &= runCommandBool("kill", { pid });
        ok &= runCommandBool("rm",   { pidfile });

        emit logMessage(QStringLiteral("Sent kill to DHCP pid %1").arg(pid));
    }

    // Update state
    m_dhcpRunning = false;
    emit dhcpRunningChanged();
    refreshStatus();

    return ok;
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

bool EthernetNetworkConroller::applyStaticConfigWorker(
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

    // 1. Stop external DHCP client
    stopDhcp();

    // 2. Find NM connection
    const QString connName = getNmConnectionForInterface(m_interface);
    if (connName.isEmpty()) {
        emit logMessage(QStringLiteral(
                            "[Ethernet] Cannot apply static config: no NetworkManager connection for %1.")
                            .arg(m_interface));
        return false;
    }

    // 3. Prepare CIDR + DNS
    const QString cidr = QStringLiteral("%1/%2").arg(ip).arg(cidrMask);

    QStringList dnsList;
    if (!dns1.trimmed().isEmpty()) dnsList << dns1.trimmed();
    if (!dns2.trimmed().isEmpty()) dnsList << dns2.trimmed();
    const QString dnsStr = dnsList.join(' ');

    bool ok = true; // track cumulative result

    // 4. Set IP + gateway
    emit logMessage(QStringLiteral(
                        "[Ethernet] Configuring NM connection \"%1\" with %2, gateway %3.")
                        .arg(connName, cidr, gateway));

    ok &= runCommandBool("nmcli",
                         { "connection", "modify", connName,
                          "ipv4.method", "manual",
                          "ipv4.addresses", cidr,
                          "ipv4.gateway", gateway });

    if (!ok) return false;

    // 5. DNS configuration
    if (!dnsStr.isEmpty()) {
        emit logMessage(QStringLiteral(
                            "[Ethernet] Setting DNS for \"%1\" to: %2")
                            .arg(connName, dnsStr));

        ok &= runCommandBool("nmcli",
                             { "connection", "modify", connName,
                              "ipv4.dns", dnsStr,
                              "ipv4.ignore-auto-dns", "yes" });
    } else {
        emit logMessage(QStringLiteral(
                            "[Ethernet] No DNS provided. Clearing manual DNS for \"%1\".")
                            .arg(connName));

        ok &= runCommandBool("nmcli",
                             { "connection", "modify", connName,
                              "ipv4.dns", "",
                              "ipv4.ignore-auto-dns", "no" });
    }

    if (!ok) return false;

    // 6. Autoconnect priority
    emit logMessage(QStringLiteral(
                        "[Ethernet] Enabling autoconnect and setting high priority for \"%1\".")
                        .arg(connName));

    ok &= runCommandBool("nmcli",
                         { "connection", "modify", connName,
                          "connection.autoconnect", "yes",
                          "connection.autoconnect-priority", "999" });

    if (!ok) return false;

    // 7. Restart NM connection
    emit logMessage(QStringLiteral("[Ethernet] Restarting NM connection \"%1\" to apply changes.")
                        .arg(connName));

    ok &= runCommandBool("nmcli", { "connection", "down", connName });
    if (!ok) return false;

    ok &= runCommandBool("nmcli", { "connection", "up", connName });
    if (!ok) return false;

    // 8. Refresh UI
    refreshStatus();

    emit logMessage(QStringLiteral(
                        "[Ethernet] Static IP configuration applied and persisted for \"%1\".")
                        .arg(connName));

    return true;
}

void EthernetNetworkConroller::applyStaticConfigAsync(
    const QString &ip,
    int cidrMask,
    const QString &gateway,
    const QString &dns1,
    const QString &dns2)
{
    emit operationStarted();
    setBusy(true);

    QtConcurrent::run([=]() {

        bool success = applyStaticConfigWorker(ip, cidrMask, gateway, dns1, dns2);

        QString message = success
                              ? "Static IP configuration applied successfully."
                              : "Failed to apply static IP configuration.";

        // Emit from background → must use queued connection
        QMetaObject::invokeMethod(this, [this, success, message]() {
            setBusy(false);
            emit operationFinished(success, message);
        });
    });
}


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

bool EthernetNetworkConroller::isBusy() const
{
    return m_isBusy;
}
