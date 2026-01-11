#include "ServerInfoColl.h"
#include "ServerInfo.h"
#include <QQmlEngine>
#include <QStorageInfo>
#include <QStringList>
#include <QLoggingCategory>
#include <QPair>

extern "C" {
    #include <libudev.h>
}


Q_LOGGING_CATEGORY(lcServerInfo, "app.serverinfo")

// ------------------------------------------------------
//  Constructor
// ------------------------------------------------------
ServerInfoColl::ServerInfoColl(QObject *parent)
    : QAbstractListModel(parent)
{
    m_database = DataBase::instance(parent);
}

// ------------------------------------------------------
//  QAbstractListModel Implementation
// ------------------------------------------------------
QHash<int, QByteArray> ServerInfoColl::roleNames() const
{
    return {
        { eServerInfoCollectionRole, "serverInformation" }
    };
}

int ServerInfoColl::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return static_cast<int>(m_ServerInfoColl.size());
}

QVariant ServerInfoColl::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()
        || index.row() < 0
        || index.row() >= (int)m_ServerInfoColl.size())
        return {};

    if (role == eServerInfoCollectionRole)
        return QVariant::fromValue(
            static_cast<QObject*>(m_ServerInfoColl.at(index.row()).get()));

    return {};
}

// ------------------------------------------------------
//  Helper — find index by ID
// ------------------------------------------------------
int ServerInfoColl::getIndexToBeRemoved(const QString &connectionId)
{
    for (int i = 0; i < (int)m_ServerInfoColl.size(); ++i) {
        if (m_ServerInfoColl[i]->connectionId() == connectionId)
            return i;
    }
    return -1;
}

// ------------------------------------------------------
//  Add server entry to model
// ------------------------------------------------------
void ServerInfoColl::setServerInfo(const QString &connectionId)
{
    beginInsertRows({}, rowCount(), rowCount());
    auto item = std::make_shared<ServerInfo>(connectionId, this);
    QQmlEngine::setObjectOwnership(item.get(), QQmlEngine::CppOwnership);
    m_ServerInfoColl.emplace_back(item);
    endInsertRows();
}

// ------------------------------------------------------
//  Remove server
// ------------------------------------------------------
void ServerInfoColl::removeConnection(const QString &connectionId)
{
    int idx = getIndexToBeRemoved(connectionId);
    if (idx < 0) return;

    beginRemoveRows({}, idx, idx);
    m_ServerInfoColl.erase(m_ServerInfoColl.begin() + idx);
    endRemoveRows();
}

// ------------------------------------------------------
//  Auto-connect placeholder
// ------------------------------------------------------
void ServerInfoColl::setAutoConnect(const QString &) {}
void ServerInfoColl::resetAutoConnect() {}


// ------------------------------------------------------
//  Main entry: start RDP session
// ------------------------------------------------------
void ServerInfoColl::connectRdServer(const QString &connectionId)
{
    if (m_running) {
        qInfo(lcServerInfo) << "RDP already running";
        emit rdpConnectionFailed(connectionId, "Session already active");
        return;
    }

    if (!m_database) {
        emit rdpConnectionFailed(connectionId, "Database unavailable");
        return;
    }

    ServerInfoStruct info = m_database->qmlQueryServerTable(connectionId);

    if (info.ip.isEmpty()) {
        emit rdpConnectionFailed(connectionId, "Invalid server data");
        return;
    }

    m_running = true;
    m_connectionId = connectionId;

    emit rdpSessionStarted(connectionId);

    buildFreeRdpArguments(info);
    // startRdp(info);
}

// ------------------------------------------------------
//  Cleanup process safely
// ------------------------------------------------------
void ServerInfoColl::cleanupProcess()
{
    if (m_rdp) {
        m_rdp->deleteLater();
        m_rdp = nullptr;
    }
    m_running = false;
}

QStringList ServerInfoColl::buildGraphicsArgs(bool useAvc444,
                                              bool animationEnabled,
                                              bool gdiHwEnabled) const
{
    QStringList args;

    // Codec selection (mutually exclusive)
    if (!useAvc444) {
        args << "/gfx:rfx";
    } else {
        args << "/gfx-h264:AVC444";
    }

    // Animation options
    if (animationEnabled) {
        args << "+menu-anims"
             << "+window-drag";
    }

    // Hardware GDI
    if (gdiHwEnabled) {
        args << "/gdi:hw";
    }

    return args;
}

/* ===================== Audio ===================== */

QStringList ServerInfoColl::buildAudioOutputArgs(bool enabled) const
{
    if (!enabled)
        return {};

    return {
        "/sound:sys:alsa,format:1,quality:high,rate:44100,latency:700,channel:2"
    };
}

