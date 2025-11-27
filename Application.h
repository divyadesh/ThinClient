#pragma once

#include <QObject>
#include <QQmlApplicationEngine>
#include <QThread>
#include <QTimer>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>

#include "wifimonitor.h"
#include "ethernetmonitor.h"
#include "logoloader.h"
#include "wifiworker.h"
#include "ethernetworker.h"
#include "dnsnetworkinfo.h"
#include "Database.h"
#include "AudioSettingsOptions.h"
#include "DeviceInfo.h"
#include "ServerInfoColl.h"
#include "WifiNetworkDetailsColl.h"
#include "PersistData.h"
#include "deviceinfosettings.h"
#include "imageupdater.h"
#include "systemresetmanager.h"
#include "appsettings.h"
#include "appunlockmanager.h"
#include "language_model.h"
#include "timezone_model.h"
#include "timezone_filter_model.h"
#include "ethernetNetworkConroller.h"
#include "devicesettings.h"
#include "rdservermodel.h"
#include "UdevMonitor.h"
#include "wifisettingsmanager.h"
#include "logexporter.h"
#include "wifiaddnetworkmanager.h"
#include "DisplaySettings.h"
#include "timezonemodel.h"
#include "resolutionlistmodel.h"

class Application : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool resetInProgress READ resetInProgress NOTIFY resetInProgressChanged)
public:
    // --- Singleton lifecycle ---
    static void initialize(QQmlApplicationEngine *engine);
    static Application *instance();
    static void destroy();

    ~Application() override;

    // --- Static getters for all core components ---
    static PersistData *persistData();
    static WiFiSettingsManager *wifiSettings();
    static DeviceSettings *deviceSettings();
    static AppSettings *appSettings();
    static DataBase *database();
    static WifiMonitor *wifiMonitor();
    static EthernetMonitor *ethernetMonitor();
    static LogoLoader *logoLoader();
    static DNSNetworkInfo *dnsNetworkInfo();
    static LanguageModel *languageModel();
    static TimeZoneModel *timezoneModel();
    static TimezoneFilterModel *timezoneProxy();
    static UdevMonitor *usbMonitor();
    static WifiNetworkDetailsColl *wifiNetworkDetails();
    static EthernetNetworkConroller *ethernetNetworkController();
    static DeviceInfo *deviceInfo();
    static ServerInfoColl *serverInfo();
    static DeviceInfoSettings *deviceInfoSettings();
    static SystemResetManager *resetManager();
    static RdServerModel *serverModel();
    static DataBase *db();
    static WiFiAddNetworkManager* wiFiAddNetworkManager();
    static ResolutionListModel* resolutionListModel();

    Q_INVOKABLE void resetAllAsync();

    bool resetInProgress() const;

signals:
    void resetInProgressChanged();
    void resetStarted();
    void resetProgress(const QString &message);
    void resetFinished(bool success);
    void resetRebooting();

private:
    explicit Application(QQmlApplicationEngine *engine, QObject *parent = nullptr);
    void init();
    void initMonitors();
    void initWorkers();
    void initModels();
    void initControllers();
    void initThreads();
    void registerTypesAndContext();
    void setupNetworkMonitors();

private:

    void setResetInProgress(bool v);

    static Application *s_instance;
    QQmlApplicationEngine *m_engine = nullptr;

    // Frontend-facing monitors / helpers
    WifiMonitor *m_wifiMonitor = nullptr;
    EthernetMonitor *m_ethMonitor = nullptr;
    LogoLoader *m_logoLoader = nullptr;

    // Workers (background threads)
    WifiWorker *m_wifiWorker = nullptr;
    EthernetWorker *m_ethWorker = nullptr;
    DNSNetworkInfo *m_dnsNetworkInfo = nullptr;

    // App / Settings / Models exposed to QML
    AppSettings *_appSettings = nullptr;
    LanguageModel *_languageModel = nullptr;
    DeviceSettings *_deviceSettings = nullptr;
    UdevMonitor *_monitor = nullptr;

    TimeZoneModel *_tzModel = nullptr;
    TimezoneFilterModel *_proxy = nullptr;
    WiFiSettingsManager *_wifiSettings = nullptr;

    PersistData *_persistData = nullptr;
    WifiNetworkDetailsColl *_wifiNetworkDetailsColl = nullptr;
    EthernetNetworkConroller *_ethernetNetworkController = nullptr;
    DeviceInfo *_deviceInfo = nullptr;
    ServerInfoColl *_serverInfoColl = nullptr;
    DeviceInfoSettings *_deviceInfoSettings = nullptr;
    SystemResetManager *_resetManager = nullptr;
    RdServerModel *_serverModel = nullptr;
    DataBase *_database = nullptr; // assumed singleton; do not delete
    WiFiAddNetworkManager *_wiFiAddNetworkManager;
    ResolutionListModel *_resolutionListModel;

    // Threads
    QThread *m_wifiThread = nullptr;
    QThread *m_ethThread = nullptr;
    bool m_resetInProgress;
};
