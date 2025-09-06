#include "Database.h"

DataBase::DataBase(QObject *parent, const QString &path) : QObject{parent} {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);
}

DataBase& DataBase::getInstance(QObject *parent) {
    static DataBase m_dbInstance(parent, "ThinClientDB.db");
    return m_dbInstance;
}

bool DataBase::open() {
    if (!db.open()) {
        qDebug() << "Error: Unable to open database" << db.lastError().text();
        return false;
    }
    return true;
}

void DataBase::close() {
    db.close();
}

bool DataBase::createTable() {
    QSqlQuery query;
    if (!query.exec("CREATE TABLE IF NOT EXISTS ServerTable (connection_name VARCHAR(50), server_ip VARCHAR(15), user_name VARCHAR(50), password VARCHAR(50), performance VARCHAR(4), enable VARCHAR(10), redirect VARCHAR(10), security VARCHAR(3), gateway VARCHAR(1), gateway_ip VARCHAR(15), gateway_user_name VARCHAR(50), gateway_password VARCHAR(50) )")) {
        qDebug() << "Error: Failed to create table contactMessage" << query.lastError().text();
        return false;
    }
    if (!query.exec("CREATE TABLE IF NOT EXISTS WifiTable (ssid VARCHAR(20), password VARCHAR(1))")) {
        qDebug() << "Error: Failed to create table login" << query.lastError().text();
        return false;
    }
    return true;
}

Q_INVOKABLE void DataBase::qmlInsertWifiData() {
    QSqlQuery query;
    query.prepare("INSERT INTO WifiTable(ssid, password) VALUES (:ssid, :password)");
    if(m_insertIntoValues.size() == 2) {
        query.bindValue(":ssid", m_insertIntoValues[0]);
        query.bindValue(":password", m_insertIntoValues[1]);
    }
    if (!query.exec()) {
        qDebug() << "Error: Failed to insert data" << query.lastError().text();
        return;
    }
}

void DataBase::qmlInsertServerData() {
    QSqlQuery query;
    query.prepare("INSERT INTO ServerTable(connection_name, server_ip, user_name, password, performance, enable, redirect, security, gateway, gateway_ip, gateway_user_name, gateway_password) VALUES (:connectionname, :serverip, :username, :passwd, :performance, :enable, :redirect, :security, :gateway, :gatewayip, :gatewayusername, :gatewaypassword)");
    if(m_insertIntoValues.size() == 12) {
        query.bindValue(":connectionname",  m_insertIntoValues[0]);
        query.bindValue(":serverip",        m_insertIntoValues[1]);
        query.bindValue(":username",        m_insertIntoValues[2]);
        query.bindValue(":passwd",          m_insertIntoValues[3]);
        query.bindValue(":performance",     m_insertIntoValues[4]);
        query.bindValue(":enable",          m_insertIntoValues[5]);
        query.bindValue(":redirect",        m_insertIntoValues[6]);
        query.bindValue(":security",        m_insertIntoValues[7]);
        query.bindValue(":gateway",         m_insertIntoValues[8]);
        query.bindValue(":gatewayip",       m_insertIntoValues[9]);
        query.bindValue(":gatewayusername", m_insertIntoValues[10]);
        query.bindValue(":gatewaypassword", m_insertIntoValues[11]);
    }
    if (!query.exec()) {
        qDebug() << "Error: Failed to insert data" << query.lastError().text();
        return;
    }
}

void DataBase::qmlQueryServerTable(QString connectionName, QString serverIpAddress) {
    QSqlQuery query;
    query.prepare("SELECT * FROM ServerTable WHERE connection_name = :connname AND server_ip = :serverip");
    query.bindValue(":connname", connectionName);
    query.bindValue(":serverip", serverIpAddress);
    qDebug()<<"C++::::> 1";
    if (!query.exec()) {
        qDebug() << "Error: Failed to execute query" << query.lastError().text();
        return;
    }

    if (query.next()) {
        qDebug()<<"C++::::> 2";
        QString connName        = query.value("connection_name").toString();
        QString serverIp        = query.value("server_ip").toString();
        QString userName        = query.value("user_name").toString();
        QString password        = query.value("password").toString();
        QString performance     = query.value("performance").toString();
        QString enable          = query.value("enable").toString();
        QString redirect        = query.value("redirect").toString();
        QString security        = query.value("security").toString();
        QString gateway         = query.value("gateway").toString();
        QString gatewayIp       = query.value("gateway_ip").toString();
        QString gatewayUserName = query.value("gateway_user_name").toString();
        QString gatewayPassword = query.value("gateway_password").toString();
        qDebug() << "connName:" << connName << ", serverIp:" << serverIp << "userName:" << userName << ", password:" << password
                 << "performance:" << performance << ", enable:" << enable << "redirect:" << redirect << ", security:" << security
                 << "gateway:" << gateway << ", gatewayIp:" << gatewayIp << "gatewayUserName:" << gatewayUserName << ", gatewayPassword:" << gatewayPassword;
        m_queryResultList.clear();
        QStringList out;
        out.append(connName); out.append(serverIp); out.append(userName); out.append(password); out.append(performance); out.append(enable);
        out.append(redirect); out.append(security); out.append(gateway); out.append(gatewayIp); out.append(gatewayUserName); out.append(gatewayPassword);
        setQueryResultList(out);
    }
}

void DataBase::removeServer(QString connectionName, QString serverIp) {
    QSqlQuery query;
    qDebug()<<"HELLOO::::> DataBase::removeServer() connectionName="<<connectionName<<", serverIp="<<serverIp;
    query.prepare("DELETE FROM ServerTable WHERE connection_name = :connectionname AND server_ip = :serverip");
    query.bindValue(":connectionname", connectionName);
    query.bindValue(":serverip",       serverIp);
    if (!query.exec()) {
        qDebug() << "Error: Failed to delete data" << query.lastError().text();
        return;
    }
}

QStringList DataBase::insertIntoValues() const {
    return m_insertIntoValues;
}

void DataBase::setInsertIntoValues(const QStringList &newInsertIntoValues) {
    if (m_insertIntoValues != newInsertIntoValues) {
        m_insertIntoValues = newInsertIntoValues;
        emit sigInsertIntoValuesChanged();
    }
}

QStringList DataBase::queryResultList() const {
    return m_queryResultList;
}

void DataBase::setQueryResultList(const QStringList &newQueryResultList) {
    if(m_queryResultList != newQueryResultList) {
        m_queryResultList = newQueryResultList;
        emit sigQueryResultListChanged();
    }
}