QStringList ServerInfoColl::buildMicrophoneArgs(bool enabled) const
{
    if (!enabled)
        return {};

    return {
        "/microphone:sys:alsa,format:1,quality:high"
    };
}

/* ===================== Drive Redirection ===================== */

QStringList ServerInfoColl::buildDriveRedirectionArgs(const QString& shareName,
                                                           const QString& path,
                                                           bool enabled) const
{
    if (!enabled || shareName.isEmpty() || path.isEmpty())
        return {};

    return { QString("/drive:%1,%2").arg(shareName, path) };
}

QPair<QString, QString> ServerInfoColl::resolveDriveShare() const
{
    const auto volumes = QStorageInfo::mountedVolumes();

    for (const QStorageInfo& volume : volumes) {

        if (!volume.isValid() || !volume.isReady())
            continue;

        const QString mountPath = volume.rootPath();
        if (mountPath.isEmpty())
            continue;

        // Yocto-safe removable detection
        if (!mountPath.startsWith("/run/media") &&
            !mountPath.startsWith("/media") &&
            !mountPath.startsWith("/mnt"))
            continue;

        QString shareName = volume.displayName().trimmed();
        if (shareName.isEmpty())
            shareName = QStringLiteral("G1rdclient");

        qInfo() << "[FreeRDP] Drive redirection:"
                << "shareName=" << shareName
                << "mountPath=" << mountPath;

        return qMakePair(shareName, mountPath);
    }

    qInfo() << "[FreeRDP] No removable drive detected";
    return {};
}

/* ===================== USB Redirection ===================== */

QStringList ServerInfoColl::buildUsbRedirectionArgs(const QString& vendorId,
                                                         const QString& productId,
                                                         bool enabled) const
{
    if (!enabled || vendorId.isEmpty() || productId.isEmpty())
        return {};

    return {
        QString("/usb:id,dev:%1:%2").arg(vendorId, productId)
    };
}

QStringList ServerInfoColl::buildSecurityArgs(bool enabled) const
{
    if (!enabled)
        return {};

    return { "/sec:nla" };
}

QPair<QString, QString> ServerInfoColl::resolveUsbDeviceIds() const
{
    struct udev* udev = udev_new();
    if (!udev)
        return {};

    struct udev_enumerate* enumerate = udev_enumerate_new(udev);
    if (!enumerate) {
        udev_unref(udev);
        return {};
    }

    udev_enumerate_add_match_subsystem(enumerate, "usb");
    udev_enumerate_scan_devices(enumerate);

    struct udev_list_entry* entry =
        udev_enumerate_get_list_entry(enumerate);

    for (; entry; entry = udev_list_entry_get_next(entry)) {

        const char* syspath = udev_list_entry_get_name(entry);
        if (!syspath)
            continue;

        struct udev_device* dev =
            udev_device_new_from_syspath(udev, syspath);
        if (!dev)
            continue;

        struct udev_device* usbDev =
            udev_device_get_parent_with_subsystem_devtype(
                dev, "usb", "usb_device");

        if (!usbDev) {
            udev_device_unref(dev);
            continue;
        }

        const char* vendor =
            udev_device_get_sysattr_value(usbDev, "idVendor");
        const char* product =
            udev_device_get_sysattr_value(usbDev, "idProduct");

        if (!vendor || !product) {
            udev_device_unref(dev);
            continue;
        }

        /* ---- FILTER ROOT HUBS ---- */
        if (strcmp(vendor, "1d6b") == 0) {
            // Linux Foundation USB root hub
            udev_device_unref(dev);
            continue;
        }

        /* ---- FILTER USB HUB CLASS ---- */
        const char* devClass =
            udev_device_get_sysattr_value(usbDev, "bDeviceClass");

        if (devClass && strcmp(devClass, "09") == 0) {
            // USB hub
            udev_device_unref(dev);
            continue;
        }

        QPair<QString, QString> ids{
            QString::fromLatin1(vendor),
            QString::fromLatin1(product)
        };

        qInfo() << "[FreeRDP] USB redirection device detected:"
                << "vendor=" << ids.first
                << "product=" << ids.second;

        udev_device_unref(dev);
        udev_enumerate_unref(enumerate);
        udev_unref(udev);
        return ids;
    }

    udev_enumerate_unref(enumerate);
    udev_unref(udev);

    qInfo() << "[FreeRDP] No valid USB device found for redirection";
    return {};
}

/* ===================== Aggregation ===================== */

