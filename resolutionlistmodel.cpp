#include "resolutionlistmodel.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

static const int DEFAULT_REFRESH_HZ = 60;

/*
 * NOTE:
 * On Qt there is no cross-platform API to enumerate all supported
 * display modes. You must use platform-specific APIs on your Yocto
 * RK3328 image (DRM/KMS, fbdev, xrandr, etc.) and feed those modes
 * into this model via addBuiltinResolution().
 */

ResolutionListModel::ResolutionListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

// --- Static table-creation helper ----------------------------------------- //

bool ResolutionListModel::ensureResolutionTable(QSqlDatabase &db,
                                                QString *errorMessage)
{
    if (!db.isOpen()) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Database is not open.");
        }
        return false;
    }

    // Table schema:
    //
    // resolutions (
    //   id         INTEGER PRIMARY KEY AUTOINCREMENT,
    //   width      INTEGER NOT NULL,
    //   height     INTEGER NOT NULL,
    //   refresh_hz INTEGER DEFAULT 0,
    //   is_custom  INTEGER NOT NULL DEFAULT 0,
    //   is_active  INTEGER NOT NULL DEFAULT 0,
    //   created_at TEXT NOT NULL DEFAULT (datetime('now')),
    //   updated_at TEXT NOT NULL DEFAULT (datetime('now'))
    // );
    //
    // Plus simple index for fast lookups.
    QSqlQuery query(db);

    const char *createTableSql =
        "CREATE TABLE IF NOT EXISTS resolutions ("
        "  id         INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  width      INTEGER NOT NULL,"
        "  height     INTEGER NOT NULL,"
        "  refresh_hz INTEGER DEFAULT 0,"
        "  is_custom  INTEGER NOT NULL DEFAULT 0,"
        "  is_active  INTEGER NOT NULL DEFAULT 0,"
        "  created_at TEXT NOT NULL DEFAULT (datetime('now')),"
        "  updated_at TEXT NOT NULL DEFAULT (datetime('now'))"
        ");";

    if (!query.exec(QLatin1String(createTableSql))) {
        if (errorMessage) {
            *errorMessage = query.lastError().text();
        }
        return false;
    }

    const char *createIndexSql =
        "CREATE INDEX IF NOT EXISTS idx_resolutions_size "
        "ON resolutions(width, height, refresh_hz);";

    if (!query.exec(QLatin1String(createIndexSql))) {
        if (errorMessage) {
            *errorMessage = query.lastError().text();
        }
        return false;
    }

    return true;
}

// --- Basic model stuff ---------------------------------------------------- //

int ResolutionListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_rows.size();
}

QVariant ResolutionListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    const int row = index.row();
    if (row < 0 || row >= m_rows.size()) {
        return {};
    }

    const ResolutionRow &r = m_rows.at(row);

    switch (role) {
    case IdRole:       return r.id;
    case WidthRole:    return r.width;
    case HeightRole:   return r.height;
    case RefreshRole:  return r.refreshHz;
    case IsCustomRole: return r.isCustom;
    case IsActiveRole: return r.isActive;
    case TextRole:
        if (r.width == 0 && r.height == 0)
            return "Auto";
        return QString("%1 x %2").arg(r.width).arg(r.height);

    case ValueRole:
        if (r.width == 0 && r.height == 0)
            return "auto";
        return QString("%1x%2").arg(r.width).arg(r.height);
    default:
        return {};
    }
}

QHash<int, QByteArray> ResolutionListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole]       = "id";
    roles[WidthRole]    = "width";
    roles[HeightRole]   = "height";
    roles[RefreshRole]  = "refreshHz";
    roles[IsCustomRole] = "isCustom";
    roles[IsActiveRole] = "isActive";
    roles[TextRole]  = "text";
    roles[ValueRole] = "value";
    return roles;
}

// --- Configuration -------------------------------------------------------- //

void ResolutionListModel::setDatabase(const QSqlDatabase &db)
{
    m_db = db;
}

void ResolutionListModel::setMinimumSize(const QSize &size)
{
    if(m_minimumSize == size) {
        return;
    }

    m_minimumSize = size;
    emit minimumSizeChanged();
}

void ResolutionListModel::setMaximumSize(const QSize &size)
{
    if(m_maximumSize == size) {
        return;
    }

    m_maximumSize = size;
    emit maximumSizeChanged();
}

// --- Error handling ------------------------------------------------------- //

void ResolutionListModel::clearError()
{
    if (!m_hasError && m_errorMessage.isEmpty()) {
        return;
    }

    m_hasError = false;
    m_errorMessage.clear();
    emit hasErrorChanged();
    emit errorMessageChanged();
}

void ResolutionListModel::setError(const QString &message)
{
    bool changedFlag = (m_hasError != true);
    bool changedMsg  = (m_errorMessage != message);

    m_hasError = true;
    m_errorMessage = message;

    if (changedFlag) {
        emit hasErrorChanged();
    }
    if (changedMsg) {
        emit errorMessageChanged();
    }
}

// --- Validation ----------------------------------------------------------- //

