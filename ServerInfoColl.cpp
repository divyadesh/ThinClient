#include "ServerInfoColl.h"
#include "ServerInfo.h"
#include <QQmlEngine>
#include <QtConcurrent/QtConcurrent>
#include <QProcess>
#include <QDebug>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(lcServerInfo, "app.serverinfo")

ServerInfoColl::ServerInfoColl(QObject *parent)
    : QAbstractListModel{parent},
    _database(&DataBase::getInstance(nullptr))
{
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

    QStringList info = _database->qmlQueryServerTable(connectionId);
    qCInfo(lcServerInfo) << "Queried server info for" << connectionId << ":" << info;

    if (info.size() <= 5) {
        qCWarning(lcServerInfo) << "Insufficient data from qmlQueryServerTable — aborting.";
        return;
    }

    const QString server   = info.at(1).trimmed();
    const QString username = info.at(3).trimmed();
    const QString password = info.at(4); // do not log this

    _already_running.store(true);
    emit rdpSessionStarted();

    QFuture<void> future = QtConcurrent::run([this, server, username, password]() {
        launchRDPSequence(server, username, password);
    });

    _rdpWatcher.setFuture(future);
}

void ServerInfoColl::launchRDPSequence(const QString &server, const QString &username, const QString &password)
{
    QString script = "/usr/bin/run_rdp.sh";

    // Run the RDP launcher as an independent systemd scope
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
