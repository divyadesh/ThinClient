// timezone_model.h
#pragma once

#include <QAbstractListModel>
#include <QTimeZone>

class TimeZoneModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum TimeZoneRoles { IdRole = Qt::UserRole + 1, NameRole, OffsetRole };

    explicit TimeZoneModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE QVariantMap get(int row) const
    {
        QVariantMap map;
        if (row < 0 || row >= m_timeZones.size())
            return map;

        QByteArray tzId = m_timeZones.at(row);
        QTimeZone tz(tzId);

        map["tzId"] = QString::fromUtf8(tzId);
        map["tzName"] = tz.displayName(QTimeZone::StandardTime, QTimeZone::LongName);
        map["tzOffset"] = tz.offsetFromUtc(QDateTime::currentDateTime()) / 3600.0;
        return map;
    }

private:
    QList<QByteArray> m_timeZones;
};
