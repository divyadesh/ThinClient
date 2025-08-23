#include <QGuiApplication>
#include <QQmlApplicationEngine>

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
