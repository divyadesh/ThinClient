#include "Database.h"
#include "ServerInfoColl.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>

/**
 * @brief Constructs the DataBase object and initializes the SQLite connection.
 * @param parent Parent QObject (usually nullptr for singleton).
 * @param path Absolute path to the SQLite database file.
 */
DataBase::DataBase(QObject *parent, const QString &path)
    : QObject{parent}
{
    db = QSqlDatabase::addDatabase("QSQLITE", "ThinClientConnection");
    db.setDatabaseName(path);
}

/**
 * @brief Returns a singleton instance of the DataBase class.
 *        Automatically chooses the database file path depending on the OS.
 *
 * On:
 *  - Windows: C:/ProgramData/ThinClientDB.db
 *  - macOS:   /Users/Shared/ThinClientDB.db
 *  - Linux:   /home/root/ThinClientDB.db
 *  - Fallback: ThinClientDB.db (current directory)
 *
 * @param parent Optional QObject parent.
 * @return Reference to the single DataBase instance.
 */
DataBase& DataBase::getInstance(QObject *parent)
{
#ifdef Q_OS_WIN
    static DataBase m_dbInstance(parent, "C:/ProgramData/ThinClientDB.db");
#elif defined(Q_OS_MAC)
    static DataBase m_dbInstance(parent, "/Users/Shared/ThinClientDB.db");
#elif defined(Q_OS_LINUX)
    static DataBase m_dbInstance(parent, "/home/root/ThinClientDB.db");
#else
    static DataBase m_dbInstance(parent, "ThinClientDB.db"); // Fallback
#endif
    return m_dbInstance;
}

/**
 * @brief Opens the SQLite database connection.
 * @return True if successful, false otherwise.
 */
bool DataBase::open()
{
    if (!db.open()) {
        qWarning() << "DataBase::open(): Unable to open database -" << db.lastError().text();
        return false;
    }

    qInfo() << "DataBase::open(): Database opened successfully at" << db.databaseName();
    return true;
}

/**
 * @brief Closes the SQLite database connection.
 */
void DataBase::close()
{
    if (db.isOpen()) {
        db.close();
        qInfo() << "DataBase::close(): Database connection closed.";
    }
}

/**
 * @brief Creates the required tables (ServerTable, WifiTable) if they do not exist.
 * @return True if all tables were created successfully or already exist; false otherwise.
 */
bool DataBase::createTable()
{
    QSqlQuery query(db);

    const QString createServerTable = R"(
        CREATE TABLE IF NOT EXISTS ServerTable (
            connection_name      VARCHAR(50),
            server_ip            VARCHAR(15),
            deviceName           VARCHAR(50),
            user_name            VARCHAR(50),
            password             VARCHAR(50),
            performance          VARCHAR(4),
            enableAudio          BOOLEAN,
            enableMicrophone     BOOLEAN,
            redirectDrive        BOOLEAN,
            redirectUsbDevice    BOOLEAN,
            security             VARCHAR(3),
            gateway              VARCHAR(1),
            gateway_ip           VARCHAR(15),
            gateway_user_name    VARCHAR(50),
            gateway_password     VARCHAR(50)
        )
    )";

    if (!query.exec(createServerTable)) {
        qWarning() << "DataBase::createTable(): Failed to create ServerTable -" << query.lastError().text();
        return false;
    }

    const QString createWifiTable = R"(
        CREATE TABLE IF NOT EXISTS WifiTable (
            ssid     VARCHAR(32),
            password VARCHAR(64)
        )
    )";

    if (!query.exec(createWifiTable)) {
        qWarning() << "DataBase::createTable(): Failed to create WifiTable -" << query.lastError().text();
        return false;
    }

    qInfo() << "DataBase::createTable(): All tables verified or created successfully.";
    return true;
}

/**
 * @brief Inserts a new Wi-Fi record into the WifiTable using the data stored in m_insertIntoValues.
 *
 * Expected order:
 *  0: ssid
 *  1: password
 */
