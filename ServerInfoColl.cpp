#include "ServerInfoColl.h"
#include "ServerInfo.h"
#include "Application.h"
#include "PersistData.h"
#include <QQmlEngine>
#include <QtConcurrent/QtConcurrent>
#include <QProcess>
#include <QDebug>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(lcServerInfo, "app.serverinfo")

ServerInfoColl::ServerInfoColl(QObject *parent)
    : QAbstractListModel{parent}
{
    _database = DataBase::instance(parent);
    connect(&_rdpWatcher, &QFutureWatcher<void>::finished, this, &ServerInfoColl::onRdpFinished);
}

// === QAbstractListModel implementation ===

QHash<int, QByteArray> ServerInfoColl::roleNames() const
{
    return {{eServerInfoCollectionRole, "serverInformation"}};
}

int ServerInfoColl::rowCount(const QModelIndex & /*parent*/) const
{
    return static_cast<int>(m_ServerInfoColl.size());
}

QVariant ServerInfoColl::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_ServerInfoColl.size()))
        return {};

    if (role == eServerInfoCollectionRole)
        return QVariant::fromValue(static_cast<QObject*>(m_ServerInfoColl.at(index.row()).get()));

    return {};
}

// === Core logic ===

int ServerInfoColl::getIndexToBeRemoved(const QString &connectionId)
{
    for (int i = 0; i < static_cast<int>(m_ServerInfoColl.size()); ++i) {
        const auto &info = m_ServerInfoColl.at(i);
        if (info->connectionId() == connectionId)
            return i;
    }
    return -1;
}

void ServerInfoColl::setAutoConnect(const QString &connectionId)
{
}

void ServerInfoColl::setServerInfo(const QString &connectionId)
{
    beginInsertRows(QModelIndex{}, rowCount(), rowCount());
    auto spServerInfo = std::make_shared<ServerInfo>(connectionId, this);
    QQmlEngine::setObjectOwnership(spServerInfo.get(), QQmlEngine::CppOwnership);
    m_ServerInfoColl.emplace_back(spServerInfo);
    endInsertRows();
}

void ServerInfoColl::removeConnection(const QString &connectionId)
{
    int index = getIndexToBeRemoved(connectionId);
    if (index < 0) return;

    beginRemoveRows(QModelIndex(), index, index);
    m_ServerInfoColl.erase(m_ServerInfoColl.begin() + index);
    endRemoveRows();
}

void ServerInfoColl::resetAutoConnect()
{
}

void ServerInfoColl::connectRdServer(const QString &connectionId)
{
    if (_already_running) {
        qCInfo(lcServerInfo) << "RDP session already running — skipping new request.";
        return;
    }

    if (!_database) {
        qCWarning(lcServerInfo) << "Database instance is null — cannot query.";
        return;
    }

    ServerInfoStruct info = _database->qmlQueryServerTable(connectionId);
    qCInfo(lcServerInfo) << "Queried server info for" << connectionId << ":" << info.deviceName;

    if (info.ip.isEmpty()) {
        qCWarning(lcServerInfo) << "Insufficient data from qmlQueryServerTable — aborting.";
        return;
    }

    _already_running.store(true);
    emit rdpSessionStarted();

    QFuture<void> future = QtConcurrent::run([this, info]() {
        startRdp(info);
    });

    _rdpWatcher.setFuture(future);
}

