#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QStyleHints>

#include "DeviceSettings.h"
#include "DeviceInfo.h"
#include "ServerInfoColl.h"
#include "WifiNetworkDetailsColl.h"
#include "Database.h"
#include "PersistData.h"

#include "DeviceInfoSettings.h"
#include "ImageUpdater.h"
#include "SystemResetManager.h"
#include "appsettings.h"
#include "appunlockmanager.h"
#include "language_model.h"
#include "timezone_model.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    QGuiApplication app(argc, argv);
    QGuiApplication::setOrganizationDomain(QStringLiteral("https://g1thinclientpc.com/"));
    QGuiApplication::setOrganizationName(QStringLiteral("G1 Thin Client pc"));
    QGuiApplication::setApplicationName(QStringLiteral("G1ThinClientPC"));
    QGuiApplication::setApplicationDisplayName(QStringLiteral("G1 Thin Client pc"));
    QGuiApplication::setApplicationVersion(QStringLiteral("1.0.0"));

    QQmlApplicationEngine engine;

    // Register QML singleton types
    qmlRegisterSingletonType(QUrl("qrc:/styles/Colors.qml"), "App.Styles", 1, 0, "Colors");
    qmlRegisterSingletonType(QUrl("qrc:/styles/Fonts.qml"), "App.Styles", 1, 0, "Fonts");
    qmlRegisterSingletonType(QUrl("qrc:/styles/Theme.qml"), "App.Styles", 1, 0, "Theme");
    qmlRegisterSingletonType(QUrl("qrc:/styles/ScreenConfig.qml"),
                             "App.Styles",
                             1,
                             0,
                             "ScreenConfig");

    AppSettings settings;
    LanguageModel languageModel(&settings);
    TimeZoneModel timeZoneModel;

    engine.rootContext()->setContextProperty("appSettings", &settings);
    engine.rootContext()->setContextProperty("languageModel", &languageModel);
    engine.rootContext()->setContextProperty("timeZoneModel", &timeZoneModel);

    qmlRegisterType<AppUnlockManager>("AppSecurity", 1, 0, "UnlockManager");
    qmlRegisterType<ImageUpdater>("App.Backend", 1, 0, "ImageUpdater");

    PersistData persistData;
    WifiNetworkDetailsColl wifiNetworkDetailsColl;
    DeviceInfo deviceInfo;
    ServerInfoColl serverInfoColl;
    auto & dbInstance  = DataBase::getInstance(nullptr);
    if(!dbInstance.open()) {
        qDebug()<<"Error: Unable to open database" ;
    }
    dbInstance.createTable();
    dbInstance.getServerList(serverInfoColl);

    deviceInfo.getDeviceInfoDetails();
    wifiNetworkDetailsColl.getWifiDetails();
    qmlRegisterUncreatableType<WifiNetworkDetailsColl>("App.Enums", 1, 0, "WifiNetworkDetailsColl", "Access to enums only");
    qmlRegisterUncreatableType<DeviceSettings>("App.Enums", 1, 0, "Audio", "Access to enums only");
    engine.rootContext()->setContextProperty("wifiNetworkDetails", &wifiNetworkDetailsColl);
    engine.rootContext()->setContextProperty("deviceInfo", &deviceInfo);
    engine.rootContext()->setContextProperty("serverInfo", &serverInfoColl);
    engine.rootContext()->setContextProperty("dataBase", &dbInstance);
    engine.rootContext()->setContextProperty("persistData", &persistData);

    DeviceInfoSettings deviceInfoSettings;
    deviceInfoSettings.loadFromFile(
        "/Users/adesh/ThinClient/deviceinfo.json"); // or "deviceinfo.json"

    engine.rootContext()->setContextProperty("deviceInfoSettings", &deviceInfoSettings);

    SystemResetManager resetManager;
    engine.rootContext()->setContextProperty("resetManager", &resetManager);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
