#ifndef APPUNLOCKMANAGER_H
#define APPUNLOCKMANAGER_H

#include <QObject>

class AppUnlockManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool hasPassword READ hasPassword NOTIFY hasPasswordChanged)

public:
    explicit AppUnlockManager(QObject *parent = nullptr);

    Q_INVOKABLE bool storeUnlockToken(const QString &password, const QString &token);
    Q_INVOKABLE QString loadUnlockToken(const QString &password);
    Q_INVOKABLE bool clearPassword(const QString &password); // new method

    bool hasPassword() const; // property getter

signals:
    void hasPasswordChanged();

private:
    QByteArray generateKey(const QString &password, const QByteArray &salt);
    QByteArray xorData(const QByteArray &data, const QByteArray &key);
    QString storagePath() const;
};

#endif // APPUNLOCKMANAGER_H
