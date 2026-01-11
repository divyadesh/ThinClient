#pragma once

#include <QObject>
#include <QString>

/**
 * @class ConnectionInfo
 * @brief Represents a full remote connection profile used by the application.
 *
 * This class exposes a complete set of connection parameters such as server IP,
 * gateway settings, audio options, credentials, and performance configurations.
 * All values are exposed using Q_PROPERTY so the object can be directly bound
 * and manipulated from QML.
 *
 * Typical usage:
 * @code
 * ConnectionInfo *info = new ConnectionInfo();
 * info->setServerIp("192.168.1.10");
 * info->setUserName("admin");
 * @endcode
 */
class ConnectionInfo : public QObject
{
    Q_OBJECT

    /* ============================================================
     *  Q_PROPERTY declarations
     * ============================================================ */

    /** @property QString connectionId
     *  @brief Unique identifier for the connection profile.
     */
    Q_PROPERTY(
        QString connectionId READ connectionId WRITE setConnectionId NOTIFY connectionIdChanged)

    /** @property QString connectionName
     *  @brief User-friendly display name of the connection entry.
     */
    Q_PROPERTY(QString connectionName READ connectionName WRITE setConnectionName NOTIFY
                   connectionNameChanged)

    /** @property QString serverIp
     *  @brief IP or hostname of the primary server.
     */
    Q_PROPERTY(QString serverIp READ serverIp WRITE setServerIp NOTIFY serverIpChanged)

    /** @property QString deviceName
     *  @brief Optional device name used to identify the client.
     */
    Q_PROPERTY(QString deviceName READ deviceName WRITE setDeviceName NOTIFY deviceNameChanged)

    /** @property QString userName
     *  @brief Username used for authentication.
     */
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)

    /** @property QString password
     *  @brief Password used for authentication.
     */
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)

    /** @property QString performance
     *  @brief Performance parameters (quality, bandwidth mode, etc.)
     */
    Q_PROPERTY(QString performance READ performance WRITE setPerformance NOTIFY performanceChanged)

    /** @property bool enableAudio
     *  @brief Enables remote audio playback.
     */
    Q_PROPERTY(bool enableAudio READ enableAudio WRITE setEnableAudio NOTIFY enableAudioChanged)

    /** @property bool enableMicrophone
     *  @brief Enables microphone redirection.
     */
    Q_PROPERTY(bool enableMicrophone READ enableMicrophone WRITE setEnableMicrophone NOTIFY
                   enableMicrophoneChanged)

    /** @property bool redirectDrive
     *  @brief Enables drive redirection.
     */
    Q_PROPERTY(
        bool redirectDrive READ redirectDrive WRITE setRedirectDrive NOTIFY redirectDriveChanged)

    /** @property bool redirectUsbDevice
     *  @brief Enables USB device redirection.
     */
    Q_PROPERTY(bool redirectUsbDevice READ redirectUsbDevice WRITE setRedirectUsbDevice NOTIFY
                   redirectUsbDeviceChanged)

    /** @property bool security
     *  @brief Enables enhanced security settings.
     */
    Q_PROPERTY(bool security READ security WRITE setSecurity NOTIFY securityChanged)

    /** @property bool gateway
     *  @brief Enables connection through a remote gateway.
     */
    Q_PROPERTY(bool gateway READ gateway WRITE setGateway NOTIFY gatewayChanged)

    /** @property QString gatewayIp
     *  @brief IP or hostname of the gateway server.
     */
    Q_PROPERTY(QString gatewayIp READ gatewayIp WRITE setGatewayIp NOTIFY gatewayIpChanged)

    /** @property QString gatewayUserName
     *  @brief Username for gateway authentication.
     */
    Q_PROPERTY(QString gatewayUserName READ gatewayUserName WRITE setGatewayUserName NOTIFY
                   gatewayUserNameChanged)

    /** @property QString gatewayPassword
     *  @brief Password for gateway authentication.
     */
    Q_PROPERTY(QString gatewayPassword READ gatewayPassword WRITE setGatewayPassword NOTIFY
                   gatewayPasswordChanged)

    /** @property bool autoConnect
     *  @brief Automatically connect on application startup.
     */
    Q_PROPERTY(bool autoConnect READ autoConnect WRITE setAutoConnect NOTIFY autoConnectChanged)

    Q_PROPERTY(bool useAVC READ useAVC WRITE setUseAVC NOTIFY useAVCChanged FINAL)
    Q_PROPERTY(bool enableAnimation READ enableAnimation WRITE setEnableAnimation NOTIFY enableAnimationChanged FINAL)
    Q_PROPERTY(bool enableGDI READ enableGDI WRITE setEnableGDI NOTIFY enableGDIChanged FINAL)