bool ResolutionListModel::validateCustomResolution(int width, int height)
{
    if (width < m_minimumSize.width() || height < m_minimumSize.height() ||
        width > m_maximumSize.width() || height > m_maximumSize.height()) {

        setError(tr("Resolution %1x%2 is outside allowed range (%3x%4 - %5x%6).")
                     .arg(width)
                     .arg(height)
                     .arg(m_minimumSize.width())
                     .arg(m_minimumSize.height())
                     .arg(m_maximumSize.width())
                     .arg(m_maximumSize.height()));
        return false;
    }

    // Optional: reject duplicates
    for (const auto &row : m_rows) {
        if (row.width == width && row.height == height && row.refreshHz == 0) {
            setError(tr("Resolution %1x%2 already exists.").arg(width).arg(height));
            return false;
        }
    }

    clearError();
    return true;
}

bool ResolutionListModel::resolutionExists(int width, int height)
{
    if (!m_db.isOpen())
        return false;

    QSqlQuery q(m_db);
    q.prepare("SELECT id FROM resolutions WHERE width = :w AND height = :h LIMIT 1");
    q.bindValue(":w", width);
    q.bindValue(":h", height);

    if (!q.exec()) {
        qWarning() << "resolutionExists(): query failed:" << q.lastError();
        return false;
    }

    if (q.next()) {
        return true;
    }

    return false;
}

// --- DB operations for rows ---------------------------------------------- //

bool ResolutionListModel::insertRowToDatabase(const ResolutionRow &row, int *newIdOut)
{
    if (!m_db.isOpen()) {
        setError(tr("Database is not open."));
        return false;
    }

    QSqlQuery q(m_db);
    q.prepare(QLatin1String(
        "INSERT INTO resolutions(width, height, refresh_hz, is_custom, is_active) "
        "VALUES(:w, :h, :r, :custom, :active)"
        ));
    q.bindValue(":w",      row.width);
    q.bindValue(":h",      row.height);
    q.bindValue(":r",      row.refreshHz);
    q.bindValue(":custom", row.isCustom ? 1 : 0);
    q.bindValue(":active", row.isActive ? 1 : 0);

    if (!q.exec()) {
        setError(tr("Failed to insert resolution into database: %1")
                     .arg(q.lastError().text()));
        return false;
    }

    bool ok = false;
    const int newId = q.lastInsertId().toInt(&ok);
    if (!ok) {
        setError(tr("Failed to read inserted row ID."));
        return false;
    }

    if (newIdOut) {
        *newIdOut = newId;
    }

    return true;
}

bool ResolutionListModel::updateActiveFlagInDatabase(int activeId)
{
    if (!m_db.isOpen()) {
        setError(tr("Database is not open."));
        return false;
    }

    QSqlQuery q(m_db);

    // Clear active flag on all rows
    if (!q.exec(QLatin1String("UPDATE resolutions SET is_active = 0"))) {
        setError(tr("Failed to clear active resolution: %1")
                     .arg(q.lastError().text()));
        return false;
    }

    // Set active flag on selected row
    q.prepare(QLatin1String(
        "UPDATE resolutions SET is_active = 1, "
        "updated_at = datetime('now') "
        "WHERE id = :id"
        ));
    q.bindValue(":id", activeId);

    if (!q.exec()) {
        setError(tr("Failed to set active resolution: %1")
                     .arg(q.lastError().text()));
        return false;
    }

    clearError();
    return true;
}

// --- Public operations ---------------------------------------------------- //

bool ResolutionListModel::reloadFromDatabase()
{
    if (!m_db.isOpen()) {
        setError(tr("Database is not open."));
        return false;
    }

    QSqlQuery q(m_db);
    if (!q.exec(QLatin1String(
            "SELECT id, width, height, refresh_hz, is_custom, is_active "
            "FROM resolutions "
            "ORDER BY width, height, refresh_hz"))) {

        setError(tr("Failed to read resolutions table: %1")
                     .arg(q.lastError().text()));
        return false;
    }

    beginResetModel();
    m_rows.clear();

    while (q.next()) {
        ResolutionRow row;
        row.id        = q.value(0).toInt();
        row.width     = q.value(1).toInt();
        row.height    = q.value(2).toInt();
        row.refreshHz = q.value(3).toInt();
        row.isCustom  = (q.value(4).toInt() != 0);
        row.isActive  = (q.value(5).toInt() != 0);
        m_rows.push_back(row);
    }

    endResetModel();
    clearError();
    return true;
}

bool ResolutionListModel::addCustomResolution(int width, int height, int refreshHz)
{
    if (!validateCustomResolution(width, height)) {
        return false;
    }

    if (resolutionExists(width, height)) {
        setError(tr("Resolution %1x%2 already exists.")
                     .arg(width)
                     .arg(height));
        return false;
    }

    ResolutionRow row;
    row.width     = width;
    row.height    = height;
    row.refreshHz = refreshHz;
    row.isCustom  = true;
    row.isActive  = false;

    int newId = -1;
    if (!insertRowToDatabase(row, &newId)) {
        return false; // setError() already called
    }

    row.id = newId;

    const int insertRowIndex = m_rows.size();
    beginInsertRows(QModelIndex(), insertRowIndex, insertRowIndex);
    m_rows.push_back(row);
    endInsertRows();

    clearError();
    return true;
}

