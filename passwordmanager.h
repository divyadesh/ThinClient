#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H

#include <QObject>

class PasswordManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool hasPassword READ hasPassword NOTIFY hasPasswordChanged)

public:
    explicit PasswordManager(QObject *parent = nullptr);

    bool hasPassword() const;

    Q_INVOKABLE bool setPassword(const QString &newPassword);
    Q_INVOKABLE bool changePassword(const QString &oldPassword, const QString &newPassword);
    Q_INVOKABLE bool removePassword(const QString &oldPassword);
    Q_INVOKABLE bool verifyPassword(const QString &password) const;

signals:
    void hasPasswordChanged();

private:
    QString m_password;
};

#endif // PASSWORDMANAGER_H
