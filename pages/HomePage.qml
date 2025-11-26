import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0
import AppSecurity 1.0

import "../controls"
import "../components"
import "../dialogs"

BasicPage {
    id: page
    objectName: "HomePage"
    StackView.visible: true
    background: Image {
        //width : height = 3 : 2 Aspect ratio
        source: Qt.resolvedUrl("qrc:/assets/icons/background.jpg")
    }

    UnlockManager {
        id: unlockManager
    }

    Connections {
        target: wifiMonitor

        // Fired when WiFi connects
        function onWifiConnected(ssid) {
            console.log("📶 WiFi Connected:", ssid)
        }

        // Fired when WiFi disconnects
        function onWifiDisconnected() {
            console.log("❌ WiFi Disconnected")
        }

        // Fired on ANY state change
        function onWifiStateChanged(state) {
            console.log("⚙ WiFi State Changed:", state)
        }
    }

    Connections {
        target: ethernetMonitor

        function onConnectedChanged(connected) {
            console.log("[Ethernet] Connected:", connected)
        }

        function onCablePlugged() {
            console.log("[Ethernet] Cable inserted!")
        }

        function onCableUnplugged() {
            console.log("[Ethernet] Cable removed!")
        }
    }


    ButtonGroup { id: tabGroup }

    Timer {
        id: delayedConnectTimer
        interval: 5000    // 5 seconds delay
        repeat: false
        running: false
        property string connectionId: ""

        onTriggered: {
            if(pageStack.currentItem.objectName === "HomePage") {
                pageStack.push(autoConnectServer, {"connectionId": connectionId})
            }
        }
    }

    function connectRDServer(connectionId, delayMs) {
        delayedConnectTimer.interval = delayMs
        delayedConnectTimer.connectionId = connectionId
        delayedConnectTimer.start()
    }

    Image {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: -40
        anchors.topMargin: -40
        sourceSize: Qt.size(600, 210)
        source: (logoLoader && logoLoader.logo) ? "file://" + logoLoader.logo  : Qt.resolvedUrl("qrc:/assets/icons/logos/logo.png")
    }

    Control {
        width: parent.width * 0.8
        height: parent.height * 0.6
        anchors.centerIn: parent
        padding: 120

        contentItem: GridView {
            id: grid
            anchors.fill: parent
            cellWidth: 220; cellHeight: 220
            clip: true

            model: serverModel

            highlight: null
            focus: true

            delegate: Control {
                padding: 20
                implicitWidth: grid.cellWidth
                implicitHeight: grid.cellHeight

                contentItem: HomeTabButton {
                    id: tabButton
                    ButtonGroup.group: tabGroup
                    text: connectionName
                    icon.source: Qt.resolvedUrl("qrc:/assets/icons/rd-client.png")

                    onClicked: {
                        serverInfo.connectRdServer(connectionId)
                    }

                    background: Rectangle {
                        anchors.fill: parent
                        color: tabButton.checked ? Colors.accentHover : "#2A2A2A"
                        radius: 8
                    }
                }

                Component.onCompleted: {
                    if(autoConnect) {
                        connectRDServer(connectionId, 5000) // will run after 5 seconds
                    }
                }
            }
        }
    }

    Control {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        padding: 20

        contentItem: ColumnLayout {
            spacing: 20

            Icon {
                id: wifi
                visible: wifiMonitor.connected
                iconWidth: Theme.iconLarge
                iconHeight: Theme.iconLarge
                icon: Qt.resolvedUrl("qrc:/assets/icons/wifi.png")
                scale: 1.0

                onClicked: {}
            }

            Icon {
                id:ethernet
                visible: ethernetMonitor.connected
                iconWidth: Theme.iconLarge
                iconHeight: Theme.iconLarge
                icon: Qt.resolvedUrl("qrc:/assets/icons/ethernet.png")
                scale: 1.0

                onClicked: {}
            }

            Icon {
                id: usb
                visible: usbMonitor.usbConnected
                iconWidth: Theme.iconLarge
                iconHeight: Theme.iconLarge
                icon: Qt.resolvedUrl("qrc:/assets/icons/usb.svg")
                scale: 1.0

                onClicked: {}
            }

            Item {
                Layout.fillHeight: true
            }

            Icon {
                iconWidth: Theme.iconLarge
                iconHeight: Theme.iconLarge
                icon: Qt.resolvedUrl("qrc:/assets/icons/settings.png")

                onClicked: {
                    if(unlockManager.hasPassword) {
                        pageStack.push(loginPage)
                    }else {
                        pageStack.push(dashboardPage)
                    }
                }
            }
        }
    }

    Component {
        id: dashboardPage
        DashboardPage {}
    }

    Component {
        id: loginPage
        LoginPage {
            onLoginSuccess: {
                pageStack.replace(dashboardPage)
            }
        }
    }

    Component {
        id: autoConnectServer
        AutoConnect {
            onCancelled: {}
        }
    }

    Connections {
        target: dataBase
        function onRefreshTable() {
            serverModel.refresh()
        }
    }
}
