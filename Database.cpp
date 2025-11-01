#include "Database.h"
#include "ServerInfoColl.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>


inline QString safeValue(const QStringList &list, int index, const QString &defaultValue = QString())
{
    if (index < 0 || index >= list.size())
        return defaultValue;

    QString val = list.at(index).trimmed();
    return val.isEmpty() ? defaultValue : val;
}
inline bool toBool(const QString &value, bool defaultVal = false)
{
    if (value.isEmpty()) return defaultVal;
    QString lower = value.trimmed().toLower();
    return (lower == "1" || lower == "true" || lower == "yes");
}

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
#  ifdef QT_DEBUG
    static DataBase m_dbInstance(parent, "/home/rajni/ThinClientDB_debug.db");
#  else
    static DataBase m_dbInstance(parent, "/home/root/ThinClientDB.db");
#  endif
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
        connection_id         TEXT PRIMARY KEY DEFAULT (lower(hex(randomblob(16)))),
        connection_name       VARCHAR(50),
        server_ip             VARCHAR(15),
        deviceName            VARCHAR(50),
        user_name             VARCHAR(50),
        password              VARCHAR(50),
        performance           VARCHAR(4),
        enableAudio           BOOLEAN,
        enableMicrophone      BOOLEAN,
        redirectDrive         BOOLEAN,
        redirectUsbDevice     BOOLEAN,
        security              VARCHAR(3),
        gateway               VARCHAR(1),
        gateway_ip            VARCHAR(15),
        gateway_user_name     VARCHAR(50),
        gateway_password      VARCHAR(50),
        autoConnect           BOOLEAN DEFAULT 0
    )
    )";

    if (!query.exec(createServerTable)) {
        qWarning() << "DataBase::createTable(): Failed to create ServerTable -" << query.lastError().text();
        return false;
    }

    const QString createWifiTable = R"(
    CREATE TABLE IF NOT EXISTS WifiTable (
        wifi_id             TEXT PRIMARY KEY DEFAULT (lower(hex(randomblob(16)))),
        ssid                VARCHAR(32),
        password            VARCHAR(64),
        known               BOOLEAN DEFAULT 0,
        last_connected_at   DATETIME DEFAULT NULL,
        signal_strength     INTEGER DEFAULT NULL,      -- e.g., RSSI value
        security_type       VARCHAR(10) DEFAULT NULL,  -- e.g., WPA2, WPA3, Open
        is_hidden           BOOLEAN DEFAULT 0
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
    // Expect at least SSID and password
    if (m_insertIntoValues.size() < 2) {
        qWarning() << "qmlInsertWifiData(): Invalid Wi-Fi data — expected at least 2 values, got"
                   << m_insertIntoValues.size();
        return;
    }

    QSqlQuery query(db);

    // Insert with defaults for optional fields
    query.prepare(R"(
        INSERT INTO WifiTable (
            ssid,
            password,
            known,
            last_connected_at,
            signal_strength,
            security_type,
            is_hidden
        ) VALUES (
            :ssid,
            :password,
            0,                     -- known (false by default)
            NULL,                  -- last_connected_at
            NULL,                  -- signal_strength
            NULL,                  -- security_type
            0                      -- is_hidden (false)
        )
    )");

    // Required bindings
    query.bindValue(":ssid", m_insertIntoValues[0]);
    query.bindValue(":password", m_insertIntoValues[1]);

    // Execute
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
    QSqlDatabase db = QSqlDatabase::database("ThinClientConnection");
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "qmlInsertServerData(): Database is not open or invalid.";
        return;
    }

    if (m_insertIntoValues.isEmpty()) {
        qWarning() << "qmlInsertServerData(): No values provided.";
        return;
    }

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

    // Safe binding using helper functions
    query.bindValue(":connectionname",    safeValue(m_insertIntoValues, 0,  "Unnamed"));
    query.bindValue(":serverip",          safeValue(m_insertIntoValues, 1,  "0.0.0.0:0000"));
    query.bindValue(":deviceName",        safeValue(m_insertIntoValues, 2,  "UnknownDevice"));
    query.bindValue(":username",          safeValue(m_insertIntoValues, 3,  "guest"));
    query.bindValue(":passwd",            safeValue(m_insertIntoValues, 4,  ""));
    query.bindValue(":performance",       safeValue(m_insertIntoValues, 5,  "Best"));
    query.bindValue(":enableAudio",       toBool(safeValue(m_insertIntoValues, 6), false));
    query.bindValue(":enableMicrophone",  toBool(safeValue(m_insertIntoValues, 7), false));
    query.bindValue(":redirectDrive",     toBool(safeValue(m_insertIntoValues, 8), false));
    query.bindValue(":redirectUsbDevice", toBool(safeValue(m_insertIntoValues, 9), false));
    query.bindValue(":security",          safeValue(m_insertIntoValues, 10, "RDP"));
    query.bindValue(":gateway",           safeValue(m_insertIntoValues, 11, "0"));
    query.bindValue(":gatewayip",         safeValue(m_insertIntoValues, 12, "0.0.0.0"));
    query.bindValue(":gatewayusername",   safeValue(m_insertIntoValues, 13, ""));
    query.bindValue(":gatewaypassword",   safeValue(m_insertIntoValues, 14, ""));

    if (!query.exec()) {
        qWarning() << "qmlInsertServerData(): Insert failed -" << query.lastError().text();
        return;
    }

    qInfo().noquote()
        << "✅ qmlInsertServerData(): Inserted record for"
        << safeValue(m_insertIntoValues, 0) << "/" << safeValue(m_insertIntoValues, 1);
    emit refreshTable();
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

