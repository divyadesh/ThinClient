#include "wifimanager.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

WiFiManager::WiFiManager(QObject *parent)
    : QObject(parent),
    m_ssid(),
    m_security(0),
    m_securityName("Open"),
    m_password(),
    m_ipMode(0),
    m_ipAddress(),
    m_gateway(),
    m_prefix(24),
    m_dns1(),
    m_dns2(),
    m_macMode(0),
    m_metered(0),
    m_hidden(0),
    m_status(),
    m_signalStrength(0),
    m_macAddress(),
    m_proxyMode(0),
    m_linkSpeed(0)
{
    // load saved values if present
    loadSavedConfig();
}

// -------------------- getters --------------------
QString WiFiManager::ssid() const { return m_ssid; }
int WiFiManager::security() const { return m_security; }
QString WiFiManager::securityName() const { return m_securityName; }
QString WiFiManager::password() const { return m_password; }

int WiFiManager::ipMode() const { return m_ipMode; }
QString WiFiManager::ipAddress() const { return m_ipAddress; }
QString WiFiManager::gateway() const { return m_gateway; }
int WiFiManager::prefix() const { return m_prefix; }
QString WiFiManager::dns1() const { return m_dns1; }
QString WiFiManager::dns2() const { return m_dns2; }

int WiFiManager::macMode() const { return m_macMode; }
int WiFiManager::metered() const { return m_metered; }
int WiFiManager::hidden() const { return m_hidden; }
int WiFiManager::proxyMode() const { return m_proxyMode; }

QString WiFiManager::status() const { return m_status; }
int WiFiManager::signalStrength() const { return m_signalStrength; }
QString WiFiManager::macAddress() const { return m_macAddress; }

// -------------------- setters --------------------
void WiFiManager::setSsid(const QString &v) { if (m_ssid==v) return; m_ssid=v; emit ssidChanged(); }
void WiFiManager::setSecurity(int v)
{
    if (m_security == v)
        return;

    m_security = v;
    emit securityChanged();

    // keep a human-readable name in sync (read-only QML property)
    QString name = mapSecurityEnumToName(v);
    if (m_securityName != name) {
        m_securityName = name;
        emit securityNameChanged();
    }
}
void WiFiManager::setPassword(const QString &v) { if (m_password==v) return; m_password=v; emit passwordChanged(); }

void WiFiManager::setIpMode(int v) { if (m_ipMode==v) return; m_ipMode=v; emit ipModeChanged(); }
void WiFiManager::setIpAddress(const QString &v) { if (m_ipAddress==v) return; m_ipAddress=v; emit ipAddressChanged(); }
void WiFiManager::setGateway(const QString &v) { if (m_gateway==v) return; m_gateway=v; emit gatewayChanged(); }
void WiFiManager::setPrefix(int v) { if (m_prefix==v) return; m_prefix=v; emit prefixChanged(); }
void WiFiManager::setDns1(const QString &v) { if (m_dns1==v) return; m_dns1=v; emit dns1Changed(); }
void WiFiManager::setDns2(const QString &v) { if (m_dns2==v) return; m_dns2=v; emit dns2Changed(); }

void WiFiManager::setMacMode(int v) { if (m_macMode==v) return; m_macMode=v; emit macModeChanged(); }
void WiFiManager::setMetered(int v) { if (m_metered==v) return; m_metered=v; emit meteredChanged(); }
void WiFiManager::setHidden(int v) { if (m_hidden==v) return; m_hidden=v; emit hiddenChanged(); }
void WiFiManager::setProxyMode(int newProxyMode) { if (m_proxyMode == newProxyMode) return; m_proxyMode = newProxyMode; emit proxyModeChanged(); }

// status setters
void WiFiManager::setStatus(const QString &s) { if (m_status == s) return; m_status = s; emit statusChanged(); }
void WiFiManager::setSignalStrength(int v) { if (m_signalStrength == v) return; m_signalStrength = v; emit signalStrengthChanged(); }
void WiFiManager::setMacAddress(const QString &m) { if (m_macAddress == m) return; m_macAddress = m; emit macAddressChanged(); }

