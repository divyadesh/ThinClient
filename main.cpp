#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QStyleHints>
#include <QTimer>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>
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
#include "qmlregistrar.h"
#include "ethernetNetworkConroller.h"
#include "devicesettings.h"
#include "rdservermodel.h"
#include "UdevMonitor.h"

bool updateWestonConfig()
{
    const QString targetPath = "/etc/xdg/weston/weston.ini";
    const QString sourcePath = "/etc/xdg/weston/thinclient/weston.ini";

    QFile sourceFile(sourcePath);
    QFile targetFile(targetPath);

    // Check if source exists
    if (!sourceFile.exists()) {
        qWarning() << "Source Weston config not found:" << sourcePath;
        return false;
    }

    // Remove old Weston config if present
    if (targetFile.exists()) {
        if (!targetFile.remove()) {
            qWarning() << "Failed to remove old Weston config:" << targetPath;
            return false;
        } else {
            qInfo() << "Removed old Weston config:" << targetPath;
        }
    }

    // Ensure target directory exists
    QDir dir(QFileInfo(targetPath).absolutePath());
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qWarning() << "Failed to create Weston config directory:" << dir.absolutePath();
            return false;
        }
    }

    // Copy new config
    if (!QFile::copy(sourcePath, targetPath)) {
        qWarning() << "Failed to copy new Weston config from" << sourcePath << "to" << targetPath;
        return false;
    }

    // Set proper permissions (optional but good practice)
    QFile::setPermissions(targetPath, QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ReadGroup | QFileDevice::ReadOther);

    qInfo() << "Weston config successfully updated from" << sourcePath << "to" << targetPath;
    return true;
}

// called at application startup
void ensureWestonConfig()
{
    QSettings settings("/var/lib/thinclient/settings.ini", QSettings::IniFormat);
    bool alreadyUpdated = settings.value("WestonConfigUpdated", false).toBool();

    if (!alreadyUpdated) {
        qInfo() << "Updating Weston config for the first time...";
        (void)QtConcurrent::run([=]() {
            if (updateWestonConfig()) {
                QSettings s("/var/lib/thinclient/settings.ini", QSettings::IniFormat);
                s.setValue("WestonConfigUpdated", true);
                s.sync();
                qInfo() << "Weston config updated and marked as done.";
            } else {
                qWarning() << "Weston config update failed!";
            }
        });
    } else {
        qInfo() << "Weston config already updated previously — skipping.";
    }
}

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    QGuiApplication app(argc, argv);

    // --- Application Metadata ---
    QGuiApplication::setOrganizationDomain(QStringLiteral("https://g1thinclientpc.com/"));
    QGuiApplication::setOrganizationName(QStringLiteral("G1 Thin Client PC"));
    QGuiApplication::setApplicationName(QStringLiteral("G1ThinClientPC"));
    QGuiApplication::setApplicationDisplayName(QStringLiteral("G1 Thin Client PC"));
    QGuiApplication::setApplicationVersion(QStringLiteral("1.0.0"));

    // --- QML Engine Setup ---
    QQmlApplicationEngine engine;

    // --- Global QML Style Singletons ---
    qmlRegisterSingletonType(QUrl("qrc:/styles/Colors.qml"), "App.Styles", 1, 0, "Colors");
    qmlRegisterSingletonType(QUrl("qrc:/styles/Fonts.qml"), "App.Styles", 1, 0, "Fonts");
    qmlRegisterSingletonType(QUrl("qrc:/styles/Theme.qml"), "App.Styles", 1, 0, "Theme");
    qmlRegisterSingletonType(QUrl("qrc:/styles/ScreenConfig.qml"), "App.Styles", 1, 0, "ScreenConfig");

    // --- Application Settings and Models ---
    AppSettings appSettings;
    LanguageModel languageModel(&appSettings);
    DeviceSettings deviceSettings;
    UdevMonitor monitor;

    TimezoneModel tzModel;
    TimezoneFilterModel proxy;
    proxy.setSourceModel(&tzModel);

    engine.rootContext()->setContextProperty("timezoneModel", &tzModel);
    engine.rootContext()->setContextProperty("timezoneProxyModel", &proxy);
    // Create a single instance of your model
    engine.rootContext()->setContextProperty("DeviceSettings", &deviceSettings);
    engine.rootContext()->setContextProperty("appSettings", &appSettings);
    engine.rootContext()->setContextProperty("languageModel", &languageModel);
    engine.rootContext()->setContextProperty("usbMonitor", &monitor);

    // --- Register QML Backend Types ---
    qmlRegisterType<AppUnlockManager>("AppSecurity", 1, 0, "UnlockManager");
    qmlRegisterType<ImageUpdater>("App.Backend", 1, 0, "ImageUpdater");

    // --- QML Registrar for central registration ---
    // This handles additional backend types and context properties
    QmlRegistrar registrar(&engine);
    registrar.registerTypesAndContext();

    // --- Register enums-only types for QML (App.Enums) ---
    qmlRegisterUncreatableType<WifiNetworkDetailsColl>(
        "App.Enums", 1, 0, "WifiNetworkDetailsColl", "Access to enums only");
    qmlRegisterUncreatableType<AudioSettingsOptions>(
        "App.Enums", 1, 0, "Audio", "Access to enums only");


    // --- Initialize Backend Components ---
    PersistData persistData;
    WifiNetworkDetailsColl wifiNetworkDetailsColl;
    EthernetNetworkConroller ethernetNetworkController;
    DeviceInfo deviceInfo;
    ServerInfoColl serverInfoColl;
    DeviceInfoSettings deviceInfoSettings;
    SystemResetManager resetManager;

    // --- Database Setup ---
    auto &dbInstance = DataBase::getInstance(nullptr);
    if (!dbInstance.open()) {
        qCritical() << "Error: Unable to open database";
    } else {
        dbInstance.createTable();
        dbInstance.getServerList(serverInfoColl);
    }

    RdServerModel serverModel;

    // Expose it to QML as a context property
    engine.rootContext()->setContextProperty("serverModel", &serverModel);

    deviceInfo.getDeviceInfoDetails();
    wifiNetworkDetailsColl.getWifiDetails();

    // --- Load Device Info Settings from JSON ---
    deviceInfoSettings.loadFromFile("/usr/share/thinclient/deviceinfo.json");

    // --- Register Backend Objects to QML Context ---
    engine.rootContext()->setContextProperty("wifiNetworkDetails", &wifiNetworkDetailsColl);
    engine.rootContext()->setContextProperty("ethernetNetworkController", &ethernetNetworkController);
    engine.rootContext()->setContextProperty("deviceInfo", &deviceInfo);
    engine.rootContext()->setContextProperty("serverInfo", &serverInfoColl);
    engine.rootContext()->setContextProperty("dataBase", &dbInstance);
    engine.rootContext()->setContextProperty("persistData", &persistData);
    engine.rootContext()->setContextProperty("deviceInfoSettings", &deviceInfoSettings);
    engine.rootContext()->setContextProperty("resetManager", &resetManager);

    ensureWestonConfig();

    // --- Load Main QML ---
    const QUrl mainQmlUrl(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [mainQmlUrl](QObject *obj, const QUrl &objUrl) {
            if (!obj && mainQmlUrl == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.load(mainQmlUrl);

    // --- Run Application ---
    return app.exec();
}
