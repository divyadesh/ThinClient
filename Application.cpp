#include "Application.h"
#include <QQmlContext>
#include <QTimer>
#include "wifimanager.h"

#include <QFile>
#include <QDir>
#include <QProcess>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>


Application *Application::s_instance = nullptr;

// ----------------------
// Singleton Entry Points
// ----------------------
void Application::initialize(QQmlApplicationEngine *engine)
{
    if (!s_instance)
        s_instance = new Application(engine);
}

Application *Application::instance() { return s_instance; }

void Application::destroy()
{
    delete s_instance;
    s_instance = nullptr;
}

// ----------------------
// Static Accessor Methods
// ----------------------

// ---------------------------
// 🔹 Core Persistent Managers
// ---------------------------

PersistData*            Application::persistData()              { return s_instance ? s_instance->_persistData              : nullptr; }
WiFiSettingsManager*    Application::wifiSettings()             { return s_instance ? s_instance->_wifiSettings             : nullptr; }
DeviceSettings*         Application::deviceSettings()           { return s_instance ? s_instance->_deviceSettings           : nullptr; }
AppSettings*            Application::appSettings()              { return s_instance ? s_instance->_appSettings              : nullptr; }
DataBase*               Application::database()                 { return s_instance ? s_instance->_database                 : nullptr; }

// ---------------------------
// 🔹 Network & Connectivity
// ---------------------------
WifiMonitor*            Application::wifiMonitor()              { return s_instance ? s_instance->m_wifiMonitor             : nullptr; }
EthernetMonitor*        Application::ethernetMonitor()          { return s_instance ? s_instance->m_ethMonitor              : nullptr; }
WifiNetworkDetailsColl* Application::wifiNetworkDetails()       { return s_instance ? s_instance->_wifiNetworkDetailsColl   : nullptr; }
EthernetNetworkConroller* Application::ethernetNetworkController() { return s_instance ? s_instance->_ethernetNetworkController : nullptr; }
DNSNetworkInfo*         Application::dnsNetworkInfo()           { return s_instance ? s_instance->m_dnsNetworkInfo          : nullptr; }

// ---------------------------
// 🔹 UI / Branding
// ---------------------------
LogoLoader*             Application::logoLoader()               { return s_instance ? s_instance->m_logoLoader              : nullptr; }

// ---------------------------
// 🔹 Models & Language Support
// ---------------------------
LanguageModel*          Application::languageModel()            { return s_instance ? s_instance->_languageModel            : nullptr; }
TimeZoneModel*          Application::timezoneModel()            { return s_instance ? s_instance->_tzModel                 : nullptr; }
TimezoneFilterModel*    Application::timezoneProxy()            { return s_instance ? s_instance->_proxy                   : nullptr; }

// ---------------------------
// 🔹 System Hardware & USB
// ---------------------------
UdevMonitor*            Application::usbMonitor()               { return s_instance ? s_instance->_monitor                 : nullptr; }
DeviceInfo*             Application::deviceInfo()               { return s_instance ? s_instance->_deviceInfo              : nullptr; }

// ---------------------------
// 🔹 Server / Backend Config
// ---------------------------
ServerInfoColl*         Application::serverInfo()               { return s_instance ? s_instance->_serverInfoColl          : nullptr; }
RdServerModel*          Application::serverModel()              { return s_instance ? s_instance->_serverModel             : nullptr; }
DataBase*               Application::db()                       { return s_instance ? s_instance->_database                : nullptr; }

WiFiAddNetworkManager *Application::wiFiAddNetworkManager()     { return s_instance ? s_instance->_wiFiAddNetworkManager   : nullptr; }

ResolutionListModel *Application::resolutionListModel()         { return s_instance ? s_instance->_resolutionListModel   : nullptr; }

SessionModel *Application::sessionModel()                       { return s_instance ? s_instance->_sessionModel   : nullptr; }