// -------------------- validation --------------------
QVariantMap WiFiManager::validateInputs() const
{
    QVariantMap out;
    out["valid"] = false;
    QString msg;

    if (m_ssid.trimmed().isEmpty()) {
        msg = "SSID is required";
    }

    // security-specific checks
    // Assuming AppEnums: open=0, WEP=1, WPA=2, WPA2=3, WPA3=5 etc.
    if (msg.isEmpty()) {
        if (m_security == 2 || m_security == 3 || m_security == 5 || m_security == 6) { // PSK types
            if (m_password.length() < 8) {
                msg = "Password required (min 8 characters) for WPA/WPA2/WPA3";
            }
        } else if (m_security == 1) { // WEP
            if (m_password.length() < 5) {
                msg = "WEP key seems too short";
            }
        } else if (m_security == 7 || m_security == 8) { // Enterprise
            // Enterprise requires additional fields — not implemented here
            msg = "Enterprise security not implemented in this UI";
        }
    }

    // If static IP chosen, validate IP fields
    if (msg.isEmpty() && m_ipMode != 0) { // ipMode != DHCP
        if (!validateIpAndPrefix(m_ipAddress, m_prefix)) {
            msg = "Invalid IPv4 address or prefix";
        } else if (!m_gateway.isEmpty() && !validateIp(m_gateway)) {
            msg = "Invalid gateway";
        } else if (!m_dns1.isEmpty() && !validateDns(m_dns1)) {
            msg = "Primary DNS is invalid";
        } else if (!m_dns2.isEmpty() && !validateDns(m_dns2)) {
            msg = "Secondary DNS is invalid";
        }
    }

    if (msg.isEmpty()) {
        out["valid"] = true;
        out["message"] = "OK";
    } else {
        out["valid"] = false;
        out["message"] = msg;
    }

    return out;
}

// -------------------- apply (system changes) --------------------
void WiFiManager::apply()
{
    QVariantMap v = validateInputs();
    if (!v["valid"].toBool()) {
        emit applyResult(false, v["message"].toString());
        emit logMessage(QString("Validation failed: %1").arg(v["message"].toString()));
        return;
    }

    // Persist config locally
    saveConfig();

    // Apply IP settings if static is chosen (platform-specific)
    if (m_ipMode == 1) { // ipStatic
        QString iface = "wlan0"; // TODO: detect real interface
        // flush addr
        QString stdoutOut, stderrOut;
        runCommand("ip", {"addr", "flush", "dev", iface}, &stdoutOut, &stderrOut);
        // add ip
        QString cidr = QString("%1/%2").arg(m_ipAddress).arg(m_prefix);
        if (!runCommand("ip", {"addr", "add", cidr, "dev", iface}, &stdoutOut, &stderrOut)) {
            emit applyResult(false, QString("Failed to add IP: %1").arg(stderrOut));
            emit logMessage(QString("ip addr add failed: %1").arg(stderrOut));
            return;
        }
        // set default route
        runCommand("ip", {"route", "del", "default"}, nullptr, nullptr);
        if (!runCommand("ip", {"route", "add", "default", "via", m_gateway, "dev", iface}, &stdoutOut, &stderrOut)) {
            emit applyResult(false, QString("Failed to set gateway: %1").arg(stderrOut));
            emit logMessage(QString("ip route add failed: %1").arg(stderrOut));
            return;
        }
        // write resolv.conf (best-effort)
        QString resolvPath = "/etc/resolv.conf";
        QFile f(resolvPath);
        if (f.exists()) QFile::remove(resolvPath);
        if (f.open(QIODevice::WriteOnly)) {
            QTextStream ts(&f);
            ts << "nameserver " << (m_dns1.isEmpty() ? "8.8.8.8" : m_dns1) << "\n";
            if (!m_dns2.isEmpty()) ts << "nameserver " << m_dns2 << "\n";
            f.close();
        }
    }

    // Now configure Wi-Fi credentials (wpa_supplicant, NetworkManager, etc.)
    // NOTE: This is platform dependent — below is a placeholder demonstrating intent.

    // For a basic WPA-PSK network using wpa_passphrase -> wpa_supplicant.conf update, then restart wpa_supplicant.
    // We'll just emit success for now.
    emit applyResult(true, "Applied (note: real connect should be implemented per platform)");
    emit logMessage("apply() finished: success");
}

// -------------------- connect (calls apply then triggers connection) --------------------
void WiFiManager::connect()
{
    // For simplicity call apply() first
    apply();
    // After applying, you would normally call "wpa_cli -i wlan0 reconfigure" or use NetworkManager D-Bus API
    emit connectResult(true, "Connect triggered (implement platform-specific connect)");
}

// -------------------- persistence --------------------
QString WiFiManager::configFilePath() const
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir d(dir);
    if (!d.exists()) d.mkpath(".");
    return d.filePath("wifi_config.json");
}

void WiFiManager::saveConfig()
{
    QJsonObject obj;
    obj["ssid"] = m_ssid;
    obj["security"] = m_security;
    obj["password"] = m_password;
    obj["ipMode"] = m_ipMode;
    obj["ipAddress"] = m_ipAddress;
    obj["gateway"] = m_gateway;
    obj["prefix"] = m_prefix;
    obj["dns1"] = m_dns1;
    obj["dns2"] = m_dns2;
    obj["macMode"] = m_macMode;
    obj["metered"] = m_metered;
    obj["hidden"] = m_hidden;
    obj["proxyMode"] = m_proxyMode;
    // securityName is derived from security, but save it as convenience
    obj["securityName"] = m_securityName;

    QFile f(configFilePath());
    if (!f.open(QIODevice::WriteOnly)) {
        emit logMessage(QString("Could not open config file for writing: %1").arg(f.errorString()));
        return;
    }
    f.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
    f.close();
    emit logMessage("Saved wifi config to " + configFilePath());
}

