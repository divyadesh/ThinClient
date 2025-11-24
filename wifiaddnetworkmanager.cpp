#include "wifiaddnetworkmanager.h"
#include <QProcess>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include <QMetaObject>
#include <QPointer>

// ======================= ctor =======================

WiFiAddNetworkManager::WiFiAddNetworkManager(QObject *parent)
    : QObject(parent)
{
    // sensible defaults
    m_securityType = 0;   // securityOpen
    m_proxyMode    = 0;   // proxyNone
    m_ipMode       = 0;   // ipDHCP
    m_macMode      = 0;   // macDevice
    m_meteredMode  = 0;   // meteredAuto
    m_hiddenMode   = 0;   // hiddenNo

    validate();
}

// ======================= logging & state =======================

void WiFiAddNetworkManager::asyncLog(const QString &msg)
{
    // always deliver logMessage on main thread
    QMetaObject::invokeMethod(this, [this, msg]() {
        emit logMessage(msg);
    }, Qt::QueuedConnection);
}

void WiFiAddNetworkManager::setSaveEnabled(bool v)
{
    if (m_saveEnabled == v)
        return;
    m_saveEnabled = v;
    emit saveEnabledChanged();
}

void WiFiAddNetworkManager::setBusy(bool v)
{
    if (m_busy == v)
        return;
    m_busy = v;
    emit busyChanged();
}

// ======================= setters =======================

void WiFiAddNetworkManager::setSsid(const QString &v)
{
    if (m_ssid == v)
        return;
    m_ssid = v;
    emit ssidChanged();
    validate();
}

void WiFiAddNetworkManager::setSecurityType(int v)
{
    if (m_securityType == v)
        return;
    m_securityType = v;
    emit securityTypeChanged();
    validate();
}

void WiFiAddNetworkManager::setPassword(const QString &v)
{
    if (m_password == v)
        return;
    m_password = v;
    emit passwordChanged();
    validate();
}

void WiFiAddNetworkManager::setProxyMode(int v)
{
    if (m_proxyMode == v)
        return;
    m_proxyMode = v;
    emit proxyModeChanged();
    validate();
}

void WiFiAddNetworkManager::setHttpProxy(const QString &v)
{
    if (m_httpProxy == v)
        return;
    m_httpProxy = v;
    emit httpProxyChanged();
    validate();
}

void WiFiAddNetworkManager::setHttpsProxy(const QString &v)
{
    if (m_httpsProxy == v)
        return;
    m_httpsProxy = v;
    emit httpsProxyChanged();
    validate();
}

void WiFiAddNetworkManager::setSocksProxy(const QString &v)
{
    if (m_socksProxy == v)
        return;
    m_socksProxy = v;
    emit socksProxyChanged();
    validate();
}

void WiFiAddNetworkManager::setIgnoreHosts(const QString &v)
{
    if (m_ignoreHosts == v)
        return;
    m_ignoreHosts = v;
    emit ignoreHostsChanged();
    validate();
}

void WiFiAddNetworkManager::setIpMode(int v)
{
    if (m_ipMode == v)
        return;
    m_ipMode = v;
    emit ipModeChanged();
    validate();
}

void WiFiAddNetworkManager::setIpAddress(const QString &v)
{
    if (m_ipAddress == v)
        return;
    m_ipAddress = v;
    emit ipAddressChanged();
    validate();
}

void WiFiAddNetworkManager::setGateway(const QString &v)
{
    if (m_gateway == v)
        return;
    m_gateway = v;
    emit gatewayChanged();
    validate();
}

void WiFiAddNetworkManager::setPrefixLength(const QString &v)
{
    if (m_prefixLength == v)
        return;
    m_prefixLength = v;
    emit prefixLengthChanged();
    validate();
}

void WiFiAddNetworkManager::setDns1(const QString &v)
{
    if (m_dns1 == v)
        return;
    m_dns1 = v;
    emit dns1Changed();
    validate();
}

void WiFiAddNetworkManager::setDns2(const QString &v)
{
    if (m_dns2 == v)
        return;
    m_dns2 = v;
    emit dns2Changed();
    validate();
}

void WiFiAddNetworkManager::setMacMode(int v)
{
    if (m_macMode == v)
        return;
    m_macMode = v;
    emit macModeChanged();
    validate();
}

void WiFiAddNetworkManager::setMeteredMode(int v)
{
    if (m_meteredMode == v)
        return;
    m_meteredMode = v;
    emit meteredModeChanged();
    validate();
}

