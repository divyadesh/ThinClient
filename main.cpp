#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QStyleHints>
#include "DeviceInfo.h"
#include "ServerInfoColl.h"
#include "WifiNetworkDetailsColl.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);
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

    WifiNetworkDetailsColl wifiNetworkDetailsColl;
    DeviceInfo deviceInfo;
    ServerInfoColl serverInfoColl;
    serverInfoColl.setServerInfo("Server1", "192.168.1.1");
    serverInfoColl.setServerInfo("Server2", "192.168.1.2");
    serverInfoColl.setServerInfo("Server3", "192.168.1.1");
    serverInfoColl.setServerInfo("Server4", "192.168.1.2");
    serverInfoColl.setServerInfo("Server5", "192.168.1.1");
    serverInfoColl.setServerInfo("Server6", "192.168.1.2");
    serverInfoColl.setServerInfo("Server7", "192.168.1.1");
    serverInfoColl.setServerInfo("Server8", "192.168.1.2");
    deviceInfo.getDeviceInfoDetails();
    wifiNetworkDetailsColl.getWifiDetails();
    engine.rootContext()->setContextProperty("wifiNetworkDetails", &wifiNetworkDetailsColl);
    engine.rootContext()->setContextProperty("deviceInfo", &deviceInfo);
    engine.rootContext()->setContextProperty("serverInfo", &serverInfoColl);

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
