#ifndef TIMEZONE_MODEL_H
#define TIMEZONE_MODEL_H

#include <QAbstractListModel>
#include <QVector>
#include <QString>

/**
 * @brief Represents a single timezone entry or a section header.
 */
struct TimezoneItem {
    QString tzId;        ///< Full timezone ID, e.g. "Asia/Tokyo". For section headers this is the UTC offset.
    QString tzName;      ///< Display name (timezone name or offset section label).
    QString utcOffset;   ///< UTC offset string, e.g. "UTC+05:30".
    bool isSection;      ///< True if this entry is a section header (offset group).
};

/**
 * @brief A model that lists system timezones grouped by UTC offset.
 *
 * The model organizes timezones into sections based on their current UTC offset.
 * Each section (offset group) is represented by a non-selectable header item.
 *
 * Example structure:
 * @code
 * UTC+02:00   (section)
 *    Europe/Berlin
 *    Africa/Cairo
 *
 * UTC+05:30   (section)
 *    Asia/Kolkata
 * @endcode
 *
 * This structure is ideal for ComboBox/ListView where section headers are displayed
 * but remain unselectable.
 */
class TimezoneModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(QString rootPath READ rootPath WRITE setRootPath NOTIFY rootPathChanged)

public:
    /**
     * @brief Data roles exposed to QML.
     */
    enum Roles {
        TzIdRole = Qt::UserRole + 1,  ///< Full timezone ID
        TzNameRole,                   ///< Display name
        UtcOffsetRole,                ///< Offset string
        IsSectionRole                 ///< Whether this row is a section header
    };

    explicit TimezoneModel(QObject *parent = nullptr);

    // QAbstractItemModel overrides
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    /**
     * @brief Reload the model from disk.
     */
    Q_INVOKABLE void refresh();

    /**
     * @brief Retrieve an item by index.
     * @param index Row index.
     * @return QVariantMap representing a TimezoneItem.
     */
    Q_INVOKABLE QVariantMap get(int index) const;

    /**
     * @brief Returns the root path used for reading system timezone files.
     */
    QString rootPath() const { return m_root; }

    /**
     * @brief Sets the root directory containing timezone data.
     */
    void setRootPath(const QString &path);

signals:
    /**
     * @brief Emitted when the root timezone directory path changes.
     */
    void rootPathChanged();

private:
    void load();                        ///< Loads and organizes the timezone data.
    QString offsetString(const QString &tzId) const; ///< Generates "UTC±HH:MM" strings.
    static bool shouldSkip(const QString &relPath);  ///< Determines whether a file should be ignored.

private:
    QString m_root = QStringLiteral("/usr/share/zoneinfo");
    QVector<TimezoneItem> m_items;
};

#endif // TIMEZONE_MODEL_H
