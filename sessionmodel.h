#ifndef SESSIONMODEL_H
#define SESSIONMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <optional>
#include <QAbstractListModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariantList>
#include <QDebug>

#include "connectioninfo.h"

/**
 * @class SessionModel
 * @brief Table model representing a list of remote connection sessions.
 *
 * This model is designed for use with a Qt Quick TableView. Each session is
 * represented by a ConnectionInfo object and exposed as a row with
 * four visible columns:
 *
 * | Column | Label              | Meaning                 |
 * |--------|--------------------|-------------------------|
 * | 0      | Connection Name    | Display name of session |
 * | 1      | Server IP          | Server address          |
 * | 2      | Auto               | Auto-connect enabled    |
 * | 3      | Manage Connection  | Action / controls       |
 *
 * All properties of ConnectionInfo are also exposed as roles so QML delegates
 * can bind directly to them. An additional role, @c columnType, is used by QML
 * to decide which delegate to choose ("name", "ip", "auto", "manage").
 */
class SessionModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    /**
     * @enum ConnectionRole
     * @brief Roles mapping directly to fields of ConnectionInfo.
     *
     * These roles allow QML to bind to properties even when they are not
     * directly displayed in the table columns.
     */
    enum ConnectionRole {
        RoleConnectionId = Qt::UserRole + 1, //!< Q_PROPERTY connectionId
        RoleConnectionName,                  //!< Q_PROPERTY connectionName
        RoleServerIp,                        //!< Q_PROPERTY serverIp
        RoleDeviceName,                      //!< Q_PROPERTY deviceName
        RoleUserName,                        //!< Q_PROPERTY userName
        RolePassword,                        //!< Q_PROPERTY password
        RolePerformance,                     //!< Q_PROPERTY performance
        RoleEnableAudio,                     //!< Q_PROPERTY enableAudio
        RoleEnableMicrophone,                //!< Q_PROPERTY enableMicrophone
        RoleRedirectDrive,                   //!< Q_PROPERTY redirectDrive
        RoleRedirectUsbDevice,               //!< Q_PROPERTY redirectUsbDevice
        RoleSecurity,                        //!< Q_PROPERTY security
        RoleGateway,                         //!< Q_PROPERTY gateway
        RoleGatewayIp,                       //!< Q_PROPERTY gatewayIp
        RoleGatewayUserName,                 //!< Q_PROPERTY gatewayUserName
        RoleGatewayPassword,                 //!< Q_PROPERTY gatewayPassword
        RoleAutoConnect,                     //!< Q_PROPERTY autoConnect

        /**
         * @brief Role used by QML DelegateChooser to decide which column delegate to use.
         *
         * Values are strings: "name", "ip", "auto", "manage".
         */
        RoleColumnType
    };
    Q_ENUM(ConnectionRole)

    Q_PROPERTY(QString autoConnectionId READ autoConnectionId WRITE setAutoConnectionId NOTIFY autoConnectionIdChanged FINAL)

public:
    /**
     * @brief Constructs an empty session model.
     * @param parent Optional QObject parent.
     */
    explicit SessionModel(QObject *parent = nullptr);

    /// @copydoc QAbstractItemModel::data()
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    /// @copydoc QAbstractItemModel::setData()
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    /// @copydoc QAbstractItemModel::flags()
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    /// @copydoc QAbstractItemModel::headerData()
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    /// @copydoc QAbstractItemModel::rowCount()
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @brief Returns the number of table columns.
     *
     * This model always uses 4 columns:
     * 0 = Connection Name, 1 = Server IP, 2 = Auto, 3 = Manage Connection.
     */
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @brief Returns QML-accessible role names.
     *
     * Includes roles for all ConnectionInfo properties and @c columnType.
     */
    QHash<int, QByteArray> roleNames() const override;

    /**
     * @brief Returns equal table column "size factors".
     *
     * In QML, your code uses:
     * @code
     * headerView.setColumnWidth(i, table.width / columnSource[i])
     * @endcode
     *
     * To get 4 equal columns, all factors must be the same (4.0).
     */
    Q_INVOKABLE QList<double> columnSizes() const;

    /**
     * @brief Clears all sessions from the model.
     */
    void clearSessions();

    /**
     * @brief Returns the session pointer at the given row.
     * @warning The model does not take ownership by default; caller must manage lifetime.
     */
    ConnectionInfo *sessionAt(int row) const;

    /**
     * @brief Returns all session pointers.
     */
    QList<ConnectionInfo *> sessions() const;

    /**
     * @brief Finds a session by its connection ID.
     *
     * @param connectionId ID to search for.
     * @return Optional pointer to the found ConnectionInfo, or std::nullopt if not found.
     */
    Q_INVOKABLE QObject* getSessionById(const QString &connectionId) const;

    /**
     * @brief Replaces model content with a new list of session pointers.
     *
     * The model does not assume ownership of the pointers. The caller must ensure that
     * the ConnectionInfo instances outlive the model, or alternatively set their parent
     * to this model externally.
     */
    void setSessions(const QList<ConnectionInfo *> &newSessions);

    /**
     * @brief Returns a symbolic column type for a given index.
     *
     * This is used internally to provide the @c columnType role. Typical values:
     * - "name"   for the Connection Name column
     * - "ip"     for the Server IP column
     * - "auto"   for the Auto column
     * - "manage" for the Manage Connection column
     */
    QVariant selectColumnType(const QModelIndex &index) const;
    QString autoConnectionId() const;

public slots:
    void setAutoConnectionId(const QString &newAutoConnectionId);
    // Core data operations
    void loadServers();
    void reloadServers();
    void fetchAllServers();

    // CRUD operations
    void deleteServer(const QString &connectionId);
    void updateServerById(const QString &connectionId);
    bool updateAutoConnect(const QString &connectionId, bool enabled);
    void addNewServer();

signals:
    void autoConnectionIdChanged();

private:
    QList<ConnectionInfo *> m_sessions;
    QString m_autoConnectionId;
};

#endif // SESSIONMODEL_H