public:
    /**
     * @brief Constructs a new ConnectionInfo object.
     * @param parent Parent QObject.
     */
    explicit ConnectionInfo(QObject *parent = nullptr);

    /* ============================================================
     *  Getters
     * ============================================================ */
    QString connectionId() const;
    QString connectionName() const;
    QString serverIp() const;
    QString deviceName() const;
    QString userName() const;
    QString password() const;
    QString performance() const;

    bool enableAudio() const;
    bool enableMicrophone() const;
    bool redirectDrive() const;
    bool redirectUsbDevice() const;
    bool security() const;
    bool gateway() const;

    QString gatewayIp() const;
    QString gatewayUserName() const;
    QString gatewayPassword() const;

    bool autoConnect() const;

    /* ============================================================
     *  Setters
     * ============================================================ */
    void setConnectionId(const QString &value);
    void setConnectionName(const QString &value);
    void setServerIp(const QString &value);
    void setDeviceName(const QString &value);
    void setUserName(const QString &value);
    void setPassword(const QString &value);
    void setPerformance(const QString &value);

    void setEnableAudio(bool value);
    void setEnableMicrophone(bool value);
    void setRedirectDrive(bool value);
    void setRedirectUsbDevice(bool value);
    void setSecurity(bool value);
    void setGateway(bool value);

    void setGatewayIp(const QString &value);
    void setGatewayUserName(const QString &value);
    void setGatewayPassword(const QString &value);

    void setAutoConnect(bool value);

    bool useAVC() const;
    bool enableAnimation() const;
    bool enableGDI() const;

    void setUseAVC(bool value);
    void setEnableAnimation(bool value);
    void setEnableGDI(bool value);

signals:
    void connectionIdChanged();
    void connectionNameChanged();
    void serverIpChanged();
    void deviceNameChanged();
    void userNameChanged();
    void passwordChanged();
    void performanceChanged();

    void enableAudioChanged();
    void enableMicrophoneChanged();
    void redirectDriveChanged();
    void redirectUsbDeviceChanged();
    void securityChanged();
    void gatewayChanged();

    void gatewayIpChanged();
    void gatewayUserNameChanged();
    void gatewayPasswordChanged();

    void autoConnectChanged();

    void useAVCChanged();

    void enableAnimationChanged();

    void enableGDIChanged();

private:
    /* ============================================================
     *  Internal storage
     * ============================================================ */
    QString m_connectionId;
    QString m_connectionName;
    QString m_serverIp;
    QString m_deviceName;
    QString m_userName;
    QString m_password;
    QString m_performance;

    bool m_enableAudio = false;
    bool m_enableMicrophone = false;
    bool m_redirectDrive = false;
    bool m_redirectUsbDevice = false;
    bool m_security = false;
    bool m_gateway = false;

    QString m_gatewayIp;
    QString m_gatewayUserName;
    QString m_gatewayPassword;

    bool m_autoConnect = false;
    bool m_useAVC = true;
    bool m_enableAnimation = false;
    bool m_enableGDI = false;
};
