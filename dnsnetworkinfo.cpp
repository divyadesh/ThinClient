#include "dnsnetworkinfo.h"
#include <QProcess>
#include <QRegularExpression>
#include <QDebug>

DNSNetworkInfo::DNSNetworkInfo( QObject *parent, const QString &iface)
    : QObject(parent), m_iface(iface)
{
    updateInfo(); // first fetch
}

DNSNetworkInfo::NetData DNSNetworkInfo::collectInfo()
{
    NetData data;

    auto runCommand = [](const QString &cmd) -> QString {
        QProcess p;
        p.start("bash", QStringList() << "-c" << cmd);
        p.waitForFinished(2000);
        return QString::fromUtf8(p.readAllStandardOutput()).trimmed();
    };

    // --- Get IP and Netmask ---
    QString ipOutput = runCommand(QString("ip addr show dev %1").arg(m_iface));
    QRegularExpression reIp("inet (\\d+\\.\\d+\\.\\d+\\.\\d+)/(\\d+)");
    auto match = reIp.match(ipOutput);
    if (match.hasMatch()) {
        data.ip = match.captured(1);
        int prefix = match.captured(2).toInt();
        quint32 mask = (0xFFFFFFFF << (32 - prefix)) & 0xFFFFFFFF;
        data.mask = QString("%1.%2.%3.%4")
                        .arg((mask >> 24) & 0xFF)
                        .arg((mask >> 16) & 0xFF)
                        .arg((mask >> 8) & 0xFF)
                        .arg(mask & 0xFF);
    }

    // --- Get Broadcast ---
    QRegularExpression reBc("brd (\\d+\\.\\d+\\.\\d+\\.\\d+)");
    auto matchBc = reBc.match(ipOutput);
    if (matchBc.hasMatch())
        data.broadcast = matchBc.captured(1);

    // --- Get Gateway ---
    QString gwOutput = runCommand("ip route | grep default");
    QRegularExpression reGw("default via (\\d+\\.\\d+\\.\\d+\\.\\d+)");
    auto matchGw = reGw.match(gwOutput);
    if (matchGw.hasMatch())
        data.gateway = matchGw.captured(1);

    // --- Get DNS ---10.42
    // --- Get DNS ---
    QString dnsOutput = runCommand("systemd-resolve --status");
    if (dnsOutput.isEmpty()) {
        // fallback if systemd-resolve no10t available
        dnsOutput = runCommand("cat /etc/resolv.conf | grep nameserver");
    }

    QRegularExpression reDns("(\\d+\\.\\d+\\.\\d+\\.\\d+)");
    QSet<QString> dnsSet;
    auto it = reDns.globalMatch(dnsOutput);
    while (it.hasNext())
        dnsSet.insert(it.next().captured(1));

    data.dns = dnsSet.values(); // remove duplicates

    return data;
}

void DNSNetworkInfo::updateInfo()
{
    auto future = QtConcurrent::run([this]() {
        return collectInfo();
    });

    auto *watcher = new QFutureWatcher<NetData>(this);
    connect(watcher, &QFutureWatcher<NetData>::finished, this, [this, watcher]() {
        NetData data = watcher->result();
        watcher->deleteLater();

        bool changed = false;
        if (m_ipAddress != data.ip) { m_ipAddress = data.ip; changed = true; }
        if (m_netmask != data.mask) { m_netmask = data.mask; changed = true; }
        if (m_gateway != data.gateway) { m_gateway = data.gateway; changed = true; }
        if (m_broadcast != data.broadcast) { m_broadcast = data.broadcast; changed = true; }
        if (m_dnsServers != data.dns) { m_dnsServers = data.dns; changed = true; }

        if (changed) emit infoChanged();
    });

    watcher->setFuture(future);
}