bool ResolutionListModel::builtinExists(int w, int h, int refreshHz)
{
    if (!m_db.isOpen())
        return false;

    QSqlQuery q(m_db);
    q.prepare("SELECT COUNT(*) FROM resolutions "
              "WHERE width = :w AND height = :h AND refresh_hz = :r AND is_custom = 0");
    q.bindValue(":w", w);
    q.bindValue(":h", h);
    q.bindValue(":r", refreshHz);

    if (!q.exec())
        return false;

    q.next();
    return q.value(0).toInt() > 0;
}

bool ResolutionListModel::addBuiltinResolution(int width, int height, int refreshHz, bool isActive)
{
    // avoid duplicates
    if (builtinExists(width, height, refreshHz)) {
        return true;
    }

    ResolutionRow row;
    row.width     = width;
    row.height    = height;
    row.refreshHz = refreshHz;
    row.isCustom  = false;
    row.isActive  = isActive;

    int newId = -1;
    if (!insertRowToDatabase(row, &newId)) {
        return false;
    }

    row.id = newId;
    const int insertRowIndex = m_rows.size();
    beginInsertRows(QModelIndex(), insertRowIndex, insertRowIndex);
    m_rows.push_back(row);
    endInsertRows();

    // If this is the active one, clear others and mark only this as active.
    if (isActive) {
        if (!updateActiveFlagInDatabase(row.id)) {
            // Error already set, but model is still consistent enough
            return false;
        }

        for (int i = 0; i < m_rows.size(); ++i) {
            const bool newActive = (i == insertRowIndex);
            if (m_rows[i].isActive != newActive) {
                m_rows[i].isActive = newActive;
                const QModelIndex idx = index(i, 0);
                emit dataChanged(idx, idx, { IsActiveRole });
            }
        }
    }

    clearError();
    return true;
}

bool ResolutionListModel::setActiveByRow(int row)
{
    if (row < 0 || row >= m_rows.size()) {
        setError(tr("Row %1 is out of range.").arg(row));
        return false;
    }

    ResolutionRow &target = m_rows[row];
    if (!updateActiveFlagInDatabase(target.id)) {
        return false; // setError already called
    }

    for (int i = 0; i < m_rows.size(); ++i) {
        bool newActive = (i == row);
        if (m_rows[i].isActive != newActive) {
            m_rows[i].isActive = newActive;
            const QModelIndex idx = index(i, 0);
            emit dataChanged(idx, idx, { IsActiveRole });
        }
    }

    clearError();
    return true;
}

QVariantMap ResolutionListModel::get(int index) const
{
    QVariantMap map;

    if (index < 0 || index >= m_rows.size())
        return map;

    const auto &r = m_rows.at(index);

    map.insert("id", r.id);
    map.insert("width", r.width);
    map.insert("height", r.height);
    map.insert("refreshHz", r.refreshHz);
    map.insert("isCustom", r.isCustom);
    map.insert("isActive", r.isActive);

    // UI roles
    if (r.width == 0 && r.height == 0) {
        map.insert("text", "Auto");
        map.insert("value", "auto");
    } else {
        map.insert("text", QString("%1 x %2").arg(r.width).arg(r.height));
        map.insert("value", QString("%1x%2").arg(r.width).arg(r.height));
    }

    return map;
}

void ResolutionListModel::init(const QString &dbPath)
{
    qDebug() << "Database init with path" << dbPath;

    // DO NOT CREATE A NEW CONNECTION
    m_db = QSqlDatabase::database("ThinClientConnection");

    if (!m_db.isOpen()) {
        if (!m_db.open()) {
            qWarning() << "ResolutionListModel: DB open failed:" << m_db.lastError().text();
            return;
        }
    }

    setMinimumSize(QSize(640, 480));
    setMaximumSize(QSize(3840, 2160));

    QString error;
    if (!ResolutionListModel::ensureResolutionTable(m_db, &error)) {
        qWarning() << "Failed to create resolutions table:" << error;
    }

    if (!reloadFromDatabase()) {
        qWarning() << "Failed to load resolutions:" << errorMessage();
    }

    // built-in resolutions
    addBuiltinResolution(1024, 768, 60, false);
    addBuiltinResolution(1280, 720, 60, false);
    addBuiltinResolution(1366, 768, 60, false);
    addBuiltinResolution(1440, 900, 60, false);
    addBuiltinResolution(1600, 900, 60, false);
    addBuiltinResolution(1680, 1050,60, false);
    addBuiltinResolution(1920, 1080,60, false);
}

void ResolutionListModel::setErrorMessage(const QString &newErrorMessage)
{
    if (m_errorMessage == newErrorMessage)
        return;
    m_errorMessage = newErrorMessage;
    emit errorMessageChanged();
}

void ResolutionListModel::setHasError(bool newHasError)
{
    if (m_hasError == newHasError)
        return;
    m_hasError = newHasError;
    emit hasErrorChanged();
}