void DataBase::qmlInsertWifiData()
{
    if (m_insertIntoValues.size() != 2) {
        qWarning() << "qmlInsertWifiData(): Invalid Wi-Fi data — expected 2 values, got"
                   << m_insertIntoValues.size();
        return;
    }

    QSqlQuery query(db);
    query.prepare(R"(
        INSERT INTO WifiTable (ssid, password)
        VALUES (:ssid, :password)
    )");

    query.bindValue(":ssid", m_insertIntoValues[0]);
    query.bindValue(":password", m_insertIntoValues[1]);

    if (!query.exec()) {
        qWarning() << "qmlInsertWifiData(): Failed to insert Wi-Fi record -" << query.lastError().text();
        return;
    }

    qInfo() << "qmlInsertWifiData(): Wi-Fi record inserted successfully for SSID:"
            << m_insertIntoValues[0];
}

/**
 * @brief Inserts a new record into the ServerTable using m_insertIntoValues.
 *
 * Expected order of values in m_insertIntoValues (15 total):
 *  0: connection_name
 *  1: server_ip
 *  2: deviceName
 *  3: user_name
 *  4: password
 *  5: performance
 *  6: enableAudio
 *  7: enableMicrophone
 *  8: redirectDrive
 *  9: redirectUsbDevice
 * 10: security
 * 11: gateway
 * 12: gateway_ip
 * 13: gateway_user_name
 * 14: gateway_password
 *
 * Example:
 * @code
 * QStringList data = { "Office", "192.168.0.10", "ThinClient01", "admin", "pass123",
 *                      "High", "true", "false", "true", "true", "NLA", "Y",
 *                      "10.10.10.1", "gatewayUser", "gatewayPass" };
 * db.setInsertIntoValues(data);
 * db.qmlInsertServerData();
 * @endcode
 */
void DataBase::qmlInsertServerData()
{
    // Ensure database connection is valid and open
    QSqlDatabase db = QSqlDatabase::database("ThinClientConnection");
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "qmlInsertServerData(): Database is not open or invalid.";
        return;
    }

    // Validate input data
    if (m_insertIntoValues.size() != 15) {
        qWarning() << "qmlInsertServerData(): Invalid number of fields. Expected 15, got"
                   << m_insertIntoValues.size();
        return;
    }

    // Prepare SQL INSERT statement
    QSqlQuery query(db);
    query.prepare(R"(
        INSERT INTO ServerTable (
            connection_name, server_ip, deviceName, user_name, password,
            performance, enableAudio, enableMicrophone, redirectDrive,
            redirectUsbDevice, security, gateway, gateway_ip,
            gateway_user_name, gateway_password
        ) VALUES (
            :connectionname, :serverip, :deviceName, :username, :passwd,
            :performance, :enableAudio, :enableMicrophone, :redirectDrive,
            :redirectUsbDevice, :security, :gateway, :gatewayip,
            :gatewayusername, :gatewaypassword
        )
    )");

    // Bind values from m_insertIntoValues
    query.bindValue(":connectionname",    m_insertIntoValues[0]);
    query.bindValue(":serverip",          m_insertIntoValues[1]);
    query.bindValue(":deviceName",        m_insertIntoValues[2]);
    query.bindValue(":username",          m_insertIntoValues[3]);
    query.bindValue(":passwd",            m_insertIntoValues[4]);
    query.bindValue(":performance",       m_insertIntoValues[5]);
    query.bindValue(":enableAudio",       m_insertIntoValues[6]);
    query.bindValue(":enableMicrophone",  m_insertIntoValues[7]);
    query.bindValue(":redirectDrive",     m_insertIntoValues[8]);
    query.bindValue(":redirectUsbDevice", m_insertIntoValues[9]);
    query.bindValue(":security",          m_insertIntoValues[10]);
    query.bindValue(":gateway",           m_insertIntoValues[11]);
    query.bindValue(":gatewayip",         m_insertIntoValues[12]);
    query.bindValue(":gatewayusername",   m_insertIntoValues[13]);
    query.bindValue(":gatewaypassword",   m_insertIntoValues[14]);

    // Execute query and handle result
    if (!query.exec()) {
        qWarning() << "qmlInsertServerData(): Failed to insert record -" << query.lastError().text();
        return;
    }

    qInfo().noquote()
        << "qmlInsertServerData(): Record inserted successfully for"
        << m_insertIntoValues[0] << "/" << m_insertIntoValues[1];
}

