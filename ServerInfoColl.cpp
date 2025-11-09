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
//need to impement in better way
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

// === RDP logic ===

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

    const QString server   = info.ip.trimmed();
    const QString username = info.username.trimmed();
    const QString password = info.password.trimmed();

    _already_running.store(true);
    emit rdpSessionStarted();

    QFuture<void> future = QtConcurrent::run([this, server, username, password]() {
        launchRDPSequence(server, username, password);
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

void ServerInfoColl::launchRDPSequence(const QString &server, const QString &username, const QString &password)
{
    QString script = "/usr/bin/run_rdp.sh";

    // Run the RDP launcher as an independent systemd scope
    SystemSettings settings = Application::persistData()->systemSettings();
    QString freerdpArgs = buildFreerdpParams(settings);

    qDebug()<<"Display and Device Settings Parameter :: "<<freerdpArgs;

    QStringList args;
    args << "--scope"
         << "--slice=rdp"
         << script
         << server
         << username
         << password;

    QProcess *process = new QProcess(this);
    process->setProgram("systemd-run");
    process->setArguments(args);

    qInfo() << "Starting RDP launcher via systemd-run:" << args.join(" ");

    // Start detached so it won’t be killed when this Qt process stops
    bool started = process->startDetached();

    if (!started) {
        qWarning() << "Failed to start RDP launcher using systemd-run.";
        delete process;
        return;
    }

    qInfo() << "RDP launcher started successfully in separate scope.";
}

void ServerInfoColl::onRdpFinished()
{
    _already_running.store(false);
    emit rdpSessionFinished(true);
    qCInfo(lcServerInfo) << "RDP session completed and flag reset.";
}