WifiNetworkInfo *Application::wifiNetworkInfo()                 { return s_instance ? s_instance->_wifiNetworkInfo   : nullptr; }

EthernetNetworkInfo *Application::ethernetNetworkInfo()         { return s_instance ? s_instance->_ethernetNetworkInfo   : nullptr; }

WifiConfigManager *Application::wifiConfigManager()             { return s_instance ? s_instance->_wifiConfigManager   : nullptr; }

void Application::resetAllAsync()
{
    if (m_resetInProgress)
        return;

    setResetInProgress(true);
    emit resetStarted();
    emit resetProgress("Starting factory reset...");

    // Run all operations in a background task
    QtConcurrent::run([this]() {

        // Step 1: QSettings
        emit resetProgress("Clearing device settings...");
        persistData()->resetSettings();

        // Step 2: Database
        emit resetProgress("Removing local database...");
        db()->resetDatabase();

        // Step 3: Weston config
        emit resetProgress("Resetting display configuration...");
        DisplaySettings::factoryReset();

        // Step 4: Network configs
        emit resetProgress("Resetting network configuration...");
        SystemResetManager::resetNetwork();

        // Step 5: Timezone
        emit resetProgress("Resetting system timezone...");

        QFile::remove("/etc/localtime");
        QFile::remove("/etc/timezone");

        QFile::link("/usr/share/zoneinfo/Asia/Kolkata", "/etc/localtime");

        QFile tz("/etc/timezone");
        if (tz.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            tz.write("Asia/Kolkata");
            tz.close();
        }

        emit resetProgress("Factory reset completed successfully.");
        emit resetFinished(true);

        // Step 7: Reboot
        emit resetProgress("Rebooting device...");
        emit resetRebooting();

        QProcess::startDetached("systemctl", {"reboot"});

        setResetInProgress(false);
    });
}


// ---------------------------
// 🔹 Device Config / System
// ---------------------------
DeviceInfoSettings*     Application::deviceInfoSettings()       { return s_instance ? s_instance->_deviceInfoSettings      : nullptr; }
SystemResetManager*     Application::resetManager()             { return s_instance ? s_instance->_resetManager            : nullptr; }

// ----------------------
// Application lifecycle
// ----------------------

Application::Application(QQmlApplicationEngine *engine, QObject *parent)
    : QObject(parent), m_engine(engine)
{
    qInfo() << "[App] Starting initialization";
    init();
    registerTypesAndContext();
    setupNetworkMonitors();
    qInfo() << "[App] Initialization complete";

    WifiNetworkInfo *info = new WifiNetworkInfo();
}

Application::~Application()
{
    qInfo() << "[App] Destroying singleton";

    if (m_wifiThread) { m_wifiThread->quit(); m_wifiThread->wait(); }
    if (m_ethThread) { m_ethThread->quit(); m_ethThread->wait(); }

    s_instance = nullptr;
}

// ----------------------
// Initialization helpers
// ----------------------

void Application::init()
{
    initMonitors();
    initWorkers();
    initModels();
    initControllers();
    initThreads();
}

void Application::initMonitors()
{
    m_wifiMonitor = new WifiMonitor(this);
    m_ethMonitor  = new EthernetMonitor(this);
    m_logoLoader  = new LogoLoader(this);
}

void Application::initWorkers()
{
    // background workers (moved to threads later)
    m_wifiWorker     = new WifiWorker();       // no parent until moved to thread
    m_ethWorker      = new EthernetWorker();   // no parent until moved to thread
    m_dnsNetworkInfo = new DNSNetworkInfo(this);
}

