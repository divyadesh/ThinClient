#ifndef TIMEZONE_MODEL_H
#define TIMEZONE_MODEL_H

#include <QAbstractListModel>
#include <QVector>
#include <QString>
#include <QVariant>

struct TimezoneItem {
    QString tzId;       // e.g. "Asia/Kolkata"
    QString tzName;     // e.g. "Kolkata"
    QString utcOffset;  // e.g. "UTC+05:30"
    bool isSection;     // true for group header
};

class TimezoneModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(QString rootPath READ rootPath WRITE setRootPath NOTIFY rootPathChanged)
public:
    enum Roles {
        TzIdRole = Qt::UserRole + 1,
        TzNameRole,
        UtcOffsetRole,
        IsSectionRole
    };

    explicit TimezoneModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void refresh();
    Q_INVOKABLE QVariantMap get(int index) const;

    QString rootPath() const { return m_root; }
    void setRootPath(const QString &path);

signals:
    void rootPathChanged();

private:
    void load();
    QString offsetString(const QString &tzId) const; // helper

    QString m_root = QStringLiteral("/usr/share/zoneinfo");
    QVector<TimezoneItem> m_items;
};

#endif // TIMEZONE_MODEL_H
