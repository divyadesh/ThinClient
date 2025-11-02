QT += quick quickcontrols2 core qml sql

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        Application.cpp \
        Database.cpp \
        DeviceInfo.cpp \
        PersistData.cpp \
        ServerInfo.cpp \
        ServerInfoColl.cpp \
        UdevMonitor.cpp \
        WifiNetworkDetails.cpp \
        WifiNetworkDetailsColl.cpp \
        appsettings.cpp \
        appunlockmanager.cpp \
        deviceinfosettings.cpp \
        devicesettings.cpp \
        dnsnetworkinfo.cpp \
        ethernetmonitor.cpp \
        ethernetworker.cpp \
        ethernetNetworkConroller.cpp \
        imageupdater.cpp \
        language_model.cpp \
        logoloader.cpp \
        main.cpp \
        rdservermodel.cpp \
        systemresetmanager.cpp \
        timezone_filter_model.cpp \
        timezone_model.cpp \
        wifimonitor.cpp \
        wifisettingsmanager.cpp \
        wifiworker.cpp

RESOURCES += qml.qrc

HEADERS += \
    Application.h \
    AudioSettingsOptions.h \
    Database.h \
    DeviceInfo.h \
    PersistData.h \
    RdpMonitorThread.h \
    ServerInfo.h \
    ServerInfoColl.h \
    UdevMonitor.h \
    WifiNetworkDetails.h \
    WifiNetworkDetailsColl.h \
    appsettings.h \
    appunlockmanager.h \
    deviceinfosettings.h \
    devicesettings.h \
    dnsnetworkinfo.h \
    ethernetmonitor.h \
    ethernetworker.h \
    ethernetNetworkConroller.h \
    imageupdater.h \
    language_model.h \
    logoloader.h \
    rdservermodel.h \
    systemresetmanager.h \
    timezone_filter_model.h \
    timezone_model.h \
    wifimonitor.h \
    wifisettingsmanager.h \
    wifiworker.h

# --- Deployment section ---
TARGET = ThinClient
target.path = /root
INSTALLS += target

LIBS += -ludev
