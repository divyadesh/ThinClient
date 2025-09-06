// language_model.cpp
#include "language_model.h"

LanguageModel::LanguageModel(AppSettings *settings, QObject *parent)
    : QAbstractListModel(parent)
    , m_settings(settings)
{
    QList<QString> supported = {"en", "hi"};
    QString systemLang = QLocale::system().name().left(2);

    for (const QString &code : supported) {
        QLocale loc(code);
        Lang lang;
        lang.code = code;
        lang.name = loc.nativeLanguageName().isEmpty() ? QLocale::languageToString(loc.language())
                                                       : loc.nativeLanguageName();
        lang.isSystem = (code == systemLang);
        m_languages.append(lang);
    }
}

int LanguageModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_languages.size();
}

QVariant LanguageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_languages.size())
        return QVariant();

    const Lang &lang = m_languages.at(index.row());
    switch (role) {
    case CodeRole:
        return lang.code;
    case NameRole:
        return lang.name;
    case SystemRole:
        return lang.isSystem;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> LanguageModel::roleNames() const
{
    return {{CodeRole, "langCode"}, {NameRole, "langName"}, {SystemRole, "isSystem"}};
}

QVariantMap LanguageModel::get(int row) const
{
    QVariantMap map;
    if (row < 0 || row >= m_languages.size())
        return map;

    const Lang &lang = m_languages.at(row);
    map["langCode"] = lang.code;
    map["langName"] = lang.name;
    map["isSystem"] = lang.isSystem;
    return map;
}

int LanguageModel::systemLanguageIndex() const
{
    for (int i = 0; i < m_languages.size(); ++i)
        if (m_languages.at(i).isSystem)
            return i;
    return -1;
}

int LanguageModel::indexForCode(const QString &code) const
{
    for (int i = 0; i < m_languages.size(); ++i)
        if (m_languages.at(i).code == code)
            return i;
    return -1;
}
