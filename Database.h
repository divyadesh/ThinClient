#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QDebug>
#include <QObject>

class ServerInfoColl;
class DataBase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList insertIntoValues READ insertIntoValues WRITE setInsertIntoValues NOTIFY sigInsertIntoValuesChanged)
    Q_PROPERTY(QStringList queryResultList READ queryResultList WRITE setQueryResultList NOTIFY sigQueryResultListChanged FINAL)

    QSqlDatabase db;
    explicit DataBase(QObject *parent = nullptr, const QString &path="");

    QStringList m_insertIntoValues;
    QStringList m_queryResultList;

public:
    static DataBase& getInstance(QObject *parent);
    bool open();
    void close();
    bool createTable();
    QStringList insertIntoValues() const;
    void setInsertIntoValues(const QStringList &newInsertIntoValues);

    QStringList queryResultList() const;
    void setQueryResultList(const QStringList &newQueryResultList);

    Q_INVOKABLE void qmlQueryServerTable(QString connectionName, QString serverIpAddress);
    Q_INVOKABLE void removeServer(QString connectionName, QString serverIp);
    Q_INVOKABLE void qmlInsertWifiData();
    Q_INVOKABLE void qmlInsertServerData();
    Q_INVOKABLE void qmlUpdateServerData(QString connectionName, QString serverIp);

    void getServerList(ServerInfoColl & serverInfoColl);

signals:
    void sigInsertIntoValuesChanged();
    void sigQueryResultListChanged();
};

#endif // DATABASE_H
