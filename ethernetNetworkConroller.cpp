#include "ethernetNetworkConroller.h"
#include <QRegularExpression>
#include <QFile>
#include <QTextStream>
#include <QDebug>

EthernetNetworkConroller::EthernetNetworkConroller(QObject *parent)
    : QObject(parent)
{
    connect(&m_proc, &QProcess::readyReadStandardOutput, this, &EthernetNetworkConroller::onProcessOutput);
    connect(&m_proc, &QProcess::errorOccurred, this, &EthernetNetworkConroller::onProcessError);

    //fetchDns();
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

void EthernetNetworkConroller::refreshStatus() {
    // --- IP + Subnet ---
    QString ipOut = runCommandCollect("ip", {"-4", "addr", "show", "dev", m_interface});
    QRegularExpression rxIp(R"(\binet\s+([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)/([0-9]+))");
    auto m = rxIp.match(ipOut);
    if (m.hasMatch()) {
        QString ip = m.captured(1);
        QString cidrMask = m.captured(2);

        if (ip != m_ip) {
            m_ip = ip;
            emit ipAddressChanged();
        }
        if (cidrMask != m_subnetMask) {
            m_subnetMask = cidrMask;
            emit subnetMaskChanged();
        }
    }

    // --- Gateway ---
    QString routeOut = runCommandCollect("ip", {"route", "show", "default", "dev", m_interface});
    QRegularExpression rxGw(R"(default via ([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+))");
    auto gwMatch = rxGw.match(routeOut);
    if (gwMatch.hasMatch()) {
        QString gw = gwMatch.captured(1);
        if (gw != m_gateway) {
            m_gateway = gw;
            emit gatewayChanged();
        }
    }

    // --- DNS ---
    QStringList dnsList;
    QFile file("/etc/resolv.conf");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QRegularExpression re("^\\s*nameserver\\s+([^#\\s]+)", QRegularExpression::CaseInsensitiveOption);
        while (!in.atEnd()) {
            QString line = in.readLine();
            auto match = re.match(line);
            if (match.hasMatch())
                dnsList << match.captured(1);
        }
    }
    if (!dnsList.isEmpty() && dnsList[0] != m_dns) {
        m_dns = dnsList[0];
        emit dnsChanged();
    }

    // Read speed from sysfs
    QString speed = runCommandCollect("cat", {QString("/sys/class/net/%1/speed").arg(m_interface)});
    if (!speed.isEmpty()) {
        m_speed = speed + "Mb/s";
        emit linkSpeedChanged();
    }

    // Read operstate from sysfs
    QString linkState = runCommandCollect("cat", {QString("/sys/class/net/%1/operstate").arg(m_interface)});
    QString newStatus = linkState.isEmpty() ? "unknown" : linkState.trimmed();
    if (newStatus != m_status) {
        m_status = newStatus;
        emit statusChanged();
    }

    // Read MAC address if needed
    QString mac = runCommandCollect("cat", {QString("/sys/class/net/%1/address").arg(m_interface)});
    if (!mac.isEmpty()) {
        m_mac = mac;
        emit macAddressChanged(); // add a new signal & property if you want to show it
    }
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

void EthernetNetworkConroller::setManualConfig(const QString &ip, int cidrMask, const QString &gateway, const QString &dns) {
    emit logMessage(QStringLiteral("Applying manual config %1/%2").arg(ip).arg(cidrMask));
    stopDhcp();

    runCommandCollect("ip", {"addr", "flush", "dev", m_interface});
    QString cidr = QString("%1/%2").arg(ip).arg(QString::number(cidrMask));
    runCommandCollect("ip", {"addr", "add", cidr, "dev", m_interface});
    runCommandCollect("ip", {"link", "set", m_interface, "up"});
    runCommandCollect("ip", {"route", "add", "default", "via", gateway, "dev", m_interface});

    QFile file("/etc/resolv.conf");
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QTextStream out(&file);
        out << "nameserver " << dns << "\\n";
        file.close();
    }

    refreshStatus();
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

QString EthernetNetworkConroller::dns() const {
    return m_dns;
}

void EthernetNetworkConroller::setDns(const QString &newDns) {
    if (m_dns == newDns)
        return;
    m_dns = newDns;
    emit dnsChanged();
}

// void EthernetNetworkConroller::fetchDns() {
//     QStringList ips;
//     QFile file("/etc/resolv.conf");
//     if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//         qWarning() << "Failed to open /etc/resolv.conf:" << file.errorString();
//         return;
//     }

//     QTextStream in(&file);

//     // Matches: nameserver <ip/host>, case-insensitive, ignores leading spaces
//     QRegularExpression re("^\\s*nameserver\\s+([^#\\s]+)", QRegularExpression::CaseInsensitiveOption);

//     while (!in.atEnd()) {
//         const QString line = in.readLine();
//         QRegularExpressionMatch m = re.match(line);
//         if (m.hasMatch()) {
//             QString value = m.captured(1);
//             ips << value; // value can be IPv4, IPv6, or a hostname
//         }
//     }
//     for(auto v:ips){
//         qDebug()<<"::::>v="<<v;
//     }
//     setDns(ips[0]);
// }

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
