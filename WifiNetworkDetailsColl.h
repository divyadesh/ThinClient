#ifndef WIFINETWORKDETAILSCOLL_H
#define WIFINETWORKDETAILSCOLL_H

#include <QObject>
#include <QProcess>
#include <deque>
#include "qabstractitemmodel.h"

class WifiNetworkDetails;
class WifiNetworkDetailsColl : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString activeSsid READ activeSsid WRITE setActiveSsid NOTIFY sigActiveSsidChanged FINAL)
    Q_PROPERTY(QString activeBars READ activeBars WRITE setActiveBars NOTIFY sigActiveBarsChanged FINAL)

    QProcess m_process;
    std::deque<std::shared_ptr<WifiNetworkDetails>> m_WifiDetailsColl;
public:
    enum WifiListCollRole {
        eWifiListCollectionRole = Qt::UserRole + 1
    };
    Q_ENUM(WifiListCollRole)

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &refModelIndex = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const override;

    explicit WifiNetworkDetailsColl(QObject *parent = nullptr);

    Q_INVOKABLE void getWifiDetails();
    /*Q_INVOKABLE QString getActiveSsid();
    Q_INVOKABLE QString getActiveBars()/*/

    QString activeSsid() const;
    void setActiveSsid(const QString &newActiveSsid);

    QString activeBars() const;
    void setActiveBars(const QString &newActiveBars);

signals:
    void sigActiveSsidChanged(QString activeSsid);
    void sigActiveBarsChanged(QString activeBars);

private:
    QString m_activeSsid;
    QString m_activeBars;
};

#endif // WIFINETWORKDETAILSCOLL_H
