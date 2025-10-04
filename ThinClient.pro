QT += quick quickcontrols2 core qml sql

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        Database.cpp \
        DeviceInfo.cpp \
        PersistData.cpp \
        ServerInfo.cpp \
        ServerInfoColl.cpp \
        WifiNetworkDetails.cpp \
        WifiNetworkDetailsColl.cpp \
        appsettings.cpp \
        appunlockmanager.cpp \
        deviceinfosettings.cpp \
        dnsnetworkinfo.cpp \
        ethernetmonitor.cpp \
        ethernetworker.cpp \
        ethernetNetworkConroller.cpp \
        imageupdater.cpp \
        language_model.cpp \
        logoloader.cpp \
        main.cpp \
        qmlregistrar.cpp \
        systemresetmanager.cpp \
        timezone_model.cpp \
        wifimonitor.cpp \
        wifiworker.cpp

RESOURCES += qml.qrc
# qml_files.files = $$files($$PWD/*.qml, true)
# qml_files.prefix = "/"
# RESOURCES += qml_files

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    Database.h \
    DeviceInfo.h \
    DeviceSettings.h \
    PersistData.h \
    ServerInfo.h \
    ServerInfoColl.h \
    WifiNetworkDetails.h \
    WifiNetworkDetailsColl.h \
    appsettings.h \
    appunlockmanager.h \
    deviceinfosettings.h \
    dnsnetworkinfo.h \
    ethernetmonitor.h \
    ethernetworker.h \
    ethernetNetworkConroller.h \
    imageupdater.h \
    language_model.h \
    logoloader.h \
    qmlregistrar.h \
    systemresetmanager.h \
    timezone_model.h \
    wifimonitor.h \
    wifiworker.h
