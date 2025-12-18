#include "sessionmodel.h"
#include <QHash>
#include <algorithm>
#include <QAbstractListModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariantList>
#include <QDebug>

namespace {
constexpr int COLUMN_CONNECTION_NAME = 0;
constexpr int COLUMN_SERVER_IP = 1;
constexpr int COLUMN_AUTOCONNECT = 2;
constexpr int COLUMN_MANAGE = 3;
constexpr int COLUMN_COUNT = 4;
} // namespace

SessionModel::SessionModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    loadServers();
}

/* ----------------------------------------------
 * Data Retrieval
 * ---------------------------------------------- */

QVariant SessionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_sessions.size())
        return {};

    const ConnectionInfo *session = m_sessions.at(index.row());
    if (!session)
        return {};

    // TableView visible columns use DisplayRole:
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case COLUMN_CONNECTION_NAME:
            return session->connectionName();
        case COLUMN_SERVER_IP:
            return session->serverIp();
        case COLUMN_AUTOCONNECT:
            return session->autoConnect() ? QStringLiteral("Yes") : QStringLiteral("No");
        case COLUMN_MANAGE:
            return QStringLiteral("Manage");
        default:
            return {};
        }
    }

    // Column type role for DelegateChooser (role: "columnType")
    if (role == RoleColumnType) {
        return selectColumnType(index);
    }

    // QML property roles:
    switch (role) {
    case RoleConnectionId:
        return session->connectionId();
    case RoleConnectionName:
        return session->connectionName();
    case RoleServerIp:
        return session->serverIp();
    case RoleDeviceName:
        return session->deviceName();
    case RoleUserName:
        return session->userName();
    case RolePassword:
        return session->password();
    case RolePerformance:
        return session->performance();
    case RoleEnableAudio:
        return session->enableAudio();
    case RoleEnableMicrophone:
        return session->enableMicrophone();
    case RoleRedirectDrive:
        return session->redirectDrive();
    case RoleRedirectUsbDevice:
        return session->redirectUsbDevice();
    case RoleSecurity:
        return session->security();
    case RoleGateway:
        return session->gateway();
    case RoleGatewayIp:
        return session->gatewayIp();
    case RoleGatewayUserName:
        return session->gatewayUserName();
    case RoleGatewayPassword:
        return session->gatewayPassword();
    case RoleAutoConnect:
        return session->autoConnect();
    default:
        break;
    }

    return {};
}

/* ----------------------------------------------
 * Editing
 * ---------------------------------------------- */

bool SessionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_sessions.size())
        return false;

    if (role != RoleAutoConnect)
        return false;

    ConnectionInfo *session = m_sessions.at(index.row());
    if (!session)
        return false;

    bool changed = false;
    switch (role) {
    case RoleAutoConnect:
    {
        session->setAutoConnect(value.toBool());
        updateAutoConnect(session->connectionId(), value.toBool());
        changed = true;
    }
    break;
    default:
        break;
    }

    if (changed) {
        emit dataChanged(index, index, {role});
        return true;
    }

    return false;
}

/* ----------------------------------------------
 * Flags
 * ---------------------------------------------- */

Qt::ItemFlags SessionModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

/* ----------------------------------------------
 * Headers
 * ---------------------------------------------- */

QVariant SessionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    switch (section) {
    case COLUMN_CONNECTION_NAME:
        return QStringLiteral("Connection Name");
    case COLUMN_SERVER_IP:
        return QStringLiteral("Server IP");
    case COLUMN_AUTOCONNECT:
        return QStringLiteral("Auto Connect");
    case COLUMN_MANAGE:
        return QStringLiteral("Manage Connection");
    default:
        return {};
    }
}

/* ---------------------------------------------- */

int SessionModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_sessions.size();
}

int SessionModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return COLUMN_COUNT;
}

/* ----------------------------------------------
 * Roles for QML
 * ---------------------------------------------- */

