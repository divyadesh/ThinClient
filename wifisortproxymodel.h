#pragma once

#include <QSortFilterProxyModel>

class WifiSortProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit WifiSortProxyModel(QObject *parent = nullptr);

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};