void WiFiAddNetworkManager::setHiddenMode(int v)
{
    if (m_hiddenMode == v)
        return;
    m_hiddenMode = v;
    emit hiddenModeChanged();
    validate();
}

void WiFiAddNetworkManager::setPacUrl(const QString &v)
{
    if (m_pacUrl == v)
        return;
    m_pacUrl = v;
    emit pacUrlChanged();
    validate();
}

// ======================= validation =======================

void WiFiAddNetworkManager::validate()
{
    bool ok = true;

    // SSID required
    if (m_ssid.trimmed().isEmpty())
        ok = false;

    // Security requirements (based on AppEnums values)
    // 0=open, 1=WEP, 2=WPA, 3=WPA2, 4=WPA/WPA2, 5=WPA3, 6=WPA2/WPA3, 7=EAP, 8=EAP3
    if (m_securityType != 0) {
        // all non-open types require password for now (EAP not supported yet)
        if (m_password.trimmed().isEmpty())
            ok = false;
    }

    // Static IP requires IP + gateway + prefix
    // ipDHCP=0, ipStatic=1
    if (m_ipMode == 1) {
        if (m_ipAddress.trimmed().isEmpty())
            ok = false;
        if (m_gateway.trimmed().isEmpty())
            ok = false;
        if (m_prefixLength.trimmed().isEmpty())
            ok = false;
    }

    // Proxy Manual: require at least one proxy address
    // proxyNone=0, proxyManual=1, proxyAuto=2
    if (m_proxyMode == 1) {
        if (m_httpProxy.trimmed().isEmpty() &&
            m_httpsProxy.trimmed().isEmpty() &&
            m_socksProxy.trimmed().isEmpty()) {
            ok = false;
        }
    }

    // Proxy Auto: require PAC URL
    if (m_proxyMode == 2) {
        if (m_pacUrl.trimmed().isEmpty())
            ok = false;
    }

    setSaveEnabled(ok);
}

// ======================= resetForm =======================

void WiFiAddNetworkManager::resetForm()
{
    m_ssid.clear();
    m_securityType = 0;
    m_password.clear();

    m_proxyMode = 0;
    m_httpProxy.clear();
    m_httpsProxy.clear();
    m_socksProxy.clear();
    m_ignoreHosts.clear();

    m_ipMode = 0;
    m_ipAddress.clear();
    m_gateway.clear();
    m_prefixLength.clear();
    m_dns1.clear();
    m_dns2.clear();

    m_macMode = 0;
    m_meteredMode = 0;
    m_hiddenMode = 0;

    m_pacUrl.clear();

    emit ssidChanged();
    emit securityTypeChanged();
    emit passwordChanged();
    emit proxyModeChanged();
    emit httpProxyChanged();
    emit httpsProxyChanged();
    emit socksProxyChanged();
    emit ignoreHostsChanged();
    emit ipModeChanged();
    emit ipAddressChanged();
    emit gatewayChanged();
    emit prefixLengthChanged();
    emit dns1Changed();
    emit dns2Changed();
    emit macModeChanged();
    emit meteredModeChanged();
    emit hiddenModeChanged();
    emit pacUrlChanged();

    validate();
}

// ======================= process helpers =======================

QString WiFiAddNetworkManager::runProcess(const QString &cmd,
                                          const QStringList &args)
{
    QProcess p;
    p.setProgram(cmd);
    p.setArguments(args);

    p.start();
    if (!p.waitForStarted(3000))
        return {};

    if (!p.waitForFinished(15000)) {
        p.kill();
        p.waitForFinished();
        return {};
    }

    return QString::fromLocal8Bit(p.readAllStandardOutput());
}

QString WiFiAddNetworkManager::buildCidr(const QString &ip,
                                         const QString &prefixLength)
{
    bool ok = false;
    int prefix = prefixLength.toInt(&ok);
    if (!ok || (prefix < 0) || (prefix > 32))
        return {};
    return ip + "/" + QString::number(prefix);
}

// ======================= public addNetwork =======================

