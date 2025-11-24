#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

// NOTE: This class assumes your QML AppEnums values are:
//
// securityOpen      = 0
// securityWEP       = 1
// securityWPA       = 2
// securityWPA2      = 3
// securityWPAMixed  = 4
// securityWPA3      = 5
// securityWPA2WPA3  = 6
// securityEAP       = 7
// securityEAP3      = 8
//
// proxyNone   = 0, proxyManual = 1, proxyAuto = 2
// ipDHCP      = 0, ipStatic    = 1
// macDevice   = 0, macRandom   = 1
// meteredAuto = 0, meteredYes  = 1, meteredNo = 2
// hiddenNo    = 0, hiddenYes   = 1
//
// From QML, bind directly to these properties and call addNetwork().

class WiFiAddNetworkManager : public QObject
{
    Q_OBJECT

    // -------- Form fields exposed to QML --------
    Q_PROPERTY(QString ssid READ ssid WRITE setSsid NOTIFY ssidChanged)
    Q_PROPERTY(int securityType READ securityType WRITE setSecurityType NOTIFY securityTypeChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)

    Q_PROPERTY(int proxyMode READ proxyMode WRITE setProxyMode NOTIFY proxyModeChanged)
    Q_PROPERTY(QString httpProxy READ httpProxy WRITE setHttpProxy NOTIFY httpProxyChanged)
    Q_PROPERTY(QString httpsProxy READ httpsProxy WRITE setHttpsProxy NOTIFY httpsProxyChanged)
    Q_PROPERTY(QString socksProxy READ socksProxy WRITE setSocksProxy NOTIFY socksProxyChanged)
    Q_PROPERTY(QString ignoreHosts READ ignoreHosts WRITE setIgnoreHosts NOTIFY ignoreHostsChanged)

    Q_PROPERTY(int ipMode READ ipMode WRITE setIpMode NOTIFY ipModeChanged)
    Q_PROPERTY(QString ipAddress READ ipAddress WRITE setIpAddress NOTIFY ipAddressChanged)
    Q_PROPERTY(QString gateway READ gateway WRITE setGateway NOTIFY gatewayChanged)
    Q_PROPERTY(QString prefixLength READ prefixLength WRITE setPrefixLength NOTIFY prefixLengthChanged)
    Q_PROPERTY(QString dns1 READ dns1 WRITE setDns1 NOTIFY dns1Changed)
    Q_PROPERTY(QString dns2 READ dns2 WRITE setDns2 NOTIFY dns2Changed)

    Q_PROPERTY(int macMode READ macMode WRITE setMacMode NOTIFY macModeChanged)
    Q_PROPERTY(int meteredMode READ meteredMode WRITE setMeteredMode NOTIFY meteredModeChanged)
    Q_PROPERTY(int hiddenMode READ hiddenMode WRITE setHiddenMode NOTIFY hiddenModeChanged)

    Q_PROPERTY(QString pacUrl READ pacUrl WRITE setPacUrl NOTIFY pacUrlChanged)

    // -------- State for UI --------
    Q_PROPERTY(bool saveEnabled READ saveEnabled NOTIFY saveEnabledChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)

public:
    explicit WiFiAddNetworkManager(QObject *parent = nullptr);

    // ---- getters ----
    QString ssid() const                 { return m_ssid; }
    int securityType() const            { return m_securityType; }
    QString password() const            { return m_password; }

    int proxyMode() const               { return m_proxyMode; }
    QString httpProxy() const           { return m_httpProxy; }
    QString httpsProxy() const          { return m_httpsProxy; }
    QString socksProxy() const          { return m_socksProxy; }
    QString ignoreHosts() const         { return m_ignoreHosts; }

    int ipMode() const                  { return m_ipMode; }
    QString ipAddress() const           { return m_ipAddress; }
    QString gateway() const             { return m_gateway; }
    QString prefixLength() const        { return m_prefixLength; }
    QString dns1() const                { return m_dns1; }
    QString dns2() const                { return m_dns2; }

    int macMode() const                 { return m_macMode; }
    int meteredMode() const             { return m_meteredMode; }
    int hiddenMode() const              { return m_hiddenMode; }

