#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

/**
 * @class DataBase
 * @brief Singleton wrapper around QSqlDatabase for managing ThinClient data.
 *
 * The DataBase class provides a singleton interface to an SQLite database
 * storing server connection information and Wi-Fi credentials.
 *
 * It exposes QML-invokable methods for inserting, querying, updating,
 * and deleting records from QML UI components.
 */
class ServerInfoColl;  // Forward declaration

class DataBase : public QObject
{
    Q_OBJECT

    // --- QML Properties ---
    Q_PROPERTY(QStringList insertIntoValues READ insertIntoValues
                   WRITE setInsertIntoValues
                       NOTIFY sigInsertIntoValuesChanged)

    Q_PROPERTY(QStringList queryResultList READ queryResultList
                   WRITE setQueryResultList
                       NOTIFY sigQueryResultListChanged FINAL)

public:
    // --- Singleton Access ---
    /**
     * @brief Returns the singleton instance of the database.
     * @param parent Optional parent QObject.
     */
    static DataBase& getInstance(QObject *parent = nullptr);

    // --- Core Database Operations ---
    bool open();
    void close();
    bool createTable();

    // --- Accessors ---
    QStringList insertIntoValues() const;
    QStringList queryResultList() const;

    void setInsertIntoValues(QStringList newInsertIntoValues);
    void setQueryResultList(QStringList newQueryResultList);

    // --- Public Methods Accessible from QML ---
    Q_INVOKABLE void qmlQueryServerTable(const QString &connectionName,
                                         const QString &serverIpAddress);

    Q_INVOKABLE bool removeServer(const QString &connectionName,
                                  const QString &serverIp);

    Q_INVOKABLE void qmlInsertWifiData();
    Q_INVOKABLE void qmlInsertServerData();
    Q_INVOKABLE void qmlUpdateServerData(const QString &connectionName,
                                         const QString &serverIp);

    /**
     * @brief Retrieves all server entries and populates a ServerInfoColl object.
     * @param serverInfoColl Reference to ServerInfoColl where results are stored.
     */
    void getServerList(ServerInfoColl &serverInfoColl);

signals:
    void sigInsertIntoValuesChanged();
    void sigQueryResultListChanged();

private:
    // --- Constructor (Private for Singleton) ---
    explicit DataBase(QObject *parent = nullptr,
                      const QString &path = QString());

    // --- Member Variables ---
    QSqlDatabase db;                ///< SQLite database connection
    QStringList m_insertIntoValues; ///< Stores temporary insert/update values
    QStringList m_queryResultList;  ///< Stores results from last executed query
};

#endif // DATABASE_H
