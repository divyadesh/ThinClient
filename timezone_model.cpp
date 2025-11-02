#include "timezone_model.h"
#include <QDirIterator>
#include <QFileInfo>
#include <algorithm>

static bool shouldSkip(const QString &relPath) {
    static const QStringList skipNames = {
        QStringLiteral("zone.tab"),
        QStringLiteral("zone1970.tab"),
        QStringLiteral("leapseconds"),
        QStringLiteral("localtime"),
        QStringLiteral("posixrules"),
        QStringLiteral("tzdata.zi")
    };
    const QString base = QFileInfo(relPath).fileName();
    if (skipNames.contains(base))
        return true;

    if (relPath.startsWith(QStringLiteral("posix/"), Qt::CaseInsensitive)) return true;
    if (relPath.startsWith(QStringLiteral("right/"), Qt::CaseInsensitive)) return true;
    if (relPath.startsWith(QStringLiteral("SystemV/"), Qt::CaseInsensitive)) return true;

    return false;
}

TimezoneModel::TimezoneModel(QObject *parent)
    : QAbstractListModel(parent)
{
    load();
}

int TimezoneModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return m_items.size();
}

QVariant TimezoneModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size())
        return {};

    const auto &item = m_items.at(index.row());

    switch (role) {
    case TzIdRole:
        return item.tzId;
    case TzNameRole:
        return item.tzName;
    default:
        return {};
    }
}

QHash<int, QByteArray> TimezoneModel::roleNames() const {
    return {
        { TzIdRole, "tzId" },
        { TzNameRole, "tzName" }
    };
}

void TimezoneModel::setRootPath(const QString &path) {
    if (m_root == path)
        return;
    m_root = path;
    emit rootPathChanged();
    load();
}

void TimezoneModel::refresh() {
    load();
}

void TimezoneModel::load() {
    beginResetModel();
    m_items.clear();

    QDir root(m_root);
    if (!root.exists()) {
        endResetModel();
        return;
    }

    QDirIterator it(m_root,
                    QDir::Files | QDir::NoSymLinks | QDir::Readable,
                    QDirIterator::Subdirectories);

    while (it.hasNext()) {
        const QString abs = it.next();
        const QString rel = root.relativeFilePath(abs);
        if (shouldSkip(rel))
            continue;

        if (QFileInfo(rel).suffix().isEmpty()) {
            TimezoneItem item;
            item.tzId = rel;
            // Human-readable name: last component after "/"
            int slashIdx = rel.lastIndexOf('/');
            item.tzName = (slashIdx >= 0) ? rel.mid(slashIdx + 1) : rel;
            m_items.push_back(item);
        }
    }

    std::sort(m_items.begin(), m_items.end(), [](const TimezoneItem &a, const TimezoneItem &b) {
        return QString::localeAwareCompare(a.tzId, b.tzId) < 0;
    });

    endResetModel();
}


QVariantMap TimezoneModel::get(int index) const {
    QVariantMap map;
    if (index < 0 || index >= m_items.size())
        return map;

    const auto &item = m_items.at(index);
    map["tzId"] = item.tzId;
    map["tzName"] = item.tzName;
    return map;
}