/**
 * @brief Updates an existing record in the ServerTable with new data.
 *
 * This function updates a server record identified by `connectionName` and `serverIp`
 * using the values stored in `m_insertIntoValues`. It assumes `m_insertIntoValues`
 * contains exactly 15 elements corresponding to the following order:
 *
 *  0: connection_name
 *  1: server_ip
 *  2: deviceName
 *  3: user_name
 *  4: password
 *  5: performance
 *  6: enableAudio
 *  7: enableMicrophone
 *  8: redirectDrive
 *  9: redirectUsbDevice
 * 10: security
 * 11: gateway
 * 12: gateway_ip
 * 13: gateway_user_name
 * 14: gateway_password
 *
 * Example usage:
 * @code
 * db.setInsertIntoValues(values);
 * db.qmlUpdateServerData("Office", "192.168.0.10");
 * @endcode
 *
 * @param connectionName The existing connection name (used for lookup)
 * @param serverIp       The existing server IP (used for lookup)
 */
void DataBase::qmlUpdateServerData(const QString &connectionName, const QString &serverIp)
{
    // Ensure the database connection is valid and open
    QSqlDatabase db = QSqlDatabase::database("ThinClientConnection");
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "qmlUpdateServerData(): Database is not open or invalid.";
        return;
    }

    // Sanity check for input data
    if (m_insertIntoValues.size() != 15) {
        qWarning() << "qmlUpdateServerData(): Invalid input — expected 15 fields, got"
                   << m_insertIntoValues.size();
        return;
    }

    // Prepare the SQL UPDATE statement
    QSqlQuery query(db);
    query.prepare(R"(
        UPDATE ServerTable SET
            connection_name      = :connectionname,
            server_ip            = :serverip,
            deviceName           = :devicename,
            user_name            = :username,
            password             = :passwd,
            performance          = :performance,
            enableAudio          = :enableaudio,
            enableMicrophone     = :enablemicrophone,
            redirectDrive        = :redirectdrive,
            redirectUsbDevice    = :redirectusbdevice,
            security             = :security,
            gateway              = :gateway,
            gateway_ip           = :gatewayip,
            gateway_user_name    = :gatewayusername,
            gateway_password     = :gatewaypassword
        WHERE
            connection_name = :connname
            AND server_ip   = :servrip
    )");

    // Bind updated values (new data)
    query.bindValue(":connectionname",      m_insertIntoValues[0]);
    query.bindValue(":serverip",            m_insertIntoValues[1]);
    query.bindValue(":devicename",          m_insertIntoValues[2]);
    query.bindValue(":username",            m_insertIntoValues[3]);
    query.bindValue(":passwd",              m_insertIntoValues[4]);
    query.bindValue(":performance",         m_insertIntoValues[5]);
    query.bindValue(":enableaudio",         m_insertIntoValues[6]);
    query.bindValue(":enablemicrophone",    m_insertIntoValues[7]);
    query.bindValue(":redirectdrive",       m_insertIntoValues[8]);
    query.bindValue(":redirectusbdevice",   m_insertIntoValues[9]);
    query.bindValue(":security",            m_insertIntoValues[10]);
    query.bindValue(":gateway",             m_insertIntoValues[11]);
    query.bindValue(":gatewayip",           m_insertIntoValues[12]);
    query.bindValue(":gatewayusername",     m_insertIntoValues[13]);
    query.bindValue(":gatewaypassword",     m_insertIntoValues[14]);

    // Bind old identifiers (the record to be updated)
    query.bindValue(":connname", connectionName);
    query.bindValue(":servrip",  serverIp);

    // Execute the query
    if (!query.exec()) {
        qWarning().noquote()
        << "qmlUpdateServerData(): Failed to update record:"
        << query.lastError().text();
        return;
    }

    // Log success and how many rows were updated
    const int rows = query.numRowsAffected();
    if (rows > 0) {
        qInfo().noquote()
        << "qmlUpdateServerData(): Successfully updated record for"
        << connectionName << "/" << serverIp
        << "→ Rows affected:" << rows;
    } else {
        qInfo().noquote()
        << "qmlUpdateServerData(): No rows updated (record not found or identical data).";
    }
}

