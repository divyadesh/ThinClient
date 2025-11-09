#include "timezone_model.h"
#include <QDirIterator>
#include <QFileInfo>
#include <QTimeZone>
#include <QDateTime>
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
    case UtcOffsetRole:
        return item.utcOffset;
    case IsSectionRole:
        return item.isSection;
    default:
        return {};
    }
}

QHash<int, QByteArray> TimezoneModel::roleNames() const {
    return {
        { TzIdRole, "tzId" },
        { TzNameRole, "tzName" },
        { UtcOffsetRole, "utcOffset" },
        { IsSectionRole, "isSection" }
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

QString TimezoneModel::offsetString(const QString &tzId) const {
    QTimeZone tz(tzId.toUtf8());
    if (!tz.isValid())
        return QStringLiteral("UTC±00:00");

    int offsetSecs = tz.offsetFromUtc(QDateTime::currentDateTimeUtc());
    int hours = offsetSecs / 3600;
    int mins = qAbs(offsetSecs % 3600) / 60;

    return QString("UTC%1%2:%3")
        .arg(hours >= 0 ? "+" : "-")
        .arg(qAbs(hours), 2, 10, QLatin1Char('0'))
        .arg(mins, 2, 10, QLatin1Char('0'));
}

void TimezoneModel::load() {
    beginResetModel();
    m_items.clear();

    QDir root(m_root);
    if (!root.exists()) {
        endResetModel();
        return;
    }

    // Collect zones grouped by offset
    QMap<QString, QVector<TimezoneItem>> grouped;

    QDirIterator it(m_root,
                    QDir::Files | QDir::NoSymLinks | QDir::Readable,
                    QDirIterator::Subdirectories);

    while (it.hasNext()) {
        const QString abs = it.next();
        const QString rel = root.relativeFilePath(abs);
        if (shouldSkip(rel))
            continue;

        if (QFileInfo(rel).suffix().isEmpty()) {
            QTimeZone tz(rel.toUtf8());
            if (!tz.isValid())
                continue;

            QString offset = offsetString(rel);
            TimezoneItem item;
            item.tzId = rel;
            item.tzName = rel.mid(rel.lastIndexOf('/') + 1);
            item.utcOffset = offset;
            item.isSection = false;

            grouped[offset].append(item);
        }
    }

    // Sort offsets and items
    const auto sortedOffsets = grouped.keys();
    for (const auto &offset : sortedOffsets) {
        auto zones = grouped[offset];
        std::sort(zones.begin(), zones.end(), [](const TimezoneItem &a, const TimezoneItem &b) {
            return QString::localeAwareCompare(a.tzId, b.tzId) < 0;
        });
        m_items += zones;
    }

    endResetModel();
}

QVariantMap TimezoneModel::get(int index) const {
    QVariantMap map;
    if (index < 0 || index >= m_items.size())
        return map;

    const auto &item = m_items.at(index);
    map["tzId"] = item.tzId;
    map["tzName"] = item.tzName;
    map["utcOffset"] = item.utcOffset;
    map["isSection"] = item.isSection;
    return map;
}
