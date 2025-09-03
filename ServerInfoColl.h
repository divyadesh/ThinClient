#ifndef SERVERINFOCOLL_H
#define SERVERINFOCOLL_H

#include <QObject>
#include <deque>
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

    explicit ServerInfoColl(QObject *parent = nullptr);

    Q_INVOKABLE void setServerInfo(QString connectionName, QString serverIp);

signals:
};

#endif // SERVERINFOCOLL_H
