#ifndef RDSERVERMODEL_H
#define RDSERVERMODEL_H

#include <QAbstractListModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariantList>
#include <QDebug>

class RdServerModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit RdServerModel(QObject *parent = nullptr);

    enum ServerRoles {
        ConnectionIdRole = Qt::UserRole + 1,
        ConnectionNameRole,
        ServerIpRole,
        DeviceNameRole,
        UserNameRole,
        PasswordRole,
        PerformanceRole,
        EnableAudioRole,
        EnableMicrophoneRole,
        RedirectDriveRole,
        RedirectUsbDeviceRole,
        SecurityRole,
        GatewayRole,
        GatewayIpRole,
        GatewayUserNameRole,
        GatewayPasswordRole,
        AutoConnectRole,
    };
    Q_ENUM(ServerRoles)

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // QML-accessible methods
    Q_INVOKABLE void loadFromDatabase();
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void removeServer(const QString &connectionId);
    Q_INVOKABLE void updateServer(const QString &connectionId);
    Q_INVOKABLE void addServer(); // optional trigger if you insert externally

private:
    struct ServerEntry {
        QString connectionId;
        QString connectionName;
        QString serverIp;
        QString deviceName;
        QString userName;
        QString password;
        QString performance;
        bool enableAudio;
        bool enableMicrophone;
        bool redirectDrive;
        bool redirectUsbDevice;
        QString security;
        QString gateway;
        QString gatewayIp;
        QString gatewayUserName;
        QString gatewayPassword;
        bool autoConnect = false;
    };

    QList<ServerEntry> m_servers;

    void fetchAll(); // internal reload helper
};

#endif // RDSERVERMODEL_H