void DataBase::qmlUpdateServerData(const QString &connectionId)
{
    // Ensure the database connection is valid and open
    QSqlDatabase db = QSqlDatabase::database("ThinClientConnection");
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "qmlUpdateServerData(): Database is not open or invalid.";
        return;
    }

    // Validate UUID
    if (connectionId.trimmed().isEmpty()) {
        qWarning() << "qmlUpdateServerData(): Invalid connection ID.";
        return;
    }

    // Pad missing fields to ensure 15 values
    if (m_insertIntoValues.size() < 15) {
        qWarning() << "qmlUpdateServerData(): Fewer than 15 values, filling with defaults.";
        while (m_insertIntoValues.size() < 15)
            m_insertIntoValues.append("");
    }

    // Prepare SQL statement
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
            connection_id = :connectionid
    )");

    // Bind safe values using your helpers
    query.bindValue(":connectionname",    safeValue(m_insertIntoValues, 0,  "Unnamed"));
    query.bindValue(":serverip",          safeValue(m_insertIntoValues, 1,  "0.0.0.0:0000"));
    query.bindValue(":devicename",        safeValue(m_insertIntoValues, 2,  "UnknownDevice"));
    query.bindValue(":username",          safeValue(m_insertIntoValues, 3,  "guest"));
    query.bindValue(":passwd",            safeValue(m_insertIntoValues, 4,  ""));
    query.bindValue(":performance",       safeValue(m_insertIntoValues, 5,  "Best"));
    query.bindValue(":enableaudio",       toBool(m_insertIntoValues[6], false));
    query.bindValue(":enablemicrophone",  toBool(m_insertIntoValues[7], false));
    query.bindValue(":redirectdrive",     toBool(m_insertIntoValues[8], false));
    query.bindValue(":redirectusbdevice", toBool(m_insertIntoValues[9], false));
    query.bindValue(":security",          safeValue(m_insertIntoValues, 10, "RDP"));
    query.bindValue(":gateway",           safeValue(m_insertIntoValues, 11, "0"));
    query.bindValue(":gatewayip",         safeValue(m_insertIntoValues, 12, "0.0.0.0"));
    query.bindValue(":gatewayusername",   safeValue(m_insertIntoValues, 13, ""));
    query.bindValue(":gatewaypassword",   safeValue(m_insertIntoValues, 14, ""));

    // Bind connection ID (primary key)
    query.bindValue(":connectionid", connectionId);

    // Execute safely
    if (!query.exec()) {
        qWarning().noquote()
        << "❌ qmlUpdateServerData(): Update failed -" << query.lastError().text();
        return;
    }

    // Report result
    const int rows = query.numRowsAffected();
    if (rows > 0) {
        qInfo().noquote()
        << "✅ qmlUpdateServerData(): Updated record with connection_id:" << connectionId
        << "→ Rows affected:" << rows;
        emit refreshTable();
    } else {
        qInfo().noquote()
        << "⚠️ qmlUpdateServerData(): No rows updated (invalid ID or identical data).";
    }
}