QString buildFreerdpParams(const SystemSettings &settings)
{
    QStringList params;

    // --- AUDIO ---
    switch (settings.audio.toInt()) {
    case 0: // Jack (analog)
        params << "/sound:sys:alsa,dev:0";
        params << "/microphone:sys:alsa,dev:0";
        break;
    case 1: // USB
        params << "/sound:sys:alsa,dev:1";
        params << "/microphone:sys:alsa,dev:1";
        break;
    case 2: // HDMI
        params << "/sound:sys:alsa,dev:2";
        params << "/microphone:sys:alsa,dev:2";
        break;
    default:
        // fallback if something is wrong
        params << "-sound";
        break;
    }

    // --- RESOLUTION ---
    if (!settings.resolution.isEmpty() && settings.resolution != "Auto")
        params << QString("/size:%1").arg(settings.resolution);
    // if "Auto", skip — FreeRDP uses default display resolution automatically

    // --- ORIENTATION ---
    // Always pass orientation (since 0 = Landscape is valid)
    params << QString("/orientation:%1").arg(settings.orientation);

    // --- TOUCH + OSK ---
    if (settings.enableOnScreenKeyboard)
        params << "+osk"; // or your custom flag
    if (settings.enableTouchScreen)
        params << "+multitouch";

    // --- DEVICE / DISPLAY OFF ---
    // Only add if > 0 (meaning enabled for X mins/hours)
    if (settings.deviceOff > 0)
        params << QString("/device-off:%1").arg(settings.deviceOff);
    if (settings.displayOff > 0)
        params << QString("/display-off:%1").arg(settings.displayOff);

    // --- TIMEZONE ---
    if (!settings.timeZone.isEmpty())
        params << QString("/tz:%1").arg(settings.timeZone);

    // --- DEFAULT FIXED PARAMS ---
    params << "+clipboard";
    params << "/drive:home,/home/user";
    params << "/printer";

    return params.join(" ");
}

void ServerInfoColl::onRdpFinished()
{
    _already_running.store(false);
    emit rdpSessionFinished(true);
    qCInfo(lcServerInfo) << "RDP session completed and flag reset.";
}

void ServerInfoColl::startRdp(ServerInfoStruct info)
{
    // ----------- Parse Struct Values -----------
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

    // ----------- Logging (Professional) -----------
    qInfo() << "-------------------------------------------";
    qInfo() << "[RDP] Starting RDP Connection for device name : " << deviceName;
    qInfo() << " ID:              " << id;
    qInfo() << " Name:            " << name;
    qInfo() << " Server IP:       " << server;
    qInfo() << " Username:        " << username;
    qInfo() << " Performance:     " << performance;
    qInfo() << " Audio:           " << audio;
    qInfo() << " Microphone:      " << mic;
    qInfo() << " Drive Redirect:  " << redirectDrive;
    qInfo() << " USB Redirect:    " << redirectUsb;
    qInfo() << " Security (NLA):  " << securityNla;
    qInfo() << " Gateway Enabled: " << useGateway;

    if (useGateway) {
        qInfo() << " Gateway IP:     " << gatewayIp;
        qInfo() << " Gateway User:   " << gatewayUser;
    }
    qInfo() << "-------------------------------------------";

    // ----------- Build FreeRDP Command -----------
    QString cmd = "wlfreerdp";
    QStringList args;

    // Base flags
    args << "/f"
         << "/bpp:32"
         << "/cert-ignore"
         << "+auto-reconnect"
         << "/auto-reconnect-max-retries:6";

    // Security
    if (securityNla)
        args << "/sec:nla";
    else
        args << "-sec-nla";

    // Performance mode
    if (performance.compare("best", Qt::CaseInsensitive) == 0) {
        args << "/network:lan";
        args << "+bitmap-cache";
        args << "+offscreen-cache";
        args << "+glyph-cache";
    } else {
        args << "/network:auto";
    }

    // Graphics (optimized)
    args << "/gfx:avc420"
         << "+gfx-progressive"
         << "+gfx-thin-client"
         << "+gfx-small-cache"
         << "+fast-path";

    // ---- Audio (Optimized with Large Buffer) ----
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

    // ----------- Print Final Command -----------
    qInfo() << "[RDP] Final Command:";
    qInfo() << "wlfreerdp" << args.join(" ");
    qInfo() << "-------------------------------------------";

    // Environment setup
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("XDG_RUNTIME_DIR", "/run/user/0");
    env.insert("WAYLAND_DISPLAY", "wayland-0");
    env.insert("QT_QPA_PLATFORM", "wayland-egl");
    env.insert("QT_QUICK_BACKEND", "software");

    // Start RDP
    bool ok = QProcess::startDetached(cmd, args, QString(), nullptr);

    if (!ok)
        qWarning() << "[RDP] Failed to start wlfreerdp!";
    else
        qInfo() << "[RDP] RDP started successfully.";
}