    QString pacUrl() const              { return m_pacUrl; }

    bool saveEnabled() const            { return m_saveEnabled; }
    bool busy() const                   { return m_busy; }

    // ---- setters (called from QML bindings) ----
    void setSsid(const QString &v);
    void setSecurityType(int v);
    void setPassword(const QString &v);

    void setProxyMode(int v);
    void setHttpProxy(const QString &v);
    void setHttpsProxy(const QString &v);
    void setSocksProxy(const QString &v);
    void setIgnoreHosts(const QString &v);

    void setIpMode(int v);
    void setIpAddress(const QString &v);
    void setGateway(const QString &v);
    void setPrefixLength(const QString &v);
    void setDns1(const QString &v);
    void setDns2(const QString &v);

    void setMacMode(int v);
    void setMeteredMode(int v);
    void setHiddenMode(int v);

    void setPacUrl(const QString &v);

    // ---- API called from QML ----
    Q_INVOKABLE void resetForm();   // optional helper
    Q_INVOKABLE void addNetwork();  // async, non-blocking, uses the properties above

signals:
    // property signals
    void ssidChanged();
    void securityTypeChanged();
    void passwordChanged();

    void proxyModeChanged();
    void httpProxyChanged();
    void httpsProxyChanged();
    void socksProxyChanged();
    void ignoreHostsChanged();

    void ipModeChanged();
    void ipAddressChanged();
    void gatewayChanged();
    void prefixLengthChanged();
    void dns1Changed();
    void dns2Changed();

    void macModeChanged();
    void meteredModeChanged();
    void hiddenModeChanged();

    void pacUrlChanged();

    void saveEnabledChanged();
    void busyChanged();

    // logging and result signals
    void logMessage(const QString &msg);
    void addNetworkResult(bool success, const QString &message);

private:
    // data members
    QString m_ssid;
    int     m_securityType = 0;
    QString m_password;

    int     m_proxyMode = 0;
    QString m_httpProxy;
    QString m_httpsProxy;
    QString m_socksProxy;
    QString m_ignoreHosts;

    int     m_ipMode = 0;
    QString m_ipAddress;
    QString m_gateway;
    QString m_prefixLength;
    QString m_dns1;
    QString m_dns2;

    int     m_macMode = 0;
    int     m_meteredMode = 0;
    int     m_hiddenMode = 0;

    QString m_pacUrl;

    bool    m_saveEnabled = false;
    bool    m_busy = false;

    // ------------ internal helpers ------------
    void validate();            // recompute saveEnabled
    void setSaveEnabled(bool v);
    void setBusy(bool v);
    void asyncLog(const QString &msg);

    // ==== async worker structures ====
    struct AddParams {
        QString ssid;
        int securityType;
        QString password;

        int proxyMode;
        QString httpProxy;
        QString httpsProxy;
        QString socksProxy;
        QString ignoreHosts;

        int ipMode;
        QString ipAddress;
        QString gateway;
        QString prefixLength;
        QString dns1;
        QString dns2;

        int macMode;
        int meteredMode;
        int hiddenMode;

        QString pacUrl;
    };

    struct AddResult {
        bool success = false;
        QString message;
    };

    // worker executed in background thread
    AddResult doAddNetwork(const AddParams &params);

    // process helpers (used in worker thread)
    static QString runProcess(const QString &cmd, const QStringList &args);
    static QString buildCidr(const QString &ip, const QString &prefixLength);

    bool configureHidden(const QString &conName, int hiddenMode);
    bool configureSecurity(const QString &conName, int securityType, const QString &password);
    bool configureIp(const QString &conName, int ipMode,
                     const QString &ipAddress, const QString &gateway,
                     const QString &prefixLength,
                     const QString &dns1, const QString &dns2);
    bool configureProxy(const QString &conName, int proxyMode,
                        const QString &httpProxy,
                        const QString &httpsProxy,
                        const QString &socksProxy,
                        const QString &ignoreHosts,
                        const QString &pacUrl);
    bool configureMac(const QString &conName, int macMode);
    bool configureMetered(const QString &conName, int meteredMode);
};
