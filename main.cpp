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
#include <QFile>
#include <QTextStream>
#include <QFont>
#include <QFontDatabase>
#include<QQuickWindow>
#include <QSGRendererInterface>

#include "Application.h"
#include "logger.h"
#include "inputactivityfilter.h"
#include "qpaplatform.h"


int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    /* -------------------------------------------------------
     * Environment (Wayland + Qt)
     * ------------------------------------------------------- */
    qputenv("QT_IM_MODULE", "qtvirtualkeyboard");
    qputenv("QT_QPA_PLATFORM", QpaPlatform::toEnvValue(QpaPlatform::Wayland));

    /* Use software rendering ONLY if GPU/EGL is unstable */
    QQuickWindow::setSceneGraphBackend(QSGRendererInterface::Software);
    qputenv("QT_QUICK_BACKEND", "software");

    /* -------------------------------------------------------
     * Qt Application
     * ------------------------------------------------------- */


    QGuiApplication app(argc, argv);
    Logger::init("/var/log/thinclient.log");

    // Set global application font
    QFont rubik("Arial", 12);
    QGuiApplication::setFont(rubik);


    // --- Metadata ---
    const QString appName = "G1ThinClientPC";
    const QString appDisplayName = "G1 Thin Client PC";
    const QString appVersion = "1.0.1";
    const QString appDescription = "Thin client desktop for embedded systems (Qt/QML + C++).";

    QGuiApplication::setOrganizationDomain("https://g1thinclientpc.com/");
    QGuiApplication::setOrganizationName("G1 Thin Client PC");
    QGuiApplication::setApplicationName(appName);
    QGuiApplication::setApplicationDisplayName(appDisplayName);
    QGuiApplication::setApplicationVersion(appVersion);

    // --- Command line parser ---
    QCommandLineParser parser;
    parser.setApplicationDescription(appDescription);
    parser.addHelpOption();
    parser.addVersionOption(); // enables --version and -v

    parser.process(app);

    // If --version passed, Qt automatically prints "<name> <version>" and exits
    if (parser.isSet("version")) {
        return 0;
    }

    // --- Print startup info ---
    qInfo().noquote() << QString("==== %1 v%2 ====").arg(appDisplayName, appVersion);
    qInfo().noquote() << QString("Description: %1").arg(appDescription);
    qInfo().noquote() << QString("Startup Time: %1").arg(QDateTime::currentDateTime().toString(Qt::ISODate));
    qInfo() << "Organization:" << QGuiApplication::organizationName();
    qInfo() << "Domain:" << QGuiApplication::organizationDomain();
    qInfo() << "Application path:" << QCoreApplication::applicationFilePath();

    // --- QML Engine Setup ---
    QQmlApplicationEngine engine;

    // --- Initialize our global Application Singleton ---
    Application::initialize(&engine);

    InputActivityFilter *activity = new InputActivityFilter(&app);
    engine.rootContext()->setContextProperty("ActivityMonitor", activity);
    engine.rootContext()->setContextProperty("cApplication", Application::instance());
    app.installEventFilter(activity);

    // --- Load Main UI ---
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

    // --- Execute ---
    int code = app.exec();

    // --- Clean up singleton ---
    Application::destroy();

    qInfo() << "Application exited with code:" << code;
    Logger::shutdown();

    return code;
}
