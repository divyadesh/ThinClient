#ifndef SERVERINFOCOLL_H
#define SERVERINFOCOLL_H

#include <QObject>
#include <QAbstractListModel>
#include <QFutureWatcher>
#include <atomic>
#include <deque>
#include <memory>

#include "Database.h"

class ServerInfo;

class ServerInfoColl : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ServerInfoColl(QObject *parent = nullptr);

    enum ServerInfoCollRole {
        eServerInfoCollectionRole = Qt::UserRole + 1
    };
    Q_ENUM(ServerInfoCollRole)

    // QAbstractListModel overrides
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Exposed QML methods
    Q_INVOKABLE void setServerInfo(const QString &connectionId);
    Q_INVOKABLE void removeConnection(const QString &connectionId);
    Q_INVOKABLE void resetAutoConnect();
    Q_INVOKABLE void connectRdServer(const QString &connectionId);

    // Helpers
    int getIndexToBeRemoved(const QString &connectionId);
    void setAutoConnect(const QString &connectionId);

signals:
    void rdpSessionStarted();
    void rdpSessionFinished(bool success);

private slots:
    void onRdpFinished();
    void startRdp(ServerInfoStruct info);

private:
    void launchRDPSequence(const QString &server, const QString &username, const QString &password);

private:
    std::deque<std::shared_ptr<ServerInfo>> m_ServerInfoColl;
    DataBase *_database{nullptr};

    std::atomic_bool _already_running{false};
    QFutureWatcher<void> _rdpWatcher;
};

#endif // SERVERINFOCOLL_H
