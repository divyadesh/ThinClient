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
#include "Application.h"
#include "ethernetNetworkConroller.h"
#include "devicesettings.h"
#include "rdservermodel.h"
#include "UdevMonitor.h"
#include "wifisettingsmanager.h"

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

    QGuiApplication app(argc, argv);

    // --- Metadata ---
    QGuiApplication::setOrganizationDomain("https://g1thinclientpc.com/");
    QGuiApplication::setOrganizationName("G1 Thin Client PC");
    QGuiApplication::setApplicationName("G1ThinClientPC");
    QGuiApplication::setApplicationDisplayName("G1 Thin Client PC");
    QGuiApplication::setApplicationVersion("1.0.0");

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

    return code;
}
