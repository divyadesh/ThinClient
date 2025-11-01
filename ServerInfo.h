#ifndef SERVERINFO_H
#define SERVERINFO_H

#include <QObject>

class ServerInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString connectionId READ connectionId WRITE setConnectionId NOTIFY connectionIdChanged FINAL)

public:
    explicit ServerInfo(const QString &connId, QObject *parent = nullptr);
    QString connectionId() const;

public slots:
    void setConnectionId(const QString &newConnectionId);

signals:
    void connectionIdChanged();

private:
    QString m_connectionId;
};

#endif // SERVERINFO_H
