#include "rdservermodel.h"

RdServerModel::RdServerModel(QObject *parent)
    : QAbstractListModel(parent)
{
    loadFromDatabase();
}

int RdServerModel::rowCount(const QModelIndex &) const
{
    return m_servers.size();
}

QVariant RdServerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_servers.size())
        return {};

    const auto &s = m_servers.at(index.row());

    switch (role) {
    case ConnectionIdRole: return s.connectionId;
    case ConnectionNameRole: return s.connectionName;
    case ServerIpRole: return s.serverIp;
    case DeviceNameRole: return s.deviceName;
    case UserNameRole: return s.userName;
    case PasswordRole: return s.password;
    case PerformanceRole: return s.performance;
    case EnableAudioRole: return s.enableAudio;
    case EnableMicrophoneRole: return s.enableMicrophone;
    case RedirectDriveRole: return s.redirectDrive;
    case RedirectUsbDeviceRole: return s.redirectUsbDevice;
    case SecurityRole: return s.security;
    case GatewayRole: return s.gateway;
    case GatewayIpRole: return s.gatewayIp;
    case GatewayUserNameRole: return s.gatewayUserName;
    case GatewayPasswordRole: return s.gatewayPassword;
    case AutoConnectRole: return s.autoConnect;
    default: return {};
    }
}

QHash<int, QByteArray> RdServerModel::roleNames() const
{
    return {
        {ConnectionIdRole, "connectionId"},
        {ConnectionNameRole, "connectionName"},
        {ServerIpRole, "serverIp"},
        {DeviceNameRole, "deviceName"},
        {UserNameRole, "userName"},
        {PasswordRole, "password"},
        {PerformanceRole, "performance"},
        {EnableAudioRole, "enableAudio"},
        {EnableMicrophoneRole, "enableMicrophone"},
        {RedirectDriveRole, "redirectDrive"},
        {RedirectUsbDeviceRole, "redirectUsbDevice"},
        {SecurityRole, "security"},
        {GatewayRole, "gatewayValue"},
        {GatewayIpRole, "gatewayIpValue"},
        {GatewayUserNameRole, "gatewayUserNameValue"},
        {GatewayPasswordRole, "gatewayPasswordValue"},
        {AutoConnectRole, "autoConnect"},
    };
}

bool RdServerModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_servers.size())
        return false;

    ServerEntry &entry = m_servers[index.row()];

    if (role != AutoConnectRole)
        return false;

    entry.autoConnect = value.toBool();

    QSqlDatabase db = QSqlDatabase::database("ThinClientConnection");
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "ServerModel::setData(): Database not open.";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE ServerTable SET autoConnect = :value WHERE connection_id = :id");
    query.bindValue(":value", entry.autoConnect);
    query.bindValue(":id", entry.connectionId);

    if (!query.exec()) {
        qWarning() << "ServerModel::setData(): Failed to update autoConnect field -"
                   << query.lastError().text();
        return false;
    }

    emit dataChanged(index, index, {role});
    qInfo() << "✅ AutoConnect updated for connection_id:" << entry.connectionId
            << "→" << (entry.autoConnect ? "true" : "false");

    return true;
}

Qt::ItemFlags RdServerModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void RdServerModel::loadFromDatabase()
{
    beginResetModel();
    m_servers.clear();
    fetchAll();
    endResetModel();

    qInfo() << "RdServerModel: Loaded" << m_servers.size() << "records from database.";
}

void RdServerModel::refresh()
{
    loadFromDatabase();
}

void RdServerModel::fetchAll()
{
    QSqlDatabase db = QSqlDatabase::database("ThinClientConnection");
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "RdServerModel::fetchAll(): Database not open.";
        return;
    }

    QSqlQuery query(db);
    if (!query.exec("SELECT * FROM ServerTable ORDER BY connection_name ASC")) {
        qWarning() << "RdServerModel::fetchAll(): Query failed -" << query.lastError().text();
        return;
    }

    while (query.next()) {
        ServerEntry s;
        s.connectionId        = query.value("connection_id").toString();
        s.connectionName      = query.value("connection_name").toString();
        s.serverIp            = query.value("server_ip").toString();
        s.deviceName          = query.value("deviceName").toString();
        s.userName            = query.value("user_name").toString();
        s.password            = query.value("password").toString();
        s.performance         = query.value("performance").toString();
        s.enableAudio         = query.value("enableAudio").toBool();
        s.enableMicrophone    = query.value("enableMicrophone").toBool();
        s.redirectDrive       = query.value("redirectDrive").toBool();
        s.redirectUsbDevice   = query.value("redirectUsbDevice").toBool();
        s.security            = query.value("security").toBool();
        s.gateway             = query.value("gateway").toBool();
        s.gatewayIp           = query.value("gateway_ip").toString();
        s.gatewayUserName     = query.value("gateway_user_name").toString();
        s.gatewayPassword     = query.value("gateway_password").toString();
        s.autoConnect         = query.value("autoConnect").toBool();
        m_servers.append(s);
    }
}

void RdServerModel::removeServer(const QString &connectionId)
{
    QSqlDatabase db = QSqlDatabase::database("ThinClientConnection");
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "RdServerModel::removeServer(): Database not open.";
        return;
    }

    QSqlQuery query(db);
    query.prepare("DELETE FROM ServerTable WHERE connection_id = :id");
    query.bindValue(":id", connectionId);

    if (!query.exec()) {
        qWarning() << "RdServerModel::removeServer(): Delete failed -" << query.lastError().text();
        return;
    }

    // Reflect in model
    for (int i = 0; i < m_servers.size(); ++i) {
        if (m_servers[i].connectionId == connectionId) {
            beginRemoveRows(QModelIndex(), i, i);
            m_servers.removeAt(i);
            endRemoveRows();
            qInfo() << "RdServerModel: Deleted record with connectionId:" << connectionId;
            break;
        }
    }
}

void RdServerModel::updateServer(const QString &connectionId)
{
    Q_UNUSED(connectionId)
    // For simplicity — just reload model for now
    refresh();
}

void RdServerModel::addServer()
{
    refresh();
}
