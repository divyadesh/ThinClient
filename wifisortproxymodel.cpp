#include "wifisortproxymodel.h"
#include "WifiNetworkDetailsColl.h"   // For Role IDs
#include <QDebug>

WifiSortProxyModel::WifiSortProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    sort(0);
}

/*
 * Custom sorting rules:
 * 1. ACTIVE == "yes" always goes to top
 * 2. Then sort by Bars (strength) descending
 * 3. Then by SSID alphabetically
 */
bool WifiSortProxyModel::lessThan(const QModelIndex &left,
                                  const QModelIndex &right) const
{
    auto Lactive = sourceModel()->data(left, WifiNetworkDetailsColl::RoleActive).toString();
    auto Ractive = sourceModel()->data(right, WifiNetworkDetailsColl::RoleActive).toString();

    auto Lbars = sourceModel()->data(left, WifiNetworkDetailsColl::RoleBars).toInt();
    auto Rbars = sourceModel()->data(right, WifiNetworkDetailsColl::RoleBars).toInt();

    auto Lssid = sourceModel()->data(left, WifiNetworkDetailsColl::RoleSsid).toString();
    auto Rssid = sourceModel()->data(right, WifiNetworkDetailsColl::RoleSsid).toString();

    // ---------------------------------------------------------
    // PRIORITY #1: Connected network ALWAYS goes to TOP
    // ---------------------------------------------------------
    if (Lactive == "yes" && Ractive != "yes")
        return true;

    if (Lactive != "yes" && Ractive == "yes")
        return false;

    // ---------------------------------------------------------
    // PRIORITY #2: Sort by signal strength (DESCENDING)
    // ---------------------------------------------------------
    if (Lbars != Rbars)
        return Lbars > Rbars;     // High → low

    // ---------------------------------------------------------
    // PRIORITY #3: Alphabetical fallback
    // ---------------------------------------------------------
    return Lssid < Rssid;
}


