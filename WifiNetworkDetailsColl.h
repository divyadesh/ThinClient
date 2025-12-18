#ifndef WIFINETWORKDETAILSCOLL_H
#define WIFINETWORKDETAILSCOLL_H

#include <QObject>
#include <QAbstractListModel>
#include <QProcess>
#include <QTimer>
#include <deque>
#include <memory>


struct ParsedInfo {
    QString active;
    QString ssid;
    int bars;
    bool secured;
    QString bssid;
    int chan;
    QString rate;
};

class WifiNetworkDetails;

/**
 * @class WifiNetworkDetailsColl
 * @brief Model class representing a list of available Wi-Fi networks.
 *
 * This class provides:
 *   • A QAbstractListModel exposing Wi-Fi scan results.
 *   • Asynchronous background scanning using nmcli.
 *   • Utility methods for connect/disconnect/forget operations.
 *   • Exposed QML properties for active SSID and signal bars.
 *   • Status message signals (info, warning, success, error).
 */
class WifiNetworkDetailsColl : public QAbstractListModel
{
    Q_OBJECT

    /**
     * @property activeSsid
     * @brief Current active Wi-Fi SSID.
     */
    Q_PROPERTY(QString activeSsid READ activeSsid WRITE setActiveSsid NOTIFY sigActiveSsidChanged FINAL)

    /**
     * @property activeBars
     * @brief Signal strength bars of current active Wi-Fi.
     */
    Q_PROPERTY(int activeBars READ activeBars WRITE setActiveBars NOTIFY sigActiveBarsChanged FINAL)

    /**
     * @property scanning
     * @brief Indicates whether an async Wi-Fi scan is running.
     */
    Q_PROPERTY(bool scanning READ scanning NOTIFY scanningChanged FINAL)

public:

    /**
     * @enum WifiListCollRole
     * @brief Role identifiers used by the model for QML.
     *
     * eWifiListCollectionRole returns the full WifiNetworkDetails object.
     * Remaining roles return individual properties for QML delegates.
     */
    enum WifiListCollRole {
        RoleActive = Qt::UserRole + 1,                ///< ACTIVE field
        RoleSsid,                                     ///< SSID field
        RoleBars,                                     ///< Signal bars
        RoleSecurity,                                 ///< Security enabled/disabled
        RoleBssid,                                    ///< BSSID of AP
        RoleChan,                                     ///< Channel number
        RoleRate                                      ///< Rate (bitrate)
    };
    Q_ENUM(WifiListCollRole)

    /**
     * @enum SignalStrength
     * @brief Categorized representation of Wi-Fi signal.
     */
    enum SignalStrength {
        StrengthNone = 0,
        StrengthWeak,
        StrengthFair,
        StrengthGood,
        StrengthExcellent
    };
    Q_ENUM(SignalStrength)

    explicit WifiNetworkDetailsColl(QObject *parent = nullptr);

    // --- QAbstractListModel overrides ---
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    // --- Public API exposed to QML ---
    Q_INVOKABLE void clear();
    Q_INVOKABLE void getWifiDetails();                        ///< Blocking scan
    Q_INVOKABLE void scanWifiNetworksAsync();                 ///< Non-blocking scan
    Q_INVOKABLE void startAutoRefresh(int intervalMs = 5000); ///< Start periodic scanning
    Q_INVOKABLE void stopAutoRefresh();                       ///< Stop periodic scanning
    Q_INVOKABLE void fetchActiveWifiDetails();                ///< Query active Wi-Fi status
    Q_INVOKABLE void connectToSsid(QString ssid, QString password);
    Q_INVOKABLE void disconnectWifiNetwork(const QString &ssid);
    Q_INVOKABLE void forgetWifiNetwork(const QString &ssid);
    Q_INVOKABLE QString getActiveWifiDevice();                ///< Returns device interface (e.g. wlan0)
    Q_INVOKABLE bool disconnectWifi();                        ///< Low-level disconnect

    // --- Properties accessors ---
    QString activeSsid() const;
    void setActiveSsid(const QString &newActiveSsid);

    int activeBars() const;
    void setActiveBars(const int &newActiveBars);

    bool scanning() const;

signals:

    // Property changed signals
    void sigActiveSsidChanged(QString activeSsid);
    void sigActiveBarsChanged(int activeBars);
    void scanningChanged();

    // Connection process signals
    void sigConnectionStarted();
    void sigConnectionFinished();

    // Data update signal
    void sigWifiListUpdated();

    // User-facing message signals
    void infoMessage(const QString &msg);
    void warningMessage(const QString &msg);
    void errorMessage(const QString &msg);
    void successMessage(const QString &msg);

private:
    // --- Internal state ---
    QProcess m_process;                  ///< Synchronous nmcli process
    QProcess m_asyncProcess;             ///< Async scan process
    QTimer m_autoRefreshTimer;           ///< Timer driving periodic scans

    QString m_activeSsid;                ///< Current SSID
    int m_activeBars{-1};                ///< Signal bars for current Wi-Fi
    bool m_scanning{false};              ///< True if async scan is running

    QList<WifiNetworkDetails*> m_wifiList;
    void parseScanOutput(const QStringList &lines, QVector<ParsedInfo> &parsed);
    void removeMissingEntries(const QVector<ParsedInfo> &parsed);
    void updateOrInsertEntries(const QVector<ParsedInfo> &parsed);
    void updateActiveInfo();
    void finalizeScanSuccess();
    void finalizeScanFailure();
    QVector<ParsedInfo> deduplicateParsedResults(const QVector<ParsedInfo> &input);
};

#endif // WIFINETWORKDETAILSCOLL_H