void Application::initModels()
{
    _appSettings   = new AppSettings(this);
    _deviceSettings= new DeviceSettings(this);
    _monitor       = new UdevMonitor(this);

    _tzModel       = new TimeZoneModel(this);
    _proxy         = new TimezoneFilterModel(this);
    _proxy->setSourceModel(_tzModel);

    _wifiSettings  = new WiFiSettingsManager(this);

    _persistData   = new PersistData(this);
    _wifiNetworkDetailsColl = new WifiNetworkDetailsColl(this);
    _wifiSortProxyModel = new WifiSortProxyModel(this);
    _ethernetNetworkController = new EthernetNetworkConroller(this);
    _wiFiAddNetworkManager = new WiFiAddNetworkManager(this);
    _deviceInfo    = new DeviceInfo(this);
    _serverInfoColl= new ServerInfoColl(this);
    _deviceInfoSettings = new DeviceInfoSettings(this);
    _resetManager  = new SystemResetManager(this);
    _serverModel   = new RdServerModel(this);

    // Database singleton (assumed). Do not set parent / delete.
    _database = DataBase::instance(this);
    _resolutionListModel = new ResolutionListModel(this);
    _languageModel = new LanguageModel(this);
    _sessionModel  = new SessionModel(this);
    _wifiConfigManager = new WifiConfigManager(this);
    _wifiNetworkInfo = new WifiNetworkInfo(this);
    _ethernetNetworkInfo = new EthernetNetworkInfo(this);

    _wifiSortProxyModel->setSourceModel(_wifiNetworkDetailsColl);
    _wifiSortProxyModel->sort(0);   // activate sorting
}

void Application::initControllers()
{
    // --- Database Setup ---
    if (!_database || !_database->open()) {
        qCritical() << "[DB] Error: Unable to open database";
    } else {
        _database->createTable();
        // fixed: use the correct member pointer
        _database->getServerList(_serverInfoColl);

        _resolutionListModel->init(_database->getPath());
    }

    // Device info and Wi-Fi cache
    _deviceInfo->getDeviceInfoDetails();
    _wifiNetworkDetailsColl->getWifiDetails();

    // Load Device Info Settings from JSON
    _deviceInfoSettings->loadFromFile(QStringLiteral("/usr/share/thinclient/deviceinfo.json"));
}

void Application::initThreads()
{
    m_wifiThread = new QThread(this);
    m_ethThread  = new QThread(this);
}

void Application::setupNetworkMonitors()
{
    // --- Wi-Fi worker on its own thread ---
    m_wifiWorker->moveToThread(m_wifiThread);

    connect(m_wifiThread, &QThread::started,
            m_wifiWorker, &WifiWorker::checkConnection);

    connect(m_wifiWorker, &WifiWorker::connectedChanged,
            m_wifiMonitor, &WifiMonitor::setConnected,
            Qt::QueuedConnection);

    connect(m_wifiThread, &QThread::finished,
            m_wifiWorker, &QObject::deleteLater);

    m_wifiThread->start();

    // --- Ethernet worker on its own thread ---
    m_ethWorker->moveToThread(m_ethThread);

    connect(m_ethThread, &QThread::started,
            m_ethWorker, &EthernetWorker::checkConnection);

    connect(m_ethWorker, &EthernetWorker::connectedChanged,
            m_ethMonitor, &EthernetMonitor::setIsConnected,
            Qt::QueuedConnection);

    connect(m_ethThread, &QThread::finished,
            m_ethWorker, &QObject::deleteLater);

    m_ethThread->start();
}