void WiFiAddNetworkManager::addNetwork()
{
    if (m_busy) {
        emit logMessage("[WiFiAddNetwork] Busy - addNetwork already in progress");
        emit addNetworkResult(false, tr("Another operation is already running"));
        return;
    }

    if (!m_saveEnabled) {
        emit logMessage("[WiFiAddNetwork] addNetwork() called but saveEnabled=false (invalid form)");
        emit addNetworkResult(false, tr("Form is not valid"));
        return;
    }

    AddParams params;
    params.ssid          = m_ssid;
    params.securityType  = m_securityType;
    params.password      = m_password;

    params.proxyMode     = m_proxyMode;
    params.httpProxy     = m_httpProxy;
    params.httpsProxy    = m_httpsProxy;
    params.socksProxy    = m_socksProxy;
    params.ignoreHosts   = m_ignoreHosts;

    params.ipMode        = m_ipMode;
    params.ipAddress     = m_ipAddress;
    params.gateway       = m_gateway;
    params.prefixLength  = m_prefixLength;
    params.dns1          = m_dns1;
    params.dns2          = m_dns2;

    params.macMode       = m_macMode;
    params.meteredMode   = m_meteredMode;
    params.hiddenMode    = m_hiddenMode;

    params.pacUrl        = m_pacUrl;

    setBusy(true);
    emit logMessage("========== WiFi Add Manual Network ==========");
    emit logMessage("[WiFiAddNetwork] Starting for SSID: " + m_ssid);

    QPointer<WiFiAddNetworkManager> that(this);

    auto future = QtConcurrent::run([that, params]() -> AddResult {
        if (!that)
            return {false, QStringLiteral("Object deleted")};
        return that->doAddNetwork(params);
    });

    auto *watcher = new QFutureWatcher<AddResult>(this);
    connect(watcher, &QFutureWatcher<AddResult>::finished, this,
            [this, watcher]() {
                AddResult res = watcher->result();
                watcher->deleteLater();

                setBusy(false);
                emit logMessage("[WiFiAddNetwork] Finished: " + res.message);
                emit logMessage("========== End WiFi Add Manual Network ==========");
                emit addNetworkResult(res.success, res.message);
            });

    watcher->setFuture(future);
}

// ======================= worker logic =======================

WiFiAddNetworkManager::AddResult
WiFiAddNetworkManager::doAddNetwork(const AddParams &p)
{
    AddResult result;

    QString ssidTrimmed = p.ssid.trimmed();
    if (ssidTrimmed.isEmpty()) {
        asyncLog("[WiFiAddNetwork] ERROR: SSID empty in worker");
        result.success = false;
        result.message = tr("SSID is empty");
        return result;
    }

    const QString conName = ssidTrimmed;
    asyncLog("[WiFiAddNetwork] Creating connection: " + conName);

    QString outAdd = runProcess("nmcli", {
                                             "connection", "add",
                                             "type", "wifi",
                                             "ifname", "wlan0",
                                             "con-name", conName,
                                             "ssid", ssidTrimmed
                                         });
    asyncLog("[WiFiAddNetwork] nmcli add:\n" + outAdd.trimmed());

    if (!configureHidden(conName, p.hiddenMode)) {
        result.success = false;
        result.message = tr("Failed to configure hidden mode");
        return result;
    }

    if (!configureSecurity(conName, p.securityType, p.password)) {
        result.success = false;
        result.message = tr("Failed to configure security");
        return result;
    }

    if (!configureIp(conName, p.ipMode, p.ipAddress, p.gateway,
                     p.prefixLength, p.dns1, p.dns2)) {
        result.success = false;
        result.message = tr("Failed to configure IP");
        return result;
    }

    if (!configureProxy(conName, p.proxyMode, p.httpProxy, p.httpsProxy,
                        p.socksProxy, p.ignoreHosts, p.pacUrl)) {
        result.success = false;
        result.message = tr("Failed to configure proxy");
        return result;
    }

    if (!configureMac(conName, p.macMode)) {
        result.success = false;
        result.message = tr("Failed to configure MAC");
        return result;
    }

    if (!configureMetered(conName, p.meteredMode)) {
        result.success = false;
        result.message = tr("Failed to configure metered");
        return result;
    }

    asyncLog("[WiFiAddNetwork] Bringing connection up: " + conName);
    QString outUp = runProcess("nmcli", {"connection", "up", conName});
    asyncLog("[WiFiAddNetwork] nmcli up:\n" + outUp.trimmed());

    result.success = true;
    result.message = tr("Network added successfully");
    return result;
}

// ======================= configure helpers =======================

bool WiFiAddNetworkManager::configureHidden(const QString &conName,
                                            int hiddenMode)
{
    bool isHidden = (hiddenMode == 1); // hiddenYes
    QString value = isHidden ? "yes" : "no";

    asyncLog("[WiFiAddNetwork] Hidden=" + value);

    QString out = runProcess("nmcli", {
                                          "connection", "modify", conName,
                                          "802-11-wireless.hidden", value
                                      });
    asyncLog("[WiFiAddNetwork] Hidden result:\n" + out.trimmed());
    return true;
}

