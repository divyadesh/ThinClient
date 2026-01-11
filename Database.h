#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QString>


struct ServerInfoStruct
{
    // Identification
    QString id;
    QString name;
    QString ip;
    QString deviceName;

    // Credentials
    QString username;
    QString password;

    // Performance / display profile
    QString performance;

    // Redirection options
    bool audio        = false;
    bool mic          = false;
    bool redirectDrive= false;
    bool redirectUsb  = false;

    // Security
    bool security     = false;

    // Gateway
    bool gateway      = false;
    QString gatewayIp;
    QString gatewayUser;
    QString gatewayPass;

    // Connection behavior
    bool autoConnect  = false;
    bool useAvc = false;
    bool animationEnabled = false;
    bool gdiHwEnabled = false;
};

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
    static DataBase* instance(QObject *parent = nullptr);

    // --- Core Database Operations ---
    bool open();
    void close();
    bool createTable();
    bool resetDatabase();

    // --- Accessors ---
    QStringList insertIntoValues() const;
    QStringList queryResultList() const;

    QString getPath();

    void setInsertIntoValues(QStringList newInsertIntoValues);
    void setQueryResultList(QStringList newQueryResultList);

    // --- Public Methods Accessible from QML ---
    Q_INVOKABLE ServerInfoStruct qmlQueryServerTable(const QString &connectionId);
    Q_INVOKABLE bool removeServer(const QString &connectionId);
    Q_INVOKABLE void qmlInsertWifiData();
    Q_INVOKABLE void qmlInsertServerData();
    Q_INVOKABLE void qmlUpdateServerData(const QString &connectionId);
    Q_INVOKABLE bool serverExists(const QString &connectionName, const QString &serverIp);
    Q_INVOKABLE bool serverExists(const QString &connectionId);

    /**
     * @brief Retrieves all server entries and populates a ServerInfoColl object.
     * @param serverInfoColl Reference to ServerInfoColl where results are stored.
     */
    void getServerList(ServerInfoColl *serverInfoColl);

signals:
    void sigInsertIntoValuesChanged();
    void sigQueryResultListChanged();
    void refreshTable();

private:
    // --- Constructor (Private for Singleton) ---
    explicit DataBase(QObject *parent = nullptr,
                      const QString &path = QString());

    // --- Member Variables ---
    static DataBase *s_instance;
    QSqlDatabase db;                ///< SQLite database connection
    QStringList m_insertIntoValues; ///< Stores temporary insert/update values
    QStringList m_queryResultList;  ///< Stores results from last executed query
    QString m_path;
};

#endif // DATABASE_H