QHash<int, QByteArray> SessionModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[RoleConnectionId] = "connectionId";
    roles[RoleConnectionName] = "connectionName";
    roles[RoleServerIp] = "serverIp";
    roles[RoleDeviceName] = "deviceName";
    roles[RoleUserName] = "userName";
    roles[RolePassword] = "password";
    roles[RolePerformance] = "performance";
    roles[RoleEnableAudio] = "enableAudio";
    roles[RoleEnableMicrophone] = "enableMicrophone";
    roles[RoleRedirectDrive] = "redirectDrive";
    roles[RoleRedirectUsbDevice] = "redirectUsbDevice";
    roles[RoleSecurity] = "security";
    roles[RoleGateway] = "gateway";
    roles[RoleGatewayIp] = "gatewayIp";
    roles[RoleGatewayUserName] = "gatewayUserName";
    roles[RoleGatewayPassword] = "gatewayPassword";
    roles[RoleAutoConnect] = "autoConnect";

    // Role for DelegateChooser { role: "columnType" }
    roles[RoleColumnType] = "columnType";

    return roles;
}

/* ----------------------------------------------
 * Column sizing (4 equal columns)
 * ---------------------------------------------- */

QList<double> SessionModel::columnSizes() const
{
    // Your QML does: headerView.setColumnWidth(i, table.width / columnSource[i])
    // To get 4 equal columns, we return [4,4,4,4]:
    // columnWidth = table.width / 4 for each column.
    return {4.0, 4.0, 4.0, 4.0};
}

void SessionModel::disableAutoConnectForAll()
{
    if (rowCount() == 0)
        return;

    const int autoConnectColumn = 2; // 3rd column (0-based)
    for (int row = 0; row < m_sessions.size(); ++row) {
        ConnectionInfo *info = m_sessions.at(row);
        if (!info)
            continue;

        if (info->autoConnect()) {
            QModelIndex idx = index(row, autoConnectColumn);
            setData(idx, false, RoleAutoConnect);
        }
    }
}


/* ----------------------------------------------
 * Helpers
 * ---------------------------------------------- */

void SessionModel::clearSessions()
{
    beginResetModel();
    m_sessions.clear();
    endResetModel();
}

ConnectionInfo *SessionModel::sessionAt(int row) const
{
    if (row < 0 || row >= m_sessions.size())
        return nullptr;
    return m_sessions.at(row);
}

QList<ConnectionInfo *> SessionModel::sessions() const
{
    return m_sessions;
}

QObject* SessionModel::getSessionById(const QString &connectionId) const
{
    for (ConnectionInfo *session : m_sessions) {
        if (session && session->connectionId() == connectionId)
            return session;  // implicit upcast to QObject*
    }
    return nullptr;
}

/* ----------------------------------------------
 * Column Type Mapping (for "columnType" role)
 * ---------------------------------------------- */

QVariant SessionModel::selectColumnType(const QModelIndex &index) const
{
    if (!index.isValid())
        return {};

    switch (index.column()) {
    case COLUMN_CONNECTION_NAME:
        return QStringLiteral("name");
    case COLUMN_SERVER_IP:
        return QStringLiteral("ip");
    case COLUMN_AUTOCONNECT:
        return QStringLiteral("auto");
    case COLUMN_MANAGE:
        return QStringLiteral("manage");
    default:
        return {};
    }
}

/* ----------------------------------------------
 * Updating Sessions
 * ---------------------------------------------- */

void SessionModel::setSessions(const QList<ConnectionInfo *> &newSessions)
{
    beginResetModel();
    m_sessions = newSessions;
    endResetModel();
}

void SessionModel::loadServers()
{
    beginResetModel();

    qDeleteAll(m_sessions);   // prevent memory leak
    m_sessions.clear();

    fetchAllServers();

    endResetModel();
}

void SessionModel::reloadServers()
{
    loadServers();
}

