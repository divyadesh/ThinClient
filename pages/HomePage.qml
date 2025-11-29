import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0
import AppSecurity 1.0
import App.Backend 1.0

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
            console.log("WiFi Connected:", ssid)
        }

        // Fired when WiFi disconnects
        function onWifiDisconnected() {
            console.log("WiFi Disconnected")
        }

        // Fired on ANY state change
        function onWifiStateChanged(state) {
            console.log("WiFi State Changed:", state)
        }
    }

    ButtonGroup { id: tabGroup }

    Component.onCompleted: {
        console.log("[AutoConnect] Component loaded")
        console.log("[AutoConnect] autoConnectionId =", sessionModel.autoConnectionId)

        // Must call function → shouldShowRebootDialog()
        if (BootHelper.shouldShowRebootDialog()
                && sessionModel.autoConnectionId) {

            console.log("[AutoConnect] Reboot detected → scheduling auto connect for ID:",
                        sessionModel.autoConnectionId)

            connectRDServer(sessionModel.autoConnectionId, 5000)
        } else {
            console.log("[AutoConnect] No auto connect triggered → conditions not met")
            console.log("  bootHelper =", BootHelper.shouldShowRebootDialog())
            console.log("  autoConnectionId =", sessionModel.autoConnectionId)
        }
    }

    Timer {
        id: delayedConnectTimer
        interval: 5000
        repeat: false
        running: false

        onTriggered: {
            console.log("[AutoConnect] Timer triggered after", interval, "ms")

            console.log("[AutoConnect] Current page:", pageStack.currentItem.objectName)
            console.log("[AutoConnect] autoConnectionId:", sessionModel.autoConnectionId)

            if (pageStack.currentItem.objectName === "HomePage"
                    && sessionModel.autoConnectionId) {

                console.log("[AutoConnect] Navigating to auto-connect server page with ID:",
                            sessionModel.autoConnectionId)

                pageStack.push(autoConnectServer, {
                                   "connectionId": sessionModel.autoConnectionId
                               })
            } else {
                console.warn("[AutoConnect] Conditions failed → auto connect aborted")
            }
        }
    }

    function connectRDServer(connectionId, delayMs) {
        console.log("[AutoConnect] Preparing delayed connection")
        console.log("   → connectionId:", connectionId)
        console.log("   → delay:", delayMs, "ms")

        delayedConnectTimer.interval = delayMs
        delayedConnectTimer.start()

        console.log("[AutoConnect] Timer started")
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

                Connections {
                    target: serverInfo

                    // 1️⃣ Connecting started
                    function onRdpSessionStarted(id) {
                        if (id !== connectionId) return

                        showProgress.visible = true
                        showAlert("Connecting to server...", Type.Info)
                    }

                    // 2️⃣ Connected successfully
                    function onRdpConnected(id) {
                        if (id !== connectionId) return

                        showProgress.visible = false
                        showAlert("Connected successfully!", Type.Success)
                        removeAutoConnectPageIfActive()
                    }

                    // 3️⃣ Connection failed
                    function onRdpConnectionFailed(id, reason) {
                        if (id !== connectionId) return

                        showProgress.visible = false
                        showAlert("Connection failed: " + reason, Type.Error)
                        removeAutoConnectPageIfActive()
                    }

                    // 4️⃣ Server disconnected
                    function onRdpDisconnected(id) {
                        if (id !== connectionId) return

                        showProgress.visible = false
                        showAlert("RDP session disconnected", Type.Warning)
                        removeAutoConnectPageIfActive()
                    }
                }

                PrefsBusyIndicator {
                    id: showProgress
                    visible: false
                    anchors.centerIn: parent
                }

                contentItem: HomeTabButton {
                    id: tabButton
                    ButtonGroup.group: tabGroup
                    text: showProgress.visible ? "" : connectionName
                    icon.source: showProgress.visible ? "" : Qt.resolvedUrl("qrc:/assets/icons/rd-client.png")

                    onClicked: {
                        serverInfo.connectRdServer(connectionId)
                    }

                    background: Rectangle {
                        anchors.fill: parent
                        color: tabButton.checked ? Colors.accentHover : "#2A2A2A"
                        radius: 8
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
            objectName: "AutoConnect"
            onCancelled: {}
        }
    }

    Connections {
        target: dataBase
        function onRefreshTable() {
            serverModel.refresh()
        }
    }

    function removeAutoConnectPageIfActive() {
        const current = pageStack.currentItem

        if (!current) {
            console.warn("[AutoConnect] No current page → nothing to remove")
            return
        }

        console.log("[AutoConnect] Current page =", current.objectName)

        if (current.objectName === "AutoConnect") {
            console.log("[AutoConnect] AutoConnect page detected → removing")
            pageStack.pop()
        } else {
            console.log("[AutoConnect] AutoConnect page not active → no action")
        }
    }
}