QStringList ServerInfoColl::buildAdvancedFreeRdpArguments(const ServerInfoStruct& info)
{
    QStringList args;

    qInfo() << "[FreeRDP] Building arguments";

    const auto isBestPerformance = info.performance.compare("best", Qt::CaseInsensitive) == 0;

    Q_UNUSED(isBestPerformance);

    args << buildGraphicsArgs(info.useAvc, info.animationEnabled, info.gdiHwEnabled);
    args << buildAudioOutputArgs(info.audio);
    args << buildMicrophoneArgs(info.mic);
    args << buildSecurityArgs(info.security);

    const auto driveInfo = resolveDriveShare();
    args << buildDriveRedirectionArgs(driveInfo.first,
                                      driveInfo.second,
                                      info.redirectDrive);

    const auto usbIds = resolveUsbDeviceIds();
    args << buildUsbRedirectionArgs(usbIds.first,
                                    usbIds.second,
                                    info.redirectUsb);

    return args;
}

QStringList ServerInfoColl::buildConnectionFreeRdpArguments(const ServerInfoStruct& info) const
{
    QStringList args;

    qInfo() << "[FreeRDP] Building connection arguments";

    // Gateway configuration (optional)
    if (info.gateway) {
        qInfo() << "[FreeRDP] Gateway enabled:"
                << "ip=" << info.gatewayIp
                << "user=" << info.gatewayUser;

        if (!info.gatewayIp.isEmpty())
            args << QString("/g:%1").arg(info.gatewayIp);

        if (!info.gatewayUser.isEmpty())
            args << QString("/gu:%1").arg(info.gatewayUser);

        if (!info.gatewayPass.isEmpty())
            args << QString("/gp:%1").arg(info.gatewayPass);
    }

    // Target server
    if (!info.ip.isEmpty()) {
        args << QString("/v:%1").arg(info.ip);
    }

    // Credentials
    if (!info.username.isEmpty())
        args << QString("/u:%1").arg(info.username);

    if (!info.password.isEmpty())
        args << QString("/p:%1").arg(info.password);

    return args;
}

void ServerInfoColl::buildFreeRdpArguments(const ServerInfoStruct &info) {

    cleanupProcess();
    m_rdp = new QProcess(this);

    // --- Connect QProcess signals ---
    connect(m_rdp, &QProcess::started,
            this, &ServerInfoColl::onRdpStarted);

    connect(m_rdp,
            qOverload<QProcess::ProcessError>(&QProcess::errorOccurred),
            this, &ServerInfoColl::onRdpError);

    connect(m_rdp,
            qOverload<int, QProcess::ExitStatus>(&QProcess::finished),
            this, &ServerInfoColl::onRdpFinished);

    connect(m_rdp, &QProcess::readyReadStandardOutput,
            this, &ServerInfoColl::onRdpStdOut);

    connect(m_rdp, &QProcess::readyReadStandardError,
            this, &ServerInfoColl::onRdpStdErr);

    const QString id          = info.id.trimmed();
    const QString name        = info.name.trimmed();
    const QString server      = info.ip.trimmed();
    const QString deviceName  = info.deviceName.trimmed();
    const QString username    = info.username.trimmed();

    qInfo() << "-------------------------------------------";
    qInfo() << "[RDP] Starting RDP Connection for device name : " << deviceName;
    qInfo() << " ID:              " << id;
    qInfo() << " Name:            " << name;
    qInfo() << " Server IP:       " << server;
    qInfo() << " Username:        " << username;

    QStringList args;
    args << "/f"
         << "/dynamic-resolution"
         << "/bpp:32"
         << "/network:lan"
         << "/cert-ignore"
         << "/client-hostname:G1rdclient";

    args << buildConnectionFreeRdpArguments(info);
    args << buildAdvancedFreeRdpArguments(info);

    // Log final command

    qInfo() << "-------------------------------------------";
    qInfo() << "[RDP] Final Command:";
    qInfo() << "wlfreerdp" << args.join(" ");
    qInfo() << "-------------------------------------------";

    // Start FreeRDP
    m_rdp->start("wlfreerdp", args);
}