void Application::registerTypesAndContext()
{
    // --- Enums for QML ---
    qmlRegisterUncreatableType<WifiNetworkDetailsColl>("App.Enums", 1, 0, "WifiNetworkDetailsColl",
                                                       "Access to enums only");
    qmlRegisterUncreatableType<AudioSettingsOptions>("App.Enums", 1, 0, "Audio",
                                                     "Access to enums only");

    // --- QML Style singletons ---
    qmlRegisterSingletonType(QUrl("qrc:/styles/Colors.qml"),      "App.Styles", 1, 0, "Colors");
    qmlRegisterSingletonType(QUrl("qrc:/styles/Fonts.qml"),       "App.Styles", 1, 0, "Fonts");
    qmlRegisterSingletonType(QUrl("qrc:/styles/Theme.qml"),       "App.Styles", 1, 0, "Theme");
    qmlRegisterSingletonType(QUrl("qrc:/styles/ScreenConfig.qml"),"App.Styles", 1, 0, "ScreenConfig");

    qmlRegisterSingletonType(QUrl("qrc:/dialogs/AddNetworkEnums.qml"),"AddNetworkEnums", 1, 0, "AppEnums");

    // --- Backend types available to QML for instantiation if needed ---
    qmlRegisterType<AppUnlockManager>("AppSecurity", 1, 0, "UnlockManager");
    qmlRegisterType<ImageUpdater>("App.Backend", 1, 0, "ImageUpdater");
    qmlRegisterType<LogExporter>("G1.ThinClient", 1, 0, "LogExporter");
    qmlRegisterType<WiFiManager>("App.Backend", 1, 0, "WiFiManager");
    qmlRegisterType<WiFiAddNetworkManager>("App.Backend", 1, 0, "WiFiAddNetworkManager");
    qmlRegisterType<DisplaySettings>("App.Backend", 1, 0, "DisplaySettings");

    // --- Context singletons (instances) ---
    auto *ctx = m_engine->rootContext();

    ctx->setContextProperty("wifiMonitor", m_wifiMonitor);
    ctx->setContextProperty("ethernetMonitor", m_ethMonitor);
    ctx->setContextProperty("logoLoader", m_logoLoader);
    ctx->setContextProperty("dnsNetworkInfo", m_dnsNetworkInfo);

    ctx->setContextProperty("timezoneModel", _tzModel);
    ctx->setContextProperty("timezoneProxyModel", _proxy);

    ctx->setContextProperty("DeviceSettings", _deviceSettings);
    ctx->setContextProperty("appSettings", _appSettings);
    ctx->setContextProperty("languageModel", _languageModel);
    ctx->setContextProperty("usbMonitor", _monitor);
    ctx->setContextProperty("wifiSettings", _wifiSettings);

    ctx->setContextProperty("serverModel", _serverModel);
    ctx->setContextProperty("wifiNetworkDetails", _wifiNetworkDetailsColl);
    ctx->setContextProperty("sortedWifiModel", _wifiSortProxyModel);
    ctx->setContextProperty("ethernetNetworkController", _ethernetNetworkController);
    ctx->setContextProperty("deviceInfo", _deviceInfo);
    ctx->setContextProperty("serverInfo", _serverInfoColl);
    ctx->setContextProperty("dataBase", _database);
    ctx->setContextProperty("persistData", _persistData);
    ctx->setContextProperty("deviceInfoSettings", _deviceInfoSettings);
    ctx->setContextProperty("resetManager", _resetManager);
    ctx->setContextProperty("resolutionListModel", _resolutionListModel);

    ctx->setContextProperty("wifiConfigManager", _wifiConfigManager);
    ctx->setContextProperty("wifiNetworkInfo", _wifiNetworkInfo);
    ctx->setContextProperty("ethernetNetworkInfo", _ethernetNetworkInfo);

    qmlRegisterUncreatableType<ConnectionInfo>("App.Backend", 1, 0, "ConnectionInfo",
                                               "ConnectionInfo cannot be created in QML");

    ctx->setContextProperty("sessionModel", _sessionModel);


    qmlRegisterSingletonType<BootHelper>("App.Backend", 1, 0, "BootHelper",
                                         [](QQmlEngine*, QJSEngine*) -> QObject* {
                                             return new BootHelper();
                                         }
                                         );
    qmlRegisterType<NotificationItem>("App.Backend", 1, 0, "Type");
}

bool Application::resetInProgress() const
{
    return m_resetInProgress;
}

void Application::setResetInProgress(bool v)
{
    if (m_resetInProgress == v)
        return;
    m_resetInProgress = v;
    emit resetInProgressChanged();
}