void SessionModel::fetchAllServers()
{
    QSqlDatabase db = QSqlDatabase::database("ThinClientConnection");
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "SessionModel::fetchAllServers(): Database not open.";
        return;
    }

    QSqlQuery query(db);
    if (!query.exec("SELECT * FROM ServerTable ORDER BY connection_name ASC")) {
        qWarning() << "RdServerModel::fetchAllServers(): Query failed -" << query.lastError().text();
        return;
    }

    setAutoConnectionId("");

    while (query.next()) {
        ConnectionInfo *session = new ConnectionInfo(this);

        session->setConnectionId(query.value("connection_id").toString());
        session->setConnectionName(query.value("connection_name").toString());
        session->setServerIp(query.value("server_ip").toString());
        session->setDeviceName(query.value("deviceName").toString());
        session->setUserName(query.value("user_name").toString());
        session->setPassword(query.value("password").toString());
        session->setPerformance(query.value("performance").toString());

        session->setEnableAudio(query.value("enableAudio").toBool());
        session->setEnableMicrophone(query.value("enableMicrophone").toBool());
        session->setRedirectDrive(query.value("redirectDrive").toBool());
        session->setRedirectUsbDevice(query.value("redirectUsbDevice").toBool());
        session->setSecurity(query.value("security").toBool());
        session->setGateway(query.value("gateway").toBool());

        session->setGatewayIp(query.value("gateway_ip").toString());
        session->setGatewayUserName(query.value("gateway_user_name").toString());
        session->setGatewayPassword(query.value("gateway_password").toString());

        session->setAutoConnect(query.value("autoConnect").toBool());

        if(session->autoConnect()) {
            setAutoConnectionId(session->connectionId());
        }

        // Add object to model storage
        m_sessions.append(session);
    }
}


void SessionModel::deleteServer(const QString &connectionId)
{
    QSqlDatabase db = QSqlDatabase::database("ThinClientConnection");
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "SessionModel::deleteServer(): Database not open.";
        return;
    }

    // --- Delete from database ---
    QSqlQuery query(db);
    query.prepare("DELETE FROM ServerTable WHERE connection_id = :id");
    query.bindValue(":id", connectionId);

    if (!query.exec()) {
        qWarning() << "SessionModel::deleteServer(): Delete failed -"
                   << query.lastError().text();
        return;
    }

    // --- Delete from model list ---
    for (int i = 0; i < m_sessions.size(); ++i) {

        ConnectionInfo *session = m_sessions.at(i);
        if (session->connectionId() == connectionId) {

            beginRemoveRows(QModelIndex(), i, i);

            // Remove from internal list
            m_sessions.removeAt(i);

            endRemoveRows();

            // Free memory (QObject created with 'this' parent)
            session->deleteLater();

            qInfo() << "SessionModel: Deleted record with connectionId:" << connectionId;
            return;   // Stop after removing one item
        }
    }

    qWarning() << "SessionModel::deleteServer(): Record with ID"
               << connectionId << "not found in model list.";
}

void SessionModel::updateServerById(const QString &connectionId)
{
    Q_UNUSED(connectionId);
    // For simplicity — just reload model for now
    reloadServers();
}

bool SessionModel::updateAutoConnect(const QString &connectionId, bool enabled)
{

    QSqlDatabase db = QSqlDatabase::database("ThinClientConnection");
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "SessionModel::updateAutoConnect(): Database not open.";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE ServerTable "
                  "SET autoConnect = :value "
                  "WHERE connection_id = :id");

    query.bindValue(":value", enabled);
    query.bindValue(":id", connectionId);

    if (!query.exec()) {
        qWarning() << "SessionModel::updateAutoConnect(): Failed to update autoConnect -"
                   << query.lastError().text();
        return false;
    }

    qWarning() << "SessionModel::updateAutoConnect(): Record updated in DB successfully";
    return true;
}

void SessionModel::addNewServer()
{
    reloadServers();
}


QString SessionModel::autoConnectionId() const
{
    return m_autoConnectionId;
}

void SessionModel::setAutoConnectionId(const QString &newAutoConnectionId)
{
    if (m_autoConnectionId == newAutoConnectionId)
        return;
    m_autoConnectionId = newAutoConnectionId;
    emit autoConnectionIdChanged();
}