// ------------------------------------------------------
//  Start FreeRDP with QProcess
// ------------------------------------------------------
void ServerInfoColl::startRdp(const ServerInfoStruct &info)
{
    cleanupProcess();
    m_rdp = new QProcess(this);

    // --- Connect QProcess signals ---
    connect(m_rdp, &QProcess::started,
            this, &ServerInfoColl::onRdpStarted);

    connect(m_rdp,
            qOverload<QProcess::ProcessError>(&QProcess::errorOccurred),
            this, &ServerInfoColl::onRdpError);

    connect(m_rdp,
            qOverload<int, QProcess::ExitStatus>(&QProcess::finished),
            this, &ServerInfoColl::onRdpFinished);

    connect(m_rdp, &QProcess::readyReadStandardOutput,
            this, &ServerInfoColl::onRdpStdOut);

    connect(m_rdp, &QProcess::readyReadStandardError,
            this, &ServerInfoColl::onRdpStdErr);


    // ============================================
    // Parse Struct Values (same as your old code)
    // ============================================
    const QString id          = info.id.trimmed();
    const QString name        = info.name.trimmed();
    const QString server      = info.ip.trimmed();
    const QString deviceName  = info.deviceName.trimmed();
    const QString username    = info.username.trimmed();
    const QString password    = info.password.trimmed();
    const QString performance = info.performance.trimmed();

    const bool audio          = info.audio;
    const bool mic            = info.mic;
    const bool redirectDrive  = info.redirectDrive;
    const bool redirectUsb    = info.redirectUsb;
    const bool securityNla    = info.security;
    const bool useGateway     = info.gateway;

    const QString gatewayIp   = info.gatewayIp.trimmed();
    const QString gatewayUser = info.gatewayUser.trimmed();
    const QString gatewayPass = info.gatewayPass.trimmed();

    // Logs (same as your old version)
    qInfo() << "-------------------------------------------";
    qInfo() << "[RDP] Starting RDP Connection for device name : " << deviceName;
    qInfo() << " ID:              " << id;
    qInfo() << " Name:            " << name;
    qInfo() << " Server IP:       " << server;
    qInfo() << " Username:        " << username;

    // ============================================
    // Build FreeRDP Command    (same as old)
    // ============================================
    QStringList args;
    args << "/f"
         << "/bpp:32"
         << "/cert-ignore"
         << "+auto-reconnect"
         << "/auto-reconnect-max-retries:6";

    if (securityNla)
        args << "/sec:nla";
    else
        args << "-sec-nla";

    if (performance.compare("best", Qt::CaseInsensitive) == 0) {
        args << "/network:lan"
             << "+bitmap-cache"
             << "+offscreen-cache"
             << "+glyph-cache";
    } else {
        args << "/network:auto";
    }

    args << "/gfx:avc420"
         << "+gfx-progressive"
         << "+gfx-thin-client"
         << "+gfx-small-cache"
         << "+fast-path";

    if (audio)
        args << "/sound:sys:alsa,latency:450,rate:44100,channel:2";

    if (mic)
        args << "/microphone:sys:alsa";

    if (redirectDrive)
        args << "/drives";

    if (redirectUsb)
        args << "/usb:auto";

    if (useGateway) {
        args << QString("/g:%1").arg(gatewayIp);
        args << QString("/gu:%1").arg(gatewayUser);
        args << QString("/gp:%1").arg(gatewayPass);
    }

    args << QString("/v:%1").arg(server)
         << QString("/u:%1").arg(username)
         << QString("/p:%1").arg(password);

    // Log final command
    qInfo() << "[RDP] Final Command:";
    qInfo() << "wlfreerdp" << args.join(" ");
    qInfo() << "-------------------------------------------";

    // Start FreeRDP (controlled mode)
    m_rdp->start("wlfreerdp", args);
}


// ------------------------------------------------------
//  Slots: Process events
// ------------------------------------------------------
void ServerInfoColl::onRdpStarted()
{
    qInfo(lcServerInfo) << "RDP Started";
}

void ServerInfoColl::onRdpError(QProcess::ProcessError err)
{
    emit rdpConnectionFailed(m_connectionId,
                             QString("Process error %1").arg(err));
    cleanupProcess();
}

void ServerInfoColl::onRdpFinished(int exitCode,
                                   QProcess::ExitStatus status)
{
    if (exitCode == 0)
        emit rdpDisconnected(m_connectionId);
    else
        emit rdpConnectionFailed(m_connectionId,
                                 QString("Exited %1").arg(exitCode));

    cleanupProcess();
}

void ServerInfoColl::onRdpStdOut()
{
    QString out = m_rdp->readAllStandardOutput();
    qInfo(lcServerInfo) << "[STDOUT]" << out.trimmed();

    if (out.contains("connected", Qt::CaseInsensitive)) {
        emit rdpConnected(m_connectionId);
    }
}

void ServerInfoColl::onRdpStdErr()
{
    QString err = m_rdp->readAllStandardError();
    qWarning(lcServerInfo) << "[STDERR]" << err.trimmed();

    if (err.contains("Authentication", Qt::CaseInsensitive))
        emit rdpConnectionFailed(m_connectionId, "Authentication failed");

    if (err.contains("unable to connect", Qt::CaseInsensitive))
        emit rdpConnectionFailed(m_connectionId, "Unable to connect");
}
