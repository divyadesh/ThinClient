QT += quick quickcontrols2 core qml sql virtualkeyboard dbus network

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        Application.cpp \
        Database.cpp \
        DeviceInfo.cpp \
        DisplaySettings.cpp \
        PersistData.cpp \
        ServerInfo.cpp \
        ServerInfoColl.cpp \
        UdevMonitor.cpp \
        WifiNetworkDetails.cpp \
        WifiNetworkDetailsColl.cpp \
        appsettings.cpp \
        appunlockmanager.cpp \
        boothelper.cpp \
        connectioninfo.cpp \
        deviceinfosettings.cpp \
        devicesettings.cpp \
        dnsnetworkinfo.cpp \
        ethernetmonitor.cpp \
        ethernetworker.cpp \
        ethernetNetworkConroller.cpp \
        imageupdater.cpp \
        inputactivityfilter.cpp \
        language_model.cpp \
        logexporter.cpp \
        logger.cpp \
        logoloader.cpp \
        main.cpp \
        rdservermodel.cpp \
        resolutionlistmodel.cpp \
        sessionmodel.cpp \
        systemresetmanager.cpp \
        timezone_filter_model.cpp \
        timezone_model.cpp \
        timezonemodel.cpp \
        wifiaddnetworkmanager.cpp \
        wifimanager.cpp \
        wifimonitor.cpp \
        wifisettingsmanager.cpp \
        wifiworker.cpp

RESOURCES += qml.qrc \
    translation.qrc

# /home/rajni/Qt/5.15.2/gcc_64/bin/lupdate ThinClient.pro
# /home/rajni/Qt/5.15.2/gcc_64/bin/lrelease ThinClient.pro


TRANSLATIONS += \
    translations/en_US.ts \
    translations/fr_FR.ts \
    translations/es_ES.ts \
    translations/de_DE.ts \
    translations/ru_RU.ts \
    translations/ar_SA.ts \
    translations/zh_CN.ts

HEADERS += \
    Application.h \
    AudioSettingsOptions.h \
    Database.h \
    DeviceInfo.h \
    DisplaySettings.h \
    NotificationItem.h \
    PersistData.h \
    RdpMonitorThread.h \
    ServerInfo.h \
    ServerInfoColl.h \
    SystemSettings.h \
    UdevMonitor.h \
    WifiNetworkDetails.h \
    WifiNetworkDetailsColl.h \
    appsettings.h \
    appunlockmanager.h \
    boothelper.h \
    connectioninfo.h \
    deviceinfosettings.h \
    devicesettings.h \
    dnsnetworkinfo.h \
    ethernetmonitor.h \
    ethernetworker.h \
    ethernetNetworkConroller.h \
    imageupdater.h \
    inputactivityfilter.h \
    language_model.h \
    logexporter.h \
    logger.h \
    logoloader.h \
    rdservermodel.h \
    resolutionlistmodel.h \
    sessionmodel.h \
    systemresetmanager.h \
    timezone_filter_model.h \
    timezone_model.h \
    timezonemodel.h \
    wifiaddnetworkmanager.h \
    wifimanager.h \
    wifimonitor.h \
    wifisettingsmanager.h \
    wifiworker.h

# --- Deployment section ---
# TARGET = ThinClient
# target.path = /root
# INSTALLS += target

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

LIBS += -ludev
