#ifndef SERVERINFOCOLL_H
#define SERVERINFOCOLL_H

#include <QObject>
#include <QAbstractListModel>
#include <QProcess>
#include <QFutureWatcher>
#include <atomic>
#include <deque>
#include <memory>
#include "PersistData.h"
#include "Application.h"
#include "Database.h"

class ServerInfo;

/**
 * @class ServerInfoColl
 * @brief Model + controller that stores server connections and manages RDP sessions.
 *
 * This class exposes:
 *  - A QAbstractListModel for QML UI listing of configured servers.
 *  - RDP session control using FreeRDP (wlfreerdp).
 *  - Full real-time connection monitoring via stdout/stderr parsing.
 *  - Thread-safe session management preventing duplicates.
 */
class ServerInfoColl : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ServerInfoColl(QObject *parent = nullptr);

    /** @brief Roles for QAbstractListModel */
    enum ServerInfoCollRole {
        eServerInfoCollectionRole = Qt::UserRole + 1
    };
    Q_ENUM(ServerInfoCollRole)

    // --------- MODEL OVERRIDES ----------
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;

    // --------- API EXPOSED TO QML ----------
    Q_INVOKABLE void setServerInfo(const QString &connectionId);
    Q_INVOKABLE void removeConnection(const QString &connectionId);
    Q_INVOKABLE void resetAutoConnect();
    Q_INVOKABLE void connectRdServer(const QString &connectionId);
    Q_INVOKABLE void setAutoConnect(const QString &connectionId);

signals:

    /** @brief Emitted when RDP connection starts */
    void rdpSessionStarted(const QString &id);

    /** @brief Emitted when connected successfully */
    void rdpConnected(const QString &id);

    /** @brief Emitted when RDP fails with reason */
    void rdpConnectionFailed(const QString &id, const QString &reason);

    /** @brief Emitted when process exits */
    void rdpDisconnected(const QString &id);

private slots:
    void onRdpStarted();
    void onRdpError(QProcess::ProcessError error);
    void onRdpFinished(int exitCode, QProcess::ExitStatus status);
    void onRdpStdOut();
    void onRdpStdErr();

private:
    int getIndexToBeRemoved(const QString &connectionId);
    void startRdp(const ServerInfoStruct &info);
    void cleanupProcess();

private:
    std::deque<std::shared_ptr<ServerInfo>> m_ServerInfoColl;
    DataBase *m_database = nullptr;

    std::atomic_bool m_running { false };
    QString m_connectionId;

    QProcess *m_rdp = nullptr;
};

#endif // SERVERINFOCOLL_H
