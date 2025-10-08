#ifndef SERVERINFOCOLL_H
#define SERVERINFOCOLL_H

#include <QObject>
#include <deque>
#include <memory>
#include "qabstractitemmodel.h"

class ServerInfo;
class ServerInfoColl : public QAbstractListModel
{
    Q_OBJECT

    std::deque<std::shared_ptr<ServerInfo>> m_ServerInfoColl;
public:
    enum ServerInfoCollRole {
        eServerInfoCollectionRole = Qt::UserRole + 1
    };
    Q_ENUM(ServerInfoCollRole)

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &refModelIndex = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const override;

    int getIndexToBeRemoved(QString connectionName, QString serverIp);
    void setAutoConnect(QString connectionName, QString serverIp);
    std::pair<QString, QString> checkAutoConnect();

    explicit ServerInfoColl(QObject *parent = nullptr);

    Q_INVOKABLE void setServerInfo(QString connectionName, QString serverIp);
    Q_INVOKABLE void removeConnection(QString connectionName, QString serverIp);
    Q_INVOKABLE void resetAutoConnect();
    Q_INVOKABLE void connectRdServer(const QString &server, const QString &username, const QString &password);

signals:

private:
    bool _already_running{false};
    void startRdpMonitor();
    void launchRDPSequence(const QString &server, const QString &username, const QString &password);
};

#endif // SERVERINFOCOLL_H
