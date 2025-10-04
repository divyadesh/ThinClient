#ifndef WIFINETWORKDETAILSCOLL_H
#define WIFINETWORKDETAILSCOLL_H

#include <QObject>
#include <QProcess>
#include <deque>
#include <memory>
#include "qabstractitemmodel.h"

class WifiNetworkDetails;
class WifiNetworkDetailsColl : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString activeSsid READ activeSsid WRITE setActiveSsid NOTIFY sigActiveSsidChanged FINAL)
    Q_PROPERTY(int activeBars READ activeBars WRITE setActiveBars NOTIFY sigActiveBarsChanged FINAL)

    QProcess m_process;
    std::deque<std::shared_ptr<WifiNetworkDetails>> m_WifiDetailsColl;

    QString m_activeSsid;
    int m_activeBars{-1};
public:
    enum WifiListCollRole {
        eWifiListCollectionRole = Qt::UserRole + 1
    };
    Q_ENUM(WifiListCollRole)

    enum SignalStrength {
        StrengthNone = 0,
        StrengthWeak,
        StrengthFair,
        StrengthGood,
        StrengthExcellent
    };
    Q_ENUM(SignalStrength)

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &refModelIndex = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const override;

    explicit WifiNetworkDetailsColl(QObject *parent = nullptr);

    Q_INVOKABLE void clear();
    Q_INVOKABLE void getWifiDetails();
    Q_INVOKABLE void connectToSsid(QString ssid, QString password);
    Q_INVOKABLE void disconnectWifiNetwork(const QString &ssid);
    /*Q_INVOKABLE QString getActiveSsid();
    Q_INVOKABLE QString getActiveBars()/*/

    QString activeSsid() const;
    void setActiveSsid(const QString &newActiveSsid);

    int activeBars() const;
    void setActiveBars(const int &newActiveBars);

    void fetchActiveWifiDetails();

signals:
    void sigActiveSsidChanged(QString activeSsid);
    void sigActiveBarsChanged(int activeBars);
    void sigConnectionStarted();
    void sigConnectionFinished();
};

#endif // WIFINETWORKDETAILSCOLL_H
