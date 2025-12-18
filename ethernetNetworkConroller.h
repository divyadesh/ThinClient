#ifndef ETHERNETNETWORKCONROLLER_H
#define ETHERNETNETWORKCONROLLER_H

#include <QObject>
#include <QProcess>
#include <QTimer>
#include <QString>
#include <QStringList>

/*!
 * \class EthernetNetworkConroller
 * \brief Manages Ethernet network configuration and status.
 *
 * This class provides:
 *  - Control over a single Ethernet interface (default: "eth0").
 *  - DHCP start/stop control.
 *  - Static IPv4 configuration via NetworkManager (applyStaticConfig()).
 *  - Periodic status polling (link, IP, speed, MAC, etc.).
 *  - QML-accessible properties for UI binding.
 *  - Logging via the logMessage() signal.
 */
class EthernetNetworkConroller : public QObject
{
    Q_OBJECT

    /*!
     * \property interface
     * \brief Name of the Ethernet network interface (e.g. "eth0").
     */
    Q_PROPERTY(QString interface READ interface WRITE setInterface NOTIFY interfaceChanged)

    /*!
     * \property status
     * \brief Human-readable description of the current link/connection status.
     */
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)

    /*!
     * \property ipAddress
     * \brief Current IPv4 address assigned to the interface.
     */
    Q_PROPERTY(QString ipAddress READ ipAddress WRITE setIpAddress NOTIFY ipAddressChanged)

    /*!
     * \property linkSpeed
     * \brief Link speed string as reported by the system (e.g. "1000 Mb/s").
     */
    Q_PROPERTY(QString linkSpeed READ linkSpeed NOTIFY linkSpeedChanged)

    /*!
     * \property dhcpRunning
     * \brief Indicates whether a DHCP client is currently running for this interface.
     */
    Q_PROPERTY(bool dhcpRunning READ dhcpRunning NOTIFY dhcpRunningChanged)

    /*!
     * \property macAddress
     * \brief MAC address of the Ethernet interface.
     */
    Q_PROPERTY(QString macAddress READ macAddress NOTIFY macAddressChanged)

    /*!
     * \property dnsRecords
     * \brief List of DNS servers currently configured for the interface.
     */
    Q_PROPERTY(QStringList dnsRecords READ dnsRecords WRITE setDnsRecords NOTIFY dnsRecordsChanged FINAL)

    /*!
     * \property subnetMask
     * \brief Subnet mask for the current IPv4 configuration (e.g. "255.255.255.0").
     */
    Q_PROPERTY(QString subnetMask READ subnetMask WRITE setSubnetMask NOTIFY subnetMaskChanged)

    /*!
     * \property gateway
     * \brief Default gateway for the current IPv4 configuration.
     */
    Q_PROPERTY(QString gateway READ gateway WRITE setGateway NOTIFY gatewayChanged FINAL)
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)

public:
    /*!
     * \brief Constructs an EthernetNetworkConroller object.
     * \param parent Parent QObject.
     */
    explicit EthernetNetworkConroller(QObject *parent = nullptr);

    //! Returns the interface name (e.g. "eth0").
    QString interface() const { return m_interface; }
    //! Sets the interface name.
    void setInterface(const QString &ifname);

    //! Returns the current status string.
    QString status() const { return m_status; }

    //! Returns the currently assigned IPv4 address.
    QString ipAddress() const { return m_ipAddress; }

    //! Returns the current link speed string.
    QString linkSpeed() const { return m_speed; }

    //! Returns true if DHCP is running for this interface.
    bool dhcpRunning() const { return m_dhcpRunning; }

    //! Returns the MAC address of the interface.
    QString macAddress() const { return m_mac; }

    //! Returns the subnet mask for the current IPv4 configuration.
    QString subnetMask() const;
    //! Sets the subnet mask.
    void setSubnetMask(const QString &newSubnetMask);

    //! Returns the default gateway.
    QString gateway() const;
    //! Sets the default gateway.
    void setGateway(const QString &newGateway);

    //! Returns the list of configured DNS records.
    QStringList dnsRecords() const;
    //! Sets the list of DNS servers.
    void setDnsRecords(const QStringList &newDnsRecords);

    //! Sets the current IPv4 address string.
    void setIpAddress(const QString &newIpAddress);

    bool isBusy() const;

