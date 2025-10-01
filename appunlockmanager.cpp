#include "AppUnlockManager.h"
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QRandomGenerator>
#include <QStandardPaths>

AppUnlockManager::AppUnlockManager(QObject *parent)
    : QObject(parent)
{}

bool AppUnlockManager::hasPassword() const
{
    QFile file(storagePath());
    return file.exists() && file.size() > 8; // Must be at least long enough to store salt
}

QString AppUnlockManager::storagePath() const
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    qDebug() << "The File Path :: " << path;
    QDir().mkpath(path);
    return path + "/unlock.dat";
}

QByteArray AppUnlockManager::generateKey(const QString &password, const QByteArray &salt)
{
    QByteArray data = password.toUtf8() + salt;
    for (int i = 0; i < 1000; ++i) {
        data = QCryptographicHash::hash(data, QCryptographicHash::Sha256);
    }
    return data;
}

QByteArray AppUnlockManager::xorData(const QByteArray &data, const QByteArray &key)
{
    QByteArray result;
    for (int i = 0; i < data.size(); ++i) {
        result.append(data[i] ^ key[i % key.size()]);
    }
    return result;
}

bool AppUnlockManager::storeUnlockToken(const QString &password, const QString &token)
{
    QByteArray salt(8, 0);
    for (int i = 0; i < salt.size(); ++i) {
        salt[i] = QRandomGenerator::global()->bounded(256);
    }

    QByteArray key = generateKey(password, salt);
    QByteArray obfuscated = xorData(token.toUtf8(), key);

    QFile file(storagePath());
    if (!file.open(QIODevice::WriteOnly))
        return false;
    file.write(salt);
    file.write(obfuscated);
    file.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner); // chmod 600
    file.close();

    emit hasPasswordChanged(); // notify UI or logic
    return true;
}

QString AppUnlockManager::loadUnlockToken(const QString &password)
{
    QFile file(storagePath());
    if (!file.open(QIODevice::ReadOnly))
        return QString();

    QByteArray all = file.readAll();
    if (all.size() < 8)
        return QString(); // too small to contain salt

    QByteArray salt = all.left(8);
    QByteArray obfuscated = all.mid(8);

    QByteArray key = generateKey(password, salt);
    QByteArray plaintext = xorData(obfuscated, key);

    return QString::fromUtf8(plaintext);
}

bool AppUnlockManager::clearPassword(const QString &password)
{
    // Optional: Validate the password before clearing
    QString token = loadUnlockToken(password);
    if (token != "UNLOCK_OK") {
        qWarning() << "Password validation failed, not clearing.";
        return false;
    }

    QFile file(storagePath());
    if (file.exists()) {
        bool removed = file.remove();
        if (removed)
            emit hasPasswordChanged();
        return removed;
    }

    return false;
}