void WiFiManager::loadSavedConfig()
{
    QFile f(configFilePath());
    if (!f.exists()) return;
    if (!f.open(QIODevice::ReadOnly)) return;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();
    if (!doc.isObject()) return;
    QJsonObject obj = doc.object();
    m_ssid = obj.value("ssid").toString();
    m_security = obj.value("security").toInt();
    m_password = obj.value("password").toString();
    m_ipMode = obj.value("ipMode").toInt();
    m_ipAddress = obj.value("ipAddress").toString();
    m_gateway = obj.value("gateway").toString();
    m_prefix = obj.value("prefix").toInt(24);
    m_dns1 = obj.value("dns1").toString();
    m_dns2 = obj.value("dns2").toString();
    m_macMode = obj.value("macMode").toInt();
    m_metered = obj.value("metered").toInt();
    m_hidden = obj.value("hidden").toInt();
    m_proxyMode = obj.value("proxyMode").toInt();

    // derive securityName from enum (fallback to saved string if present)
    if (obj.contains("securityName")) {
        m_securityName = obj.value("securityName").toString();
    } else {
        m_securityName = mapSecurityEnumToName(m_security);
    }

    // emit change signals
    emit ssidChanged();
    emit securityChanged();
    emit securityNameChanged();
    emit passwordChanged();

    emit ipModeChanged();
    emit ipAddressChanged();
    emit gatewayChanged();
    emit prefixChanged();
    emit dns1Changed();
    emit dns2Changed();

    emit macModeChanged();
    emit meteredChanged();
    emit hiddenChanged();
    emit proxyModeChanged();

    emit logMessage("Loaded saved wifi config");
}

// -------------------- utility helpers --------------------
bool WiFiManager::runCommand(const QString &prog, const QStringList &args, QString *stdoutOut, QString *stderrOut, int timeoutMs) const
{
    QProcess p;
    p.start(prog, args);
    if (!p.waitForStarted(3000)) {
        if (stderrOut) *stderrOut = "Failed to start process";
        return false;
    }
    if (!p.waitForFinished(timeoutMs)) {
        p.kill();
        if (stderrOut) *stderrOut = "Process timeout";
        return false;
    }
    if (stdoutOut) *stdoutOut = QString::fromLocal8Bit(p.readAllStandardOutput()).trimmed();
    if (stderrOut) *stderrOut = QString::fromLocal8Bit(p.readAllStandardError()).trimmed();
    return p.exitCode() == 0;
}

bool WiFiManager::validateIp(const QString &ip) const
{
    // basic IPv4 check using QRegularExpression
    static const QRegularExpression rx(R"(^((25[0-5]|2[0-4]\d|[01]?\d?\d)\.){3}(25[0-5]|2[0-4]\d|[01]?\d?\d)$)");
    return rx.match(ip.trimmed()).hasMatch();
}

bool WiFiManager::validateIpAndPrefix(const QString &ip, int prefix) const
{
    if (!validateIp(ip)) return false;
    if ((prefix < 1) || (prefix > 32)) return false;
    return true;
}

bool WiFiManager::validateDns(const QString &d) const
{
    return validateIp(d);
}

QString WiFiManager::mapSecurityEnumToName(int sec) const
{
    switch (sec) {
    case 0: return QStringLiteral("Open");
    case 1: return QStringLiteral("WEP");
    case 2: return QStringLiteral("WPA-PSK");
    case 3: return QStringLiteral("WPA2-PSK");
    case 4: return QStringLiteral("WPA/WPA2 Mixed");
    case 5: return QStringLiteral("WPA3-SAE");
    case 6: return QStringLiteral("WPA2/WPA3 Mixed");
    case 7: return QStringLiteral("WPA2-Enterprise (EAP)");
    case 8: return QStringLiteral("WPA3-Enterprise (EAP)");
    default: return QStringLiteral("Unknown");
    }
}

void WiFiManager::onProcessFinished(int /*exitCode*/, QProcess::ExitStatus /*status*/)
{
    // placeholder for async process handling if needed
}

// End of file

int WiFiManager::linkSpeed() const
{
    return m_linkSpeed;
}

void WiFiManager::setLinkSpeed(int newLinkSpeed)
{
    if (m_linkSpeed == newLinkSpeed)
        return;
    m_linkSpeed = newLinkSpeed;
    emit linkSpeedChanged();
}
