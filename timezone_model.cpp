// timezone_model.cpp
#include "timezone_model.h"

TimeZoneModel::TimeZoneModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_timeZones = QTimeZone::availableTimeZoneIds();
}

int TimeZoneModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_timeZones.size();
}

QVariant TimeZoneModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_timeZones.size())
        return QVariant();

    QByteArray tzId = m_timeZones.at(index.row());
    QTimeZone tz(tzId);

    switch (role) {
    case IdRole:
        return QString::fromUtf8(tzId);
    case NameRole:
        return tz.displayName(QTimeZone::StandardTime, QTimeZone::LongName);
    case OffsetRole:
        return tz.offsetFromUtc(QDateTime::currentDateTime()) / 3600.0; // hours offset
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> TimeZoneModel::roleNames() const
{
    return {{IdRole, "tzId"}, {NameRole, "tzName"}, {OffsetRole, "tzOffset"}};
}
