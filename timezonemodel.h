#ifndef TIMEZONEMODEL_H
#define TIMEZONEMODEL_H

#include <QAbstractListModel>
#include <QByteArrayList>
#include <QObject>
/*!
 * \class TimeZoneModel
 * \brief Model of available timezones on the system
 */
class TimeZoneModel : public QAbstractListModel
{
    Q_OBJECT
public:
    /*!
     * Construct a new TimeZoneModel instance
     */
    explicit TimeZoneModel(QObject* parent = nullptr);

    /*!
     * \brief See QAbstractItemModel::data
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    /*!
     * \brief See QAbstractItemModel::rowCount
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    /*!
     * \brief See QAbstractItemModel::roleNames
     */
    QHash<int, QByteArray> roleNames() const override;

    enum Roles {
        TimeZoneDisplayOffset = Qt::UserRole + 1,
        TimeZoneDisplayName,
        TimeZoneFullDisplay,
        TimeZoneId
    };

    Q_INVOKABLE bool setSystemTimezone(const QString &tzId);

private:
    QByteArrayList _timeZones;
};

#endif // TIMEZONEMODEL_H
