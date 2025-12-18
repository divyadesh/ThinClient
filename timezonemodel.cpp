#include "timezonemodel.h"
#include <QTimeZone>
#include <QString>
#include <QHash>
#include <QByteArray>
#include <QVariant>
#include <QDateTime>
#include <QSet>
#include <QFile>
#include <QDebug>
#include <unistd.h>
#include <QtConcurrent/QtConcurrent>

QString currentUtcOffset(const QByteArray &timeZoneId)
{
    const QTimeZone timeZone(timeZoneId);

    return timeZone.hasDaylightTime() && timeZone.isDaylightTime(QDateTime::currentDateTime())
               ? timeZone.displayName(QTimeZone::DaylightTime, QTimeZone::NameType::OffsetName)
               : timeZone.displayName(QTimeZone::StandardTime, QTimeZone::NameType::OffsetName);
}

static QString formatOffset(int seconds)
{
    int hours = seconds / 3600;
    int mins = (seconds % 3600) / 60;

    return QString("UTC%1%2:%3")
        .arg(hours >= 0 ? "+" : "")
        .arg(hours)
        .arg(abs(mins), 2, 10, QLatin1Char('0'));
}

bool compareTimeZones(const QByteArray &a, const QByteArray &b)
{
    QTimeZone tzA(a);
    QTimeZone tzB(b);

    int offsetA = tzA.offsetFromUtc(QDateTime::currentDateTime());
    int offsetB = tzB.offsetFromUtc(QDateTime::currentDateTime());

    // First compare UTC offsets
    if (offsetA != offsetB)
        return offsetA < offsetB;

    // If same offset, sort alphabetically
    return QString::fromUtf8(a) < QString::fromUtf8(b);
}

// ---------- MODEL ---------- //

TimeZoneModel::TimeZoneModel(QObject* parent) : QAbstractListModel(parent)
{
    (void)QtConcurrent::run([this]{
        const auto availableCountriesCount = static_cast<int>(QLocale::LastCountry);
        for (int i = 1; i <= availableCountriesCount; ++i)
        {
            auto country = static_cast<QLocale::Country>(i);
            _timeZones.append(QTimeZone::availableTimeZoneIds(country));
        }
        std::sort(_timeZones.begin(), _timeZones.end(), &compareTimeZones);

        std::reverse(_timeZones.begin(), _timeZones.end());
    });
}

QVariant TimeZoneModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= _timeZones.size())
    {
        return "undefined";
    }

    switch (role)
    {
    case TimeZoneDisplayOffset:
    {
        QByteArray id = _timeZones[index.row()];
        QTimeZone tz(id);
        auto now = QDateTime::currentDateTime();
        return formatOffset(tz.offsetFromUtc(now));
    }
    case TimeZoneDisplayName:
    {
        return _timeZones[index.row()];
    }
    case TimeZoneFullDisplay:
    {
        return QString(_timeZones[index.row()]);
    }
    case TimeZoneId:
    {
        return _timeZones[index.row()];
    }
    }

    return {};
}

int TimeZoneModel::rowCount(const QModelIndex &parent) const
{
    return _timeZones.size();
}

QHash<int, QByteArray> TimeZoneModel::roleNames() const
{
    return {
            {TimeZoneDisplayOffset, "timeZoneDisplayOffset"},
            {TimeZoneDisplayName,  "timeZoneDisplayName"},
            {TimeZoneFullDisplay,  "timeZoneFullDisplay"},
            {TimeZoneId,           "timeZoneId"},
            };
}

bool TimeZoneModel::setSystemTimezone(const QString &tzId)
{
    const QString zoneInfoPath = "/usr/share/zoneinfo/" + tzId;
    const QString localTimePath = "/etc/localtime";
    const QString timezoneFile  = "/etc/timezone";

    qInfo() << "[TimeZone] Requested timezone change to:" << tzId;

    // Check if zoneinfo file exists
    if (!QFile::exists(zoneInfoPath)) {
        qWarning() << "[TimeZone] ERROR: Zoneinfo file not found:" << zoneInfoPath;
        return false;
    }

    // Remove old /etc/localtime
    if (QFile::exists(localTimePath)) {
        if (!QFile::remove(localTimePath)) {
            qWarning() << "[TimeZone] ERROR: Could not remove old /etc/localtime";
            return false;
        }
    }

    // Create symlink
    if (::symlink(zoneInfoPath.toUtf8().constData(), localTimePath.toUtf8().constData()) != 0) {
        qWarning() << "[TimeZone] ERROR: Failed to create symlink to" << zoneInfoPath;
        return false;
    }

    qInfo() << "[TimeZone] Symlink created:" << localTimePath << "->" << zoneInfoPath;

    // Update /etc/timezone (optional)
    QFile file(timezoneFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(tzId.toUtf8());
        file.close();
        qInfo() << "[TimeZone] Updated /etc/timezone with" << tzId;
    }

    qInfo() << "[TimeZone] System timezone successfully set to:" << tzId;
    return true;
}

