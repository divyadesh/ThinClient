#include "ServerInfoColl.h"
#include "ServerInfo.h"
#include <QQmlEngine>
#include <memory>
#include <QProcess>
#include <QString>
#include <QThread>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>
#include "RdpMonitorThread.h"

ServerInfoColl::ServerInfoColl(QObject *parent)
    : QAbstractListModel{parent}
{}

QHash<int, QByteArray> ServerInfoColl::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[eServerInfoCollectionRole] = "serverInformation";
    return roles;
}

int ServerInfoColl::rowCount([[maybe_unused]] const QModelIndex &refModelIndex) const {
    return static_cast<int>(m_ServerInfoColl.size());
}

QVariant ServerInfoColl::data(const QModelIndex &index, int role) const {
    if((int)index.row() >= 0 && (std::size_t)index.row() < m_ServerInfoColl.size()) {
        switch(role) {
        case eServerInfoCollectionRole: {
            return QVariant::fromValue(static_cast<QObject*>(m_ServerInfoColl.at(index.row()).get()));
        }
        }
    }
    return QVariant {};
}

int ServerInfoColl::getIndexToBeRemoved(QString connectionName, QString serverIp) {
    int index = -1;
    for(const auto& spServerInfo: m_ServerInfoColl) {
        ++index;
        if(spServerInfo->connectionName() == connectionName && spServerInfo->serverIp() == serverIp)
            return index;
    }
    return -1;
}

void ServerInfoColl::setAutoConnect(QString connectionName, QString serverIp) {
    for(auto &serverInfo: m_ServerInfoColl) {
        if(serverInfo->connectionName() == connectionName && serverInfo->serverIp() == serverIp) {
            serverInfo->setAutoEnable(true);
            break;
        }
    }
}

std::pair<QString, QString> ServerInfoColl::checkAutoConnect() {
    for(auto &serverInfo: m_ServerInfoColl) {
        if(serverInfo->autoEnable())
            return {serverInfo->connectionName(), serverInfo->serverIp()};
    }
    return {"", ""};
}

void ServerInfoColl::setServerInfo(QString connectionName, QString serverIp) {
    beginInsertRows(QModelIndex{}, static_cast<int>(m_ServerInfoColl.size()), static_cast<int>(m_ServerInfoColl.size()));
    std::shared_ptr<ServerInfo> spServerInfo = std::make_shared<ServerInfo>(this, connectionName, serverIp);
    if(spServerInfo) {
        QQmlEngine::setObjectOwnership(spServerInfo.get(), QQmlEngine::CppOwnership);
        m_ServerInfoColl.emplace_back(spServerInfo);
    }
    endInsertRows();
}

void ServerInfoColl::removeConnection(QString connectionName, QString serverIp) {
    int index = getIndexToBeRemoved(connectionName, serverIp);
    if (index < 0 || index >= static_cast<int>(m_ServerInfoColl.size()))
        return;

    beginRemoveRows(QModelIndex(), index, index);
    m_ServerInfoColl.erase(m_ServerInfoColl.begin() + index);
    endRemoveRows();
}

void ServerInfoColl::launchRDPSequence(const QString &server, const QString &username, const QString &password)
{
    if(_already_running) {
        return;
    }
    QString script = "/usr/bin/run_rdp.sh";

    // Use QProcess instead of system() for proper argument escaping
    QProcess process;
    QStringList args;
    args << server << username << password;

    qInfo() << "Starting RDP script:" << script << args;

    // Run the script synchronously in the background thread
    process.start(script, args);
    _already_running = true;
    process.waitForFinished(-1);  // Wait indefinitely until RDP session ends

    int ret = process.exitCode();
    if (ret != 0) {
        qWarning() << "RDP script exited with error code:" << ret;
    } else {
        qInfo() << "RDP session finished successfully.";
    }
    _already_running = false;
}

void ServerInfoColl::startRdpMonitor()
{
    RdpMonitorThread *monitor = new RdpMonitorThread(this);
    monitor->start(QThread::LowestPriority);
}


void ServerInfoColl::resetAutoConnect() {
    for(auto& serverInfo: m_ServerInfoColl) {
        serverInfo->setAutoEnable(false);
    }
}

void ServerInfoColl::connectRdServer(const QString &server, const QString &username, const QString &password)
{
    // QString server = "183.83.196.74:5566";
    // QString username = "u1";
    // QString password = "g1@123";

    if(_already_running) {
        return;
    }

    // Launch the RDP sequence in the background
    QtConcurrent::run([this, server, username, password]() {
        launchRDPSequence(server, username, password);
    });
}
