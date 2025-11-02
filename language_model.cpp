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

    // Gather available locales
    const QList<QLocale> locales = QLocale::matchingLocales(
        QLocale::AnyLanguage, QLocale::AnyScript, QLocale::AnyCountry);

    QSet<QString> added; // prevent duplicates

    for (const QLocale &loc : locales) {
        QString id = loc.name(); // e.g. "en_US"
        if (added.contains(id))
            continue;

        added.insert(id);
        QString name = QString("%1 (%2)")
                           .arg(QLocale::languageToString(loc.language()))
                           .arg(QLocale::countryToString(loc.country()));

        LanguageItem item;
        item.localeId = id;
        item.displayName = name;
        m_items.push_back(item);
    }

    std::sort(m_items.begin(), m_items.end(),
              [](const LanguageItem &a, const LanguageItem &b) {
                  return a.displayName.localeAwareCompare(b.displayName) < 0;
              });

    endResetModel();
}
