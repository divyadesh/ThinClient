#include "language_model.h"
#include <QDebug>
#include <QSet>

LanguageModel::LanguageModel(QObject *parent)
    : QAbstractListModel(parent)
{
    load();
}

int LanguageModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return m_items.size();
}

QVariant LanguageModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size())
        return {};

    const auto &item = m_items.at(index.row());

    switch (role) {
    case LocaleIdRole:
        return item.localeId;
    case DisplayNameRole:
        return item.displayName;
    default:
        return {};
    }
}

QHash<int, QByteArray> LanguageModel::roleNames() const {
    return {
        { LocaleIdRole, "localeId" },
        { DisplayNameRole, "displayName" }
    };
}

QVariantMap LanguageModel::get(int index) const {
    QVariantMap map;
    if (index < 0 || index >= m_items.size())
        return map;

    const auto &item = m_items.at(index);
    map["localeId"] = item.localeId;
    map["displayName"] = item.displayName;

    return map;
}

void LanguageModel::refresh() {
    load();
}

void LanguageModel::load() {
    beginResetModel();
    m_items.clear();

    // --- Fixed list of supported languages ---
    struct Lang {
        QString id;
        QString name;
    };

    static const QList<Lang> supported = {
        { "en_US", "English" },
        { "fr_FR", "French" },
        { "es_ES", "Spanish" },
        { "ru_RU", "Russian" },
        { "de_DE", "German" },
        { "ar_SA", "Arabic" },
        { "zh_CN", "Mandarin Chinese" }
    };

    for (const auto &lang : supported) {
        LanguageItem item;
        item.localeId = lang.id;
        item.displayName = lang.name;
        m_items.push_back(item);
    }

    // Default English first; rest sorted alphabetically
    std::sort(m_items.begin() + 1, m_items.end(),
              [](const LanguageItem &a, const LanguageItem &b) {
                  return a.displayName.localeAwareCompare(b.displayName) < 0;
              });

    endResetModel();
}
