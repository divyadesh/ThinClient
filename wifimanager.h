#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <QObject>
#include <QString>
#include <QProcess>
#include <QTimer>
#include <QVariantMap>

class WiFiManager : public QObject
{
    Q_OBJECT

    // Basic editable properties (bound from QML)
    Q_PROPERTY(QString ssid READ ssid WRITE setSsid NOTIFY ssidChanged)
    Q_PROPERTY(int security READ security WRITE setSecurity NOTIFY securityChanged) // use AppEnums values
    Q_PROPERTY(QString securityName READ securityName NOTIFY securityNameChanged) // read-only, derived from security
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)

    Q_PROPERTY(int ipMode READ ipMode WRITE setIpMode NOTIFY ipModeChanged) // AppEnums ipDHCP/ipStatic
    Q_PROPERTY(QString ipAddress READ ipAddress WRITE setIpAddress NOTIFY ipAddressChanged)
    Q_PROPERTY(QString gateway READ gateway WRITE setGateway NOTIFY gatewayChanged)
    Q_PROPERTY(int prefix READ prefix WRITE setPrefix NOTIFY prefixChanged)
    Q_PROPERTY(QString dns1 READ dns1 WRITE setDns1 NOTIFY dns1Changed)
    Q_PROPERTY(QString dns2 READ dns2 WRITE setDns2 NOTIFY dns2Changed)

    Q_PROPERTY(int macMode READ macMode WRITE setMacMode NOTIFY macModeChanged) // AppEnums
    Q_PROPERTY(int metered READ metered WRITE setMetered NOTIFY meteredChanged)
    Q_PROPERTY(int hidden READ hidden WRITE setHidden NOTIFY hiddenChanged)
    Q_PROPERTY(int proxyMode READ proxyMode WRITE setProxyMode NOTIFY proxyModeChanged)

    // Read-only status properties
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(int signalStrength READ signalStrength NOTIFY signalStrengthChanged)
    Q_PROPERTY(QString macAddress READ macAddress NOTIFY macAddressChanged)
    Q_PROPERTY(int linkSpeed READ linkSpeed WRITE setLinkSpeed NOTIFY linkSpeedChanged FINAL)

public:
    explicit WiFiManager(QObject *parent = nullptr);

    // getters
    QString ssid() const;
    int security() const;
    QString securityName() const;
    QString password() const;

    int ipMode() const;
    QString ipAddress() const;
    QString gateway() const;
    int prefix() const;
    QString dns1() const;
    QString dns2() const;

    int macMode() const;
    int metered() const;
    int hidden() const;
    int proxyMode() const;

    QString status() const;
    int signalStrength() const;
    QString macAddress() const;

    // setters
    void setSsid(const QString &v);
    void setSecurity(int v);
    void setPassword(const QString &v);

    void setIpMode(int v);
    void setIpAddress(const QString &v);
    void setGateway(const QString &v);
    void setPrefix(int v);
    void setDns1(const QString &v);
    void setDns2(const QString &v);

    void setMacMode(int v);
    void setMetered(int v);
    void setHidden(int v);
    void setProxyMode(int newProxyMode);

    void setStatus(const QString &s);
    void setSignalStrength(int v);
    void setMacAddress(const QString &m);

    // helper invokable methods
    Q_INVOKABLE QVariantMap validateInputs() const; // returns { valid: bool, message: "..." }
    Q_INVOKABLE void apply();   // apply network config (save/push to system)
    Q_INVOKABLE void connect(); // attempt connect to network (calls apply if needed)
    Q_INVOKABLE void loadSavedConfig(); // load persisted config
    Q_INVOKABLE void saveConfig(); // save config locally

    int linkSpeed() const;
    void setLinkSpeed(int newLinkSpeed);

signals:
    // property change signals
    void ssidChanged();
    void securityChanged();
    void securityNameChanged();
    void passwordChanged();

    void ipModeChanged();
    void ipAddressChanged();
    void gatewayChanged();
    void prefixChanged();
    void dns1Changed();
    void dns2Changed();

    void macModeChanged();
    void meteredChanged();
    void hiddenChanged();
    void proxyModeChanged();

    // status signals
    void statusChanged();
    void signalStrengthChanged();
    void macAddressChanged();

    // results
    void applyResult(bool ok, const QString &message);   // success or failure
    void connectResult(bool ok, const QString &message);

    void logMessage(const QString &line);

    void linkSpeedChanged();

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus status);

private:
    // internal helpers
    bool runCommand(const QString &prog, const QStringList &args, QString *stdoutOut = nullptr, QString *stderrOut = nullptr, int timeoutMs = 20000) const;
    bool validateIp(const QString &ip) const;
    bool validateIpAndPrefix(const QString &ip, int prefix) const;
    bool validateDns(const QString &d) const;
    QString mapSecurityEnumToName(int sec) const;

    // persisted config path helper (platform-specific)
    QString configFilePath() const;

    // data members
    QString m_ssid;
    int m_security;
    QString m_securityName;
    QString m_password;

    int m_ipMode;
    QString m_ipAddress;
    QString m_gateway;
    int m_prefix;
    QString m_dns1;
    QString m_dns2;

    int m_macMode;
    int m_metered;
    int m_hidden;

    QString m_status;
    int m_signalStrength;
    QString m_macAddress;
    int m_proxyMode;
    int m_linkSpeed;
};

#endif // WIFIMANAGER_H