public slots:
    /*!
     * \brief Starts the DHCP client for this interface.
     */
    void enableDhcpAsync();
    bool enableDhcpWorker();
    bool startDhcp();

    /*!
     * \brief Stops the DHCP client for this interface.
     */
    bool stopDhcp();

    /*!
     * \brief Refreshes the internal status information.
     *
     * Typically updates status, IP, speed, MAC, etc.
     */
    void refreshStatus();

    /*!
     * \brief Returns the NetworkManager connection name associated with a given interface.
     *
     * \param ifName Interface name (e.g. "eth0").
     * \return NetworkManager connection name, or an empty string if not found.
     */
    QString getNmConnectionForInterface(const QString &ifName);

    /*!
     * \brief Applies a static IPv4 configuration using NetworkManager.
     *
     * \param ip       IPv4 address (e.g. "192.168.1.10").
     * \param cidrMask CIDR prefix (e.g. 24 for 255.255.255.0).
     * \param gateway  Default gateway address.
     * \param dns1     Primary DNS server.
     * \param dns2     Secondary DNS server (optional).
     */
    bool applyStaticConfigWorker(
        const QString &ip,
        int cidrMask,
        const QString &gateway,
        const QString &dns1,
        const QString &dns2);

    void applyStaticConfigAsync(
        const QString &ip,
        int cidrMask,
        const QString &gateway,
        const QString &dns1,
        const QString &dns2);

    /*!
     * \brief Converts a subnet mask string (e.g. "255.255.255.0") to a CIDR prefix length.
     * \param mask Subnet mask string.
     * \return CIDR prefix length (0–32) or -1 on error.
     */
    int maskToCidr(const QString &mask);

signals:
    //! Emitted when the interface name changes.
    void interfaceChanged();
    //! Emitted when the status string changes.
    void statusChanged();
    //! Emitted when the IP address changes.
    void ipAddressChanged();
    //! Emitted when the link speed changes.
    void linkSpeedChanged();
    //! Emitted when the DHCP running state changes.
    void dhcpRunningChanged();
    //! Emitted to provide log messages to the UI or logger.
    void logMessage(const QString &msg);
    //! Emitted when the MAC address changes.
    void macAddressChanged();
    //! Emitted when the subnet mask changes.
    void subnetMaskChanged();
    //! Emitted when the gateway changes.
    void gatewayChanged();
    //! Emitted when the DNS records list changes.
    void dnsRecordsChanged();

    void operationStarted();
    void operationFinished(bool success, const QString &message);

    void isBusyChanged();

private slots:
    /*!
     * \brief Handles standard output from the internal QProcess.
     */
    void onProcessOutput();

    /*!
     * \brief Handles process-level errors from the internal QProcess.
     * \param err QProcess error code.
     */
    void onProcessError(QProcess::ProcessError err);

private:
    /*!
     * \brief Runs a command and collects its standard output.
     *
     * \param program   Program to execute (e.g. "nmcli").
     * \param args      Argument list.
     * \param timeoutMs Timeout in milliseconds.
     * \return Trimmed standard output as a QString.
     */
    QString runCommandCollect(const QString &program,
                              const QStringList &args,
                              int timeoutMs = 3000);
    bool runCommandBool(const QString &program,
                   const QStringList &args,
                   int timeoutMs = 3000);

    void setBusy(bool b) {
        if (m_isBusy == b) return;
        m_isBusy = b;
        emit isBusyChanged();
    }

    QString   m_interface{"eth0"};   //!< Target Ethernet interface name.
    QString   m_status{"unknown"};   //!< Human-readable status.
    QString   m_speed;               //!< Link speed string.
    bool      m_dhcpRunning{false};  //!< DHCP running state.
    QProcess  m_proc;                //!< Internal process for external commands.
    QTimer    m_pollTimer;           //!< Timer used for periodic status polling.
    QString   m_mac;                 //!< MAC address of the interface.
    QString   m_subnetMask;          //!< Subnet mask string.
    QString   m_gateway;             //!< Default gateway.
    QStringList m_dnsRecords;        //!< DNS servers list.
    QString   m_ipAddress;           //!< Current IPv4 address.
    bool m_isBusy{false};
};

#endif // ETHERNETNETWORKCONROLLER_H