void DataBase::qmlQueryServerTable(const QString &connectionName, const QString &serverIpAddress)
{
    // Ensure we use the correct database connection
    QSqlDatabase db = QSqlDatabase::database("ThinClientConnection");
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "Database is not open or invalid!";
        return;
    }

    QSqlQuery query(db);
    query.prepare(R"(
        SELECT connection_name, server_ip, deviceName, user_name, password,
               performance, enableAudio, enableMicrophone, redirectDrive,
               redirectUsbDevice, security, gateway, gateway_ip,
               gateway_user_name, gateway_password
        FROM ServerTable
        WHERE connection_name = :connname AND server_ip = :serverip
    )");

    query.bindValue(":connname", connectionName);
    query.bindValue(":serverip", serverIpAddress);

    if (!query.exec()) {
        qWarning() << "Query failed:" << query.lastError().text();
        return;
    }

    if (!query.next()) {
        qInfo() << "No server record found for" << connectionName << serverIpAddress;
        m_queryResultList.clear();
        setQueryResultList({});
        return;
    }

    // Extract result fields
    QStringList resultFields = {
        query.value("connection_name").toString(),
        query.value("server_ip").toString(),
        query.value("deviceName").toString(),
        query.value("user_name").toString(),
        query.value("password").toString(),
        query.value("performance").toString(),
        query.value("enableAudio").toString(),
        query.value("enableMicrophone").toString(),
        query.value("redirectDrive").toString(),
        query.value("redirectUsbDevice").toString(),
        query.value("security").toString(),
        query.value("gateway").toString(),
        query.value("gateway_ip").toString(),
        query.value("gateway_user_name").toString(),
        query.value("gateway_password").toString()
    };

    setQueryResultList(resultFields);

    // Optional: structured debug logging
    qDebug().noquote()
        << "Server record fetched:"
        << "\n Connection:" << connectionName
        << "\n Server IP:" << serverIpAddress
        << "\n Device Name:" << resultFields[2]
        << "\n User:" << resultFields[3]
        << "\n Performance:" << resultFields[5];
}

void DataBase::getServerList(ServerInfoColl &serverInfoColl)
{
    QSqlDatabase db = QSqlDatabase::database("ThinClientConnection");
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "Database is not open in getServerList()";
        return;
    }

    QSqlQuery query(db);
    query.prepare("SELECT connection_name, server_ip FROM ServerTable");

    if (!query.exec()) {
        qWarning() << "getServerList(): Failed to execute query:" << query.lastError().text();
        return;
    }

    int count = 0;
    while (query.next()) {
        const QString connectionName = query.value("connection_name").toString();
        const QString serverIp = query.value("server_ip").toString();
        serverInfoColl.setServerInfo(connectionName, serverIp);
        ++count;
    }

    qInfo() << "getServerList(): Retrieved" << count << "server records.";
}


bool DataBase::removeServer(const QString &connectionName, const QString &serverIp)
{
    QSqlDatabase db = QSqlDatabase::database("ThinClientConnection");
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "Database is not open in removeServer()";
        return false;
    }

    QSqlQuery query(db);
    query.prepare(R"(
        DELETE FROM ServerTable
        WHERE connection_name = :connectionname
        AND server_ip = :serverip
    )");

    query.bindValue(":connectionname", connectionName);
    query.bindValue(":serverip", serverIp);

    if (!query.exec()) {
        qWarning() << "removeServer(): Failed to delete record:"
                   << query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() > 0) {
        qInfo() << "removeServer(): Deleted record for"
                << connectionName << "/" << serverIp;
        return true;
    } else {
        qInfo() << "removeServer(): No record found for"
                << connectionName << "/" << serverIp;
        return false;
    }
}


QStringList DataBase::insertIntoValues() const
{
    return m_insertIntoValues;
}

void DataBase::setInsertIntoValues(QStringList newInsertIntoValues)
{
    if (m_insertIntoValues == newInsertIntoValues)
        return;

    m_insertIntoValues = std::move(newInsertIntoValues);
    emit sigInsertIntoValuesChanged();
}


QStringList DataBase::queryResultList() const
{
    return m_queryResultList;
}

void DataBase::setQueryResultList(QStringList newQueryResultList)
{
    if (m_queryResultList == newQueryResultList)
        return;

    m_queryResultList = std::move(newQueryResultList);
    emit sigQueryResultListChanged();
}
