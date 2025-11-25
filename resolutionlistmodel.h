#pragma once

#include <QAbstractListModel>
#include <QSize>
#include <QSqlDatabase>
#include <QString>
#include <QVector>

/**
 * @brief Simple POD describing one resolution row.
 */
struct ResolutionRow
{
    int     id          = -1;
    int     width       = 0;
    int     height      = 0;
    int     refreshHz   = 0;
    bool    isCustom    = false;
    bool    isActive    = false;
};

/**
 * @brief Model exposing display resolutions stored in an SQLite table.
 *
 * Typical usage (C++):
 * @code
 * QSqlDatabase db = QSqlDatabase::database(); // already set up
 *
 * ResolutionListModel *model = new ResolutionListModel(this);
 * model->setDatabase(db);
 * model->setMinimumSize(QSize(640, 480));
 * model->setMaximumSize(QSize(3840, 2160));
 *
 * QString error;
 * if (!ResolutionListModel::ensureResolutionTable(db, &error)) {
 *     qWarning() << "Failed to create resolutions table:" << error;
 * }
 *
 * if (!model->reloadFromDatabase()) {
 *     qWarning() << "Failed to load resolutions:" << model->errorMessage();
 * }
 *
 * // Add custom resolution from UI / QML:
 * model->addCustomResolution(1280, 720, 60);
 * @endcode
 *
 * The model assumes an SQLite table named "resolutions" with one row per
 * supported resolution. Hardware-specific discovery of supported resolutions
 * (e.g. on a Yocto RK3328 board via DRM/KMS) should be implemented in
 * your platform code and then inserted into the table or via addBuiltinResolution().
 */
class ResolutionListModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(bool hasError READ hasError NOTIFY hasErrorChanged)

public:
    /**
     * @brief Model roles for QML / view bindings.
     */
    enum ResolutionRole {
        IdRole = Qt::UserRole + 1,
        WidthRole,
        HeightRole,
        RefreshRole,
        IsCustomRole,
        IsActiveRole,
        TextRole,
        ValueRole
    };
    Q_ENUM(ResolutionRole);

    explicit ResolutionListModel(QObject *parent = nullptr);
    ~ResolutionListModel() override = default;

    // --- Basic model API ---
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    // --- Configuration ---

    /**
     * @brief Attach an already-open QSqlDatabase.
     *
     * The database must stay valid for the lifetime of the model.
     */
    void setDatabase(const QSqlDatabase &db);

    /**
     * @brief Set minimum allowed resolution for custom entries.
     */
    void setMinimumSize(const QSize &size);

    /**
     * @brief Set maximum allowed resolution for custom entries.
     */
    void setMaximumSize(const QSize &size);

    /**
     * @brief Return minimum allowed resolution.
     */
    QSize minimumSize() const { return m_minSize; }

    /**
     * @brief Return maximum allowed resolution.
     */
    QSize maximumSize() const { return m_maxSize; }

    // --- Error state ---

    /**
     * @brief Last user-visible error message.
     */
    QString errorMessage() const { return m_errorMessage; }

    /**
     * @brief True if model is currently in an error state.
     */
    bool hasError() const { return m_hasError; }

    // --- DB helpers ---

    /**
     * @brief Ensure that the resolutions table exists with all required columns.
     *
     * @param db SQLite database handle (must be open).
     * @param errorMessage Optional error description on failure.
     * @return true on success; false on SQL error.
     */
    static bool ensureResolutionTable(QSqlDatabase &db, QString *errorMessage = nullptr);

    /**
     * @brief Reload rows from the resolutions table into the model.
     *
     * Clears current contents and repopulates from the database.
     *
     * @return true on success.
     */
    Q_INVOKABLE bool reloadFromDatabase();

    /**
     * @brief Insert a custom resolution (validated against min/max) into DB + model.
     *
     * If validation fails, hasError() is set and false is returned.
     *
     * @param width Width in pixels.
     * @param height Height in pixels.
     * @param refreshHz Refresh rate in Hz (may be 0 if unknown).
     * @return true on success.
     */
    Q_INVOKABLE bool addCustomResolution(int width, int height, int refreshHz);

    /**
     * @brief Insert a built-in (non-custom) resolution into DB + model.
     *
     * This is intended for modes obtained from the device (DRM/KMS, etc.).
     */
    bool addBuiltinResolution(int width, int height, int refreshHz, bool isActive = false);

    /**
     * @brief Mark a resolution row as active in DB and model.
     *
     * Only one resolution will be active at a time (others are cleared).
     */
    Q_INVOKABLE bool setActiveByRow(int row);

    Q_INVOKABLE QVariantMap get(int index) const;

    Q_INVOKABLE bool builtinExists(int w, int h, int refreshHz);

    void init(const QString &dbPath);

signals:
    /**
     * @brief Emitted when the user-visible error message changes.
     */
    void errorMessageChanged();

    /**
     * @brief Emitted when the error state flag changes.
     */
    void hasErrorChanged();

private:
    void clearError();
    void setError(const QString &message);

    bool validateCustomResolution(int width, int height);
    bool insertRowToDatabase(const ResolutionRow &row, int *newIdOut);
    bool updateActiveFlagInDatabase(int activeId);

    QSqlDatabase            m_db;
    QSize                   m_minSize { 640, 480 };
    QSize                   m_maxSize { 3840, 2160 };
    QVector<ResolutionRow>  m_rows;

    QString                 m_errorMessage;
    bool                    m_hasError = false;
};