bool WiFiAddNetworkManager::configureSecurity(const QString &conName,
                                              int securityType,
                                              const QString &password)
{
    asyncLog("[WiFiAddNetwork] Security type=" + QString::number(securityType));

    switch (securityType) {
    case 0: { // Open
        QString out = runProcess("nmcli", {
                                              "connection", "modify", conName,
                                              "wifi-sec.key-mgmt", "none"
                                          });
        asyncLog("[WiFiAddNetwork] Security Open:\n" + out.trimmed());
        break;
    }
    case 1: { // WEP
        if (password.trimmed().isEmpty()) {
            asyncLog("[WiFiAddNetwork] ERROR: WEP password empty");
            return false;
        }
        QString out = runProcess("nmcli", {
                                              "connection", "modify", conName,
                                              "wifi-sec.key-mgmt", "none",
                                              "wifi-sec.wep-key0", password,
                                              "wifi-sec.wep-key-type", "1" // passphrase
                                          });
        asyncLog("[WiFiAddNetwork] Security WEP:\n" + out.trimmed());
        break;
    }
    case 2: // WPA-PSK
    case 3: // WPA2-PSK
    case 4: { // WPA/WPA2 Mixed
        if (password.trimmed().isEmpty()) {
            asyncLog("[WiFiAddNetwork] ERROR: WPA/WPA2 password empty");
            return false;
        }
        QString out = runProcess("nmcli", {
                                              "connection", "modify", conName,
                                              "wifi-sec.key-mgmt", "wpa-psk",
                                              "wifi-sec.psk", password
                                          });
        asyncLog("[WiFiAddNetwork] Security WPA/WPA2-PSK:\n" + out.trimmed());
        break;
    }
    case 5: { // WPA3-SAE
        if (password.trimmed().isEmpty()) {
            asyncLog("[WiFiAddNetwork] ERROR: WPA3 password empty");
            return false;
        }
        QString out = runProcess("nmcli", {
                                              "connection", "modify", conName,
                                              "wifi-sec.key-mgmt", "sae",
                                              "wifi-sec.psk", password
                                          });
        asyncLog("[WiFiAddNetwork] Security WPA3-SAE:\n" + out.trimmed());
        break;
    }
    case 6: { // WPA2/WPA3 Mixed
        if (password.trimmed().isEmpty()) {
            asyncLog("[WiFiAddNetwork] ERROR: WPA2/WPA3 password empty");
            return false;
        }
        QString out = runProcess("nmcli", {
                                              "connection", "modify", conName,
                                              "wifi-sec.key-mgmt", "wpa-psk sae",
                                              "wifi-sec.psk", password
                                          });
        asyncLog("[WiFiAddNetwork] Security WPA2/WPA3 Mixed:\n" + out.trimmed());
        break;
    }
    case 7: // WPA2-Enterprise (EAP)
    case 8: { // WPA3-Enterprise (EAP3)
        asyncLog("[WiFiAddNetwork] ERROR: EAP not implemented (no identity/certs UI)");
        return false;
    }
    default:
        asyncLog("[WiFiAddNetwork] ERROR: Unknown security type");
        return false;
    }

    return true;
}

bool WiFiAddNetworkManager::configureIp(const QString &conName,
                                        int ipMode,
                                        const QString &ipAddress,
                                        const QString &gateway,
                                        const QString &prefixLength,
                                        const QString &dns1,
                                        const QString &dns2)
{
    asyncLog("[WiFiAddNetwork] Configuring IP, mode=" + QString::number(ipMode));

    if (ipMode == 0) { // DHCP
        QString out = runProcess("nmcli", {
                                              "connection", "modify", conName,
                                              "ipv4.method", "auto",
                                              "ipv4.addresses", "",
                                              "ipv4.gateway", "",
                                              "ipv4.dns", ""
                                          });
        asyncLog("[WiFiAddNetwork] IP DHCP:\n" + out.trimmed());
        return true;
    }

    QString cidr = buildCidr(ipAddress, prefixLength);
    if (cidr.isEmpty()) {
        asyncLog("[WiFiAddNetwork] ERROR: Invalid IP/prefix");
        return false;
    }

    QStringList args {
        "connection", "modify", conName,
        "ipv4.method", "manual",
        "ipv4.addresses", cidr,
        "ipv4.gateway", gateway
    };

    QString dnsCombined;
    if (!dns1.trimmed().isEmpty())
        dnsCombined = dns1.trimmed();
    if (!dns2.trimmed().isEmpty()) {
        if (!dnsCombined.isEmpty())
            dnsCombined += ",";
        dnsCombined += dns2.trimmed();
    }

    if (!dnsCombined.isEmpty())
        args << "ipv4.dns" << dnsCombined;

    QString out = runProcess("nmcli", args);
    asyncLog("[WiFiAddNetwork] IP Static:\n" + out.trimmed());
    return true;
}

