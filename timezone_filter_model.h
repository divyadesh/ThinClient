#ifndef TIMEZONE_FILTER_MODEL_H
#define TIMEZONE_FILTER_MODEL_H

#include <QSortFilterProxyModel>

class TimezoneFilterModel : public QSortFilterProxyModel {
    Q_OBJECT
    Q_PROPERTY(QString filterString READ filterString WRITE setFilterString NOTIFY filterStringChanged)
public:
    explicit TimezoneFilterModel(QObject* parent = nullptr);

    QString filterString() const { return m_filter; }
    void setFilterString(const QString& s);

signals:
    void filterStringChanged();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
    QString m_filter;
};

#endif // TIMEZONE_FILTER_MODEL_H
