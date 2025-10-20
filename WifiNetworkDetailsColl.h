#ifndef WIFINETWORKDETAILSCOLL_H
#define WIFINETWORKDETAILSCOLL_H

#include <QObject>
#include <QProcess>
#include <QTimer>
#include <deque>
#include <memory>
#include "qabstractitemmodel.h"

class WifiNetworkDetails;

class WifiNetworkDetailsColl : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString activeSsid READ activeSsid WRITE setActiveSsid NOTIFY sigActiveSsidChanged FINAL)
    Q_PROPERTY(int activeBars READ activeBars WRITE setActiveBars NOTIFY sigActiveBarsChanged FINAL)
    Q_PROPERTY(bool scanning READ scanning NOTIFY scanningChanged)

    QProcess m_process;                 // existing process for synchronous use
    QProcess m_asyncProcess;            // new async process (non-blocking)
    QTimer m_autoRefreshTimer;          // timer for periodic refresh
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

    explicit WifiNetworkDetailsColl(QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &refModelIndex = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    Q_INVOKABLE void clear();
    Q_INVOKABLE void getWifiDetails();  // existing blocking version
    Q_INVOKABLE void connectToSsid(QString ssid, QString password);
    Q_INVOKABLE void disconnectWifiNetwork(const QString &ssid);
    Q_INVOKABLE void fetchActiveWifiDetails();

    // ✅ New non-blocking methods
    Q_INVOKABLE void getWifiDetailsAsync();              // async one-shot refresh
    Q_INVOKABLE void startAutoRefresh(int intervalMs = 5000);   // start periodic background scanning
    Q_INVOKABLE void stopAutoRefresh();                  // stop it

    QString activeSsid() const;
    void setActiveSsid(const QString &newActiveSsid);

    int activeBars() const;
    void setActiveBars(const int &newActiveBars);

    bool scanning() const;

signals:
    void sigActiveSsidChanged(QString activeSsid);
    void sigActiveBarsChanged(int activeBars);
    void sigConnectionStarted();
    void sigConnectionFinished();
    void sigWifiListUpdated(); // ✅ emitted after each async/auto refresh
    void scanningChanged();
private:
    bool m_scanning;
};

#endif // WIFINETWORKDETAILSCOLL_H