bool WiFiAddNetworkManager::configureProxy(const QString &conName,
                                           int proxyMode,
                                           const QString &httpProxy,
                                           const QString &httpsProxy,
                                           const QString &socksProxy,
                                           const QString &ignoreHosts,
                                           const QString &pacUrl)
{
    asyncLog("[WiFiAddNetwork] Configuring proxy, mode=" + QString::number(proxyMode));

    if (proxyMode == 0) { // None
        QString out = runProcess("nmcli", {
                                              "connection", "modify", conName,
                                              "proxy.method", "none",
                                              "proxy.http", "",
                                              "proxy.https", "",
                                              "proxy.socks", "",
                                              "proxy.ignore-hosts", "",
                                              "proxy.pac-url", ""
                                          });
        asyncLog("[WiFiAddNetwork] Proxy None:\n" + out.trimmed());
        return true;
    }

    if (proxyMode == 1) { // Manual
        QStringList args {
            "connection", "modify", conName,
            "proxy.method", "manual"
        };

        if (!httpProxy.trimmed().isEmpty())
            args << "proxy.http" << httpProxy.trimmed();
        if (!httpsProxy.trimmed().isEmpty())
            args << "proxy.https" << httpsProxy.trimmed();
        if (!socksProxy.trimmed().isEmpty())
            args << "proxy.socks" << socksProxy.trimmed();
        if (!ignoreHosts.trimmed().isEmpty())
            args << "proxy.ignore-hosts" << ignoreHosts.trimmed();

        QString out = runProcess("nmcli", args);
        asyncLog("[WiFiAddNetwork] Proxy Manual:\n" + out.trimmed());
        return true;
    }

    if (proxyMode == 2) { // Auto
        QStringList args {
            "connection", "modify", conName,
            "proxy.method", "auto",
            "proxy.http", "",
            "proxy.https", "",
            "proxy.socks", "",
            "proxy.ignore-hosts", ""
        };

        if (!pacUrl.trimmed().isEmpty())
            args << "proxy.pac-url" << pacUrl.trimmed();

        QString out = runProcess("nmcli", args);
        asyncLog("[WiFiAddNetwork] Proxy Auto:\n" + out.trimmed());
        return true;
    }

    asyncLog("[WiFiAddNetwork] ERROR: Unknown proxy mode");
    return false;
}

bool WiFiAddNetworkManager::configureMac(const QString &conName,
                                         int macMode)
{
    asyncLog("[WiFiAddNetwork] Configuring MAC, mode=" + QString::number(macMode));

    if (macMode == 0) { // device
        QString out = runProcess("nmcli", {
                                              "connection", "modify", conName,
                                              "802-11-wireless.mac-address-randomization", "never",
                                              "802-11-wireless.cloned-mac-address", "permanent"
                                          });
        asyncLog("[WiFiAddNetwork] MAC Device:\n" + out.trimmed());
    } else {            // random
        QString out = runProcess("nmcli", {
                                              "connection", "modify", conName,
                                              "802-11-wireless.mac-address-randomization", "default",
                                              "802-11-wireless.cloned-mac-address", ""
                                          });
        asyncLog("[WiFiAddNetwork] MAC Random:\n" + out.trimmed());
    }

    return true;
}

bool WiFiAddNetworkManager::configureMetered(const QString &conName,
                                             int meteredMode)
{
    asyncLog("[WiFiAddNetwork] Configuring metered, mode=" + QString::number(meteredMode));

    QString value = "auto";
    if (meteredMode == 1)
        value = "yes";
    else if (meteredMode == 2)
        value = "no";

    QString out = runProcess("nmcli", {
                                          "connection", "modify", conName,
                                          "connection.metered", value
                                      });
    asyncLog("[WiFiAddNetwork] Metered=" + value + ":\n" + out.trimmed());
    return true;
}