QStringList DataBase::qmlQueryServerTable(const QString &connectionId)
{
    // Ensure we use the correct database connection
    QSqlDatabase db = QSqlDatabase::database("ThinClientConnection");
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "qmlQueryServerTable(): Database is not open or invalid!";
        return {};
    }

    // Validate connection ID
    if (connectionId.trimmed().isEmpty()) {
        qWarning() << "qmlQueryServerTable(): Invalid connection ID provided.";
        return {};
    }

    // Prepare the query using the UUID (connection_id)
    QSqlQuery query(db);
    query.prepare(R"(
        SELECT connection_id, connection_name, server_ip, deviceName, user_name, password,
               performance, enableAudio, enableMicrophone, redirectDrive,
               redirectUsbDevice, security, gateway, gateway_ip,
               gateway_user_name, gateway_password
        FROM ServerTable
        WHERE connection_id = :connectionid
    )");

    query.bindValue(":connectionid", connectionId);

    // Execute safely
    if (!query.exec()) {
        qWarning() << "qmlQueryServerTable(): Query failed -" << query.lastError().text();
        return {};
    }

    // Check if a record was found
    if (!query.next()) {
        qInfo() << "qmlQueryServerTable(): No server record found for connection_id:" << connectionId;
        m_queryResultList.clear();
        setQueryResultList({});
        return {};
    }

    // Extract all fields safely
    QStringList resultFields = {
        query.value("connection_id").toString(),
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

    // Store result internally
    setQueryResultList(resultFields);

    // Structured log for debug
    qDebug().noquote()
        << "✅ qmlQueryServerTable(): Record fetched successfully"
        << "\n Connection ID:" << resultFields[0]
        << "\n Connection Name:" << resultFields[1]
        << "\n Server IP:" << resultFields[2]
        << "\n Device Name:" << resultFields[3]
        << "\n User:" << resultFields[4]
        << "\n Performance:" << resultFields[6];

    emit refreshTable();
    return resultFields;
}

void DataBase::getServerList(ServerInfoColl &serverInfoColl)
{
    QSqlDatabase db = QSqlDatabase::database("ThinClientConnection");
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "❌ getServerList(): Database is not open or invalid.";
        return;
    }

    QSqlQuery query(db);
    query.prepare("SELECT connection_id, connection_name, server_ip FROM ServerTable ORDER BY connection_name ASC");

    if (!query.exec()) {
        qWarning() << "❌ getServerList(): Query failed -" << query.lastError().text();
        return;
    }

    int count = 0;
    while (query.next()) {
        const QString connectionId   = query.value("connection_id").toString();
        const QString connectionName = query.value("connection_name").toString();
        const QString serverIp       = query.value("server_ip").toString();

        // Pass all three fields to the model
        serverInfoColl.setServerInfo(connectionId);
        ++count;
    }

    emit refreshTable();
    qInfo().noquote() << "✅ getServerList(): Retrieved" << count << "server records from database.";
}


bool DataBase::removeServer(const QString &connectionId)
{
    // Ensure database connection is valid and open
    QSqlDatabase db = QSqlDatabase::database("ThinClientConnection");
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "❌ removeServer(): Database is not open or invalid.";
        return false;
    }

    // Validate input
    if (connectionId.trimmed().isEmpty()) {
        qWarning() << "❌ removeServer(): Invalid connectionId (empty or null).";
        return false;
    }

    // Prepare DELETE statement
    QSqlQuery query(db);
    query.prepare(R"(
        DELETE FROM ServerTable
        WHERE connection_id = :connectionid
    )");
    query.bindValue(":connectionid", connectionId);

    // Execute safely
    if (!query.exec()) {
        qWarning().noquote()
        << "❌ removeServer(): Failed to delete record for connectionId:" << connectionId
        << "\nError:" << query.lastError().text();
        return false;
    }

    // Check affected rows
    const int rows = query.numRowsAffected();
    if (rows > 0) {
        qInfo().noquote()
        << "🗑️ removeServer(): Successfully deleted record with connectionId:" << connectionId;
        emit refreshTable();
        return true;
    } else {
        qInfo().noquote()
        << "⚠️ removeServer(): No record found for connectionId:" << connectionId;
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

bool DataBase::serverExists(const QString &connectionName, const QString &serverIp)
{
    QSqlDatabase db = QSqlDatabase::database("ThinClientConnection");
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "❌ serverExists(): Database not open.";
        return false;
    }

    QSqlQuery query(db);
    query.prepare(R"(
        SELECT COUNT(1)
        FROM ServerTable
        WHERE connection_name = :name
          AND server_ip = :ip
    )");

    query.bindValue(":name", connectionName.trimmed());
    query.bindValue(":ip", serverIp.trimmed());

    if (!query.exec()) {
        qWarning() << "❌ serverExists(): Query failed -" << query.lastError().text();
        return false;
    }

    return (query.next() && query.value(0).toInt() > 0);
}

bool DataBase::serverExists(const QString &connectionId)
{
    QSqlDatabase db = QSqlDatabase::database("ThinClientConnection");
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "❌ serverExists(): Database is not open or invalid.";
        return false;
    }

    if (connectionId.trimmed().isEmpty()) {
        qWarning() << "❌ serverExists(): Invalid (empty) connectionId.";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT COUNT(1) FROM ServerTable WHERE connection_id = :connectionid");
    query.bindValue(":connectionid", connectionId);

    if (!query.exec()) {
        qWarning() << "❌ serverExists(): Query failed -" << query.lastError().text();
        return false;
    }

    return (query.next() && query.value(0).toInt() > 0);
}

