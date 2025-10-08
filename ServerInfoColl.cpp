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

int ServerInfoColl::getIndexToBeRemoved(QString connectionName, QString serverIp)
{
    for (int i = 0; i < static_cast<int>(m_ServerInfoColl.size()); ++i) {
        const auto &info = m_ServerInfoColl.at(i);
        if (info->connectionName() == connectionName && info->serverIp() == serverIp)
            return i;
    }
    return -1;
}

void ServerInfoColl::setAutoConnect(QString connectionName, QString serverIp)
{
    for (auto &serverInfo : m_ServerInfoColl) {
        if (serverInfo->connectionName() == connectionName && serverInfo->serverIp() == serverIp) {
            serverInfo->setAutoEnable(true);
            break;
        }
    }
}

std::pair<QString, QString> ServerInfoColl::checkAutoConnect()
{
    for (auto &serverInfo : m_ServerInfoColl) {
        if (serverInfo->autoEnable())
            return {serverInfo->connectionName(), serverInfo->serverIp()};
    }
    return {"", ""};
}

void ServerInfoColl::setServerInfo(QString connectionName, QString serverIp)
{
    beginInsertRows(QModelIndex{}, rowCount(), rowCount());
    auto spServerInfo = std::make_shared<ServerInfo>(this, connectionName, serverIp);
    QQmlEngine::setObjectOwnership(spServerInfo.get(), QQmlEngine::CppOwnership);
    m_ServerInfoColl.emplace_back(spServerInfo);
    endInsertRows();
}

void ServerInfoColl::removeConnection(QString connectionName, QString serverIp)
{
    int index = getIndexToBeRemoved(connectionName, serverIp);
    if (index < 0) return;

    beginRemoveRows(QModelIndex(), index, index);
    m_ServerInfoColl.erase(m_ServerInfoColl.begin() + index);
    endRemoveRows();
}

void ServerInfoColl::resetAutoConnect()
{
    for (auto &serverInfo : m_ServerInfoColl)
        serverInfo->setAutoEnable(false);
}

// === RDP logic ===

void ServerInfoColl::connectRdServer(const QString &serverIp, const QString &connectionName)
{
    if (_already_running) {
        qCInfo(lcServerInfo) << "RDP session already running — skipping new request.";
        return;
    }

    if (!_database) {
        qCWarning(lcServerInfo) << "Database instance is null — cannot query.";
        return;
    }

    QStringList info = _database->qmlQueryServerTable(connectionName, serverIp);
    qCInfo(lcServerInfo) << "Queried server info for" << connectionName << ":" << info;

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

    QProcess process;
    QStringList args{server, username, password};

    process.start(script, args);
    if (!process.waitForStarted(5000)) {
        qCWarning(lcServerInfo) << "Failed to start RDP script.";
        return;
    }

    process.waitForFinished(-1);
    int exitCode = process.exitCode();

    if (exitCode == 0)
        qCInfo(lcServerInfo) << "RDP session finished successfully.";
    else
        qCWarning(lcServerInfo) << "RDP script exited with code:" << exitCode;
}

void ServerInfoColl::onRdpFinished()
{
    _already_running.store(false);
    emit rdpSessionFinished(true);
    qCInfo(lcServerInfo) << "RDP session completed and flag reset.";
}
