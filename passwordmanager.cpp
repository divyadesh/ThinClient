#include "PasswordManager.h"

PasswordManager::PasswordManager(QObject *parent)
    : QObject(parent)
{}

bool PasswordManager::hasPassword() const
{
    return !m_password.isEmpty();
}

bool PasswordManager::setPassword(const QString &newPassword)
{
    if (!m_password.isEmpty()) {
        // already set, must use changePassword instead
        return false;
    }

    if (newPassword.isEmpty())
        return false;

    m_password = newPassword;
    emit hasPasswordChanged();
    return true;
}

bool PasswordManager::changePassword(const QString &oldPassword, const QString &newPassword)
{
    if (m_password.isEmpty() || m_password != oldPassword || newPassword.isEmpty())
        return false;

    m_password = newPassword;
    return true;
}

bool PasswordManager::removePassword(const QString &oldPassword)
{
    if (m_password.isEmpty() || m_password != oldPassword)
        return false;

    m_password.clear();
    emit hasPasswordChanged();
    return true;
}

bool PasswordManager::verifyPassword(const QString &password) const
{
    return m_password == password && !m_password.isEmpty();
}
