#include "timezone_model.h"

#include <QDirIterator>
#include <QFileInfo>
#include <QTimeZone>
#include <QDateTime>
#include <algorithm>

/**
 * @brief List of timezone files and directories that should not be processed.
 */
bool TimezoneModel::shouldSkip(const QString &relPath)
{
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

    if (relPath.startsWith("posix/", Qt::CaseInsensitive)) return true;
    if (relPath.startsWith("right/", Qt::CaseInsensitive)) return true;
    if (relPath.startsWith("SystemV/", Qt::CaseInsensitive)) return true;

    return false;
}

TimezoneModel::TimezoneModel(QObject *parent)
    : QAbstractListModel(parent)
{
    load();
}

int TimezoneModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_items.size();
}

QVariant TimezoneModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size())
        return {};

    const auto &item = m_items.at(index.row());
    switch (role) {
    case TzIdRole:      return item.tzId;
    case TzNameRole:    return item.tzName;
    case UtcOffsetRole: return item.utcOffset;
    case IsSectionRole: return item.isSection;
    default:            return {};
    }
}

QHash<int, QByteArray> TimezoneModel::roleNames() const
{
    return {
        { TzIdRole,      "tzId" },
        { TzNameRole,    "tzName" },
        { UtcOffsetRole, "utcOffset" },
        { IsSectionRole, "isSection" }
    };
}

void TimezoneModel::setRootPath(const QString &path)
{
    if (m_root == path)
        return;

    m_root = path;
    emit rootPathChanged();
    load();
}

void TimezoneModel::refresh()
{
    load();
}

QString TimezoneModel::offsetString(const QString &tzId) const
{
    QTimeZone tz(tzId.toUtf8());
    if (!tz.isValid())
        return QStringLiteral("UTC+00:00");

    const int offset = tz.offsetFromUtc(QDateTime::currentDateTimeUtc());
    const int hours = offset / 3600;
    const int minutes = qAbs(offset % 3600) / 60;

    return QString("UTC%1%2:%3")
        .arg(hours >= 0 ? "+" : "-")
        .arg(qAbs(hours), 2, 10, QLatin1Char('0'))
        .arg(minutes,      2, 10, QLatin1Char('0'));
}

void TimezoneModel::load()
{
    beginResetModel();
    m_items.clear();

    QDir root(m_root);
    if (!root.exists()) {
        endResetModel();
        return;
    }

    QMap<QString, QVector<TimezoneItem>> grouped;

    // Iterate through timezone directory recursively
    QDirIterator it(
        m_root,
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

            const QString offset = offsetString(rel);

            TimezoneItem item;
            item.tzId = rel;
            item.tzName = rel.mid(rel.lastIndexOf('/') + 1);
            item.utcOffset = offset;
            item.isSection = false;

            grouped[offset].append(item);
        }
    }

    // Insert section header + timezone items
    const auto sortedOffsets = grouped.keys();

    for (const QString &offset : sortedOffsets) {

        // ---- Add section header ----
        TimezoneItem section;
        section.tzId = offset;
        section.tzName = offset;
        section.utcOffset = offset;
        section.isSection = true;
        m_items.append(section);

        // ---- Add sorted timezone entries ----
        auto &zones = grouped[offset];
        std::sort(zones.begin(), zones.end(),
                  [](const TimezoneItem &a, const TimezoneItem &b) {
                      return QString::localeAwareCompare(a.tzId, b.tzId) < 0;
                  });

        for (const auto &z : zones)
            m_items.append(z);
    }

    endResetModel();
}

QVariantMap TimezoneModel::get(int index) const
{
    QVariantMap map;
    if (index < 0 || index >= m_items.size())
        return map;

    const auto &item = m_items.at(index);
    map["tzId"]      = item.tzId;
    map["tzName"]    = item.tzName;
    map["utcOffset"] = item.utcOffset;
    map["isSection"] = item.isSection;
    return map;
}
