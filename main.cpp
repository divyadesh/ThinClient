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

// -------------------------------------------------------------------
// 🔹 Application Entry Point
// -------------------------------------------------------------------

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    qputenv("QT_QPA_PLATFORM", QByteArray("wayland-egl"));
    // qputenv("QT_QPA_PLATFORM", QByteArray("vnc"));
    // qputenv("QT_VNC_SIZE", "1920x1080");

    QQuickWindow::setSceneGraphBackend(QSGRendererInterface::Software);
    qputenv("QT_QUICK_BACKEND", "software");  // double-ensure

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

    // --- Update Weston Config in background (only once) ---
    ensureWestonConfig();

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
