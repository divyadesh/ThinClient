// language_model.h
#pragma once

#include <QAbstractListModel>
#include <QLocale>
#include "appsettings.h"

class LanguageModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum LanguageRoles { CodeRole = Qt::UserRole + 1, NameRole, SystemRole };

    explicit LanguageModel(AppSettings *settings, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE QVariantMap get(int row) const;
    Q_INVOKABLE int systemLanguageIndex() const;
    Q_INVOKABLE int indexForCode(const QString &code) const;

private:
    struct Lang
    {
        QString code;
        QString name;
        bool isSystem;
    };
    QList<Lang> m_languages;
    AppSettings *m_settings;
};
