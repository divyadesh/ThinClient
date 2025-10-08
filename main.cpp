#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QStyleHints>
#include <QTimer>

#include "Database.h"
#include "DeviceSettings.h"
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
#include "qmlregistrar.h"
#include "ethernetNetworkConroller.h"

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
    TimeZoneModel timeZoneModel;

    engine.rootContext()->setContextProperty("appSettings", &appSettings);
    engine.rootContext()->setContextProperty("languageModel", &languageModel);
    engine.rootContext()->setContextProperty("timeZoneModel", &timeZoneModel);

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
    qmlRegisterUncreatableType<DeviceSettings>(
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

    // --- Load Persistent and Network Data ---
    serverInfoColl.setAutoConnect(
        persistData.getData("AutoConnectConnectionName"),
        persistData.getData("AutoConnectIpAddress")
        );

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

    // --- Auto-Connect Check ---
    QTimer::singleShot(5000, [&serverInfoColl] {
        const auto result = serverInfoColl.checkAutoConnect();
        if (!result.first.isEmpty()) {
            qInfo() << "Auto-connect triggered for:" << result.first << "IP:" << result.second;
        }
    });

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
