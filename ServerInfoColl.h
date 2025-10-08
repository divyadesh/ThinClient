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
    Q_INVOKABLE void setServerInfo(QString connectionName, QString serverIp);
    Q_INVOKABLE void removeConnection(QString connectionName, QString serverIp);
    Q_INVOKABLE void resetAutoConnect();
    Q_INVOKABLE void connectRdServer(const QString &serverIp, const QString &connectionName);

    // Helpers
    int getIndexToBeRemoved(QString connectionName, QString serverIp);
    void setAutoConnect(QString connectionName, QString serverIp);
    std::pair<QString, QString> checkAutoConnect();

signals:
    void rdpSessionStarted();
    void rdpSessionFinished(bool success);

private slots:
    void onRdpFinished();

private:
    void launchRDPSequence(const QString &server, const QString &username, const QString &password);

private:
    std::deque<std::shared_ptr<ServerInfo>> m_ServerInfoColl;
    DataBase *_database{nullptr};

    std::atomic_bool _already_running{false};
    QFutureWatcher<void> _rdpWatcher;
};

#endif // SERVERINFOCOLL_H
