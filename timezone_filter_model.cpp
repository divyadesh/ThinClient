#include "timezone_filter_model.h"
#include <QAbstractItemModel>
#include <QModelIndex>

TimezoneFilterModel::TimezoneFilterModel(QObject* parent)
    : QSortFilterProxyModel(parent) {
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setDynamicSortFilter(true);
}

void TimezoneFilterModel::setFilterString(const QString& s) {
    if (m_filter == s) return;
    m_filter = s;
    emit filterStringChanged();
    invalidateFilter();
}

bool TimezoneFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const {
    if (m_filter.isEmpty()) return true;
    const QModelIndex idx = sourceModel()->index(sourceRow, 0, sourceParent);
    const QString name = sourceModel()->data(idx, Qt::UserRole + 1).toString();
    return name.contains(m_filter, Qt::CaseInsensitive);
}
