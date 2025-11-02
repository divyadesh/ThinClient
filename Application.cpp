#include "Application.h"
#include <QQmlContext>
#include <QTimer>

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
TimezoneModel*          Application::timezoneModel()            { return s_instance ? s_instance->_tzModel                 : nullptr; }
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
    _languageModel = new LanguageModel(this);
    _deviceSettings= new DeviceSettings(this);
    _monitor       = new UdevMonitor(this);

    _tzModel       = new TimezoneModel(this);
    _proxy         = new TimezoneFilterModel(this);
    _proxy->setSourceModel(_tzModel);

    _wifiSettings  = new WiFiSettingsManager(this);

    _persistData   = new PersistData(this);
    _wifiNetworkDetailsColl = new WifiNetworkDetailsColl(this);
    _ethernetNetworkController = new EthernetNetworkConroller(this);
    _deviceInfo    = new DeviceInfo(this);
    _serverInfoColl= new ServerInfoColl(this);
    _deviceInfoSettings = new DeviceInfoSettings(this);
    _resetManager  = new SystemResetManager(this);
    _serverModel   = new RdServerModel(this);

    // Database singleton (assumed). Do not set parent / delete.
    _database = DataBase::instance(this);
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
            m_ethMonitor, &EthernetMonitor::setConnected,
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

    // --- Backend types available to QML for instantiation if needed ---
    qmlRegisterType<AppUnlockManager>("AppSecurity", 1, 0, "UnlockManager");
    qmlRegisterType<ImageUpdater>("App.Backend", 1, 0, "ImageUpdater");

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
    ctx->setContextProperty("ethernetNetworkController", _ethernetNetworkController);
    ctx->setContextProperty("deviceInfo", _deviceInfo);
    ctx->setContextProperty("serverInfo", _serverInfoColl);
    ctx->setContextProperty("dataBase", _database);
    ctx->setContextProperty("persistData", _persistData);
    ctx->setContextProperty("deviceInfoSettings", _deviceInfoSettings);
    ctx->setContextProperty("resetManager", _resetManager);
}
