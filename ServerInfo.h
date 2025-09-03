#ifndef SERVERINFO_H
#define SERVERINFO_H

#include <QObject>

class ServerInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString connectionName READ connectionName WRITE setConnectionName NOTIFY sigConnectionNameChanged FINAL)
    Q_PROPERTY(QString serverIp READ serverIp WRITE setServerIp NOTIFY sigServerIpChanged FINAL)
    Q_PROPERTY(bool autoEnable READ autoEnable WRITE setAutoEnable NOTIFY sigAutoEnableChanged FINAL)

    QString m_connectionName;
    QString m_serverIp;
    bool m_autoEnable;
public:
    explicit ServerInfo(QObject *parent = nullptr, QString connectionName = "", QString serverIp = "", bool autoEnable = false);

    QString connectionName() const;
    void setConnectionName(const QString &newConnectionName);

    QString serverIp() const;
    void setServerIp(const QString &newServerIp);

    bool autoEnable() const;
    void setAutoEnable(const bool &newAutoEnable);

signals:
    void sigConnectionNameChanged(QString connectionName);
    void sigServerIpChanged(QString serverIp);
    void sigAutoEnableChanged(bool autoEnable);
};

#endif // SERVERINFO_H
