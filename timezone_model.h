#ifndef TIMEZONE_MODEL_H
#define TIMEZONE_MODEL_H

#include <QAbstractListModel>
#include <QVector>
#include <QString>

struct TimezoneItem {
    QString tzId;   // Full zone identifier, e.g. "Asia/Kolkata"
    QString tzName; // Display name, e.g. "Asia/Kolkata" or just "Kolkata"
};

class TimezoneModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(QString rootPath READ rootPath WRITE setRootPath NOTIFY rootPathChanged)
public:
    enum Roles {
        TzIdRole = Qt::UserRole + 1,
        TzNameRole
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

    QString m_root = QStringLiteral("/usr/share/zoneinfo");
    QVector<TimezoneItem> m_items;
};

#endif // TIMEZONE_MODEL_H
