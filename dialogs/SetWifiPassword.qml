import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

import App.Styles 1.0
import AppSecurity 1.0
import App.Backend 1.0

import "../pages"
import "../components"
import "../controls"
import "../"

BasicPage {
    id: control
    property string currentSsid: ""
    property bool connectRequested: false
    property bool hasSecurity: false
    property bool showProgress: false
    property bool showError: false
    property string errorMessage: ""
    property string password: wifiSettings.getPassword(currentSsid)

    background: BackgroundOverlay {}

    Page {
        anchors.centerIn: parent
        width: 520
        background: DialogBackground{}

        header: Control {
            implicitHeight: 52
            padding: 10
            topPadding: 16

            contentItem: PrefsLabel {
                text: qsTr("Wifi Password")
                font.pixelSize: 24
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        contentItem: Loader {
            anchors.fill: parent
            sourceComponent: showProgress ? prosess_com : (connectRequested ? enterPassword_Com : forgot_Com)
        }

        footer: Control {
            implicitHeight: 72

            contentItem: RowLayout {
                spacing: 20

                Item {
                    Layout.fillWidth: true
                }

                PrefsButton {
                    text: qsTr("Cancel")
                    radius: height / 2
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: pageStack.pop()
                }

                PrefsButton {
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignVCenter
                    visible: connectRequested
                    radius: height / 2
                    text: qsTr("Connect")
                    onClicked: {
                        if(control.password.trim()) {
                            showProgress = true
                            wifiNetworkDetails.connectToSsid(currentSsid, control.password.trim())
                            return
                        }

                        connectWithSSID()
                    }

                    function connectWithSSID() {
                        if(!hasSecurity) {
                            showProgress = true
                            wifiNetworkDetails.connectToSsid(currentSsid, "")
                            return
                        }

                        if(wifiSettings.hasSavedPassword(currentSsid)){
                            showProgress = true
                            const savedPass = wifiSettings.getPassword(currentSsid)
                            wifiNetworkDetails.connectToSsid(currentSsid, savedPass)
                            return
                        }

                        showError = true
                        showAlert(qsTr("Please enter the password to proceed."), Type.Error)
                    }
                }

                PrefsButton {
                    property bool forgotPassword: wifiSettings.hasSavedPassword(currentSsid)
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignVCenter
                    text: qsTr("Forgot")
                    radius: height / 2
                    visible: (forgotPassword || !connectRequested)
                    onClicked: {
                        showProgress = true
                        forgotNetwork()
                    }

                    function forgotNetwork() {
                        wifiSettings.clearPassword(currentSsid)
                        wifiNetworkDetails.forgetWifiNetwork(currentSsid)
                    }
                }

                PrefsDangerButton {
                    id: disconnectWifi
                    visible: !connectRequested
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignVCenter
                    text: qsTr("Disconnect")
                    onClicked: {
                        showProgress = true
                        wifiNetworkDetails.disconnectWifiNetwork(currentSsid)
                    }
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }
    }

    Component {
        id: enterPassword_Com
        Control {
            id: enterPassword
            padding: 20
            leftPadding: 50
            rightPadding: 50

            contentItem: ColumnLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                spacing: 20

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }

                PrefsLabel {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    text: qsTr("Enter the password below to connect to this Wi-Fi network.")
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                PrefsTextField {
                    id: enterPasswordField
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    horizontalAlignment: TextField.AlignLeft
                    echoMode: eyeButton.checked ? TextField.Normal : TextField.Password
                    rightPadding: 40
                    text: control.password
                    onTextChanged: {
                        showError = false
                        errorMessage = ""
                        control.password = enterPasswordField.text.trim()
                    }

                    ToolButton {
                        id: eyeButton
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.rightMargin: 6
                        checkable: true
                        checked: false
                        z: 10
                        icon.source: checked ? "qrc:/assets/icons/ic_eye-on.svg" : "qrc:/assets/icons/ic_eye-off.svg"
                        icon.color: Colors.textSecondary
                        background: Item {
                            implicitWidth: 28
                            implicitHeight: 28
                        }
                    }
                }

                PrefsLabel {
                    visible: showError
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    text: errorMessage ? errorMessage : qsTr("Please enter the password to proceed.")
                    font.pixelSize: 14
                    color: Colors.statusError
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
            }
        }
    }

    Component {
        id: forgot_Com
        Control {
            id: process
            padding: 20
            leftPadding: 50
            rightPadding: 50

            contentItem: ColumnLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                spacing: 10

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }

                PrefsLabel {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    text: qsTr("This Wi-Fi network is already connected.\nIf you're having trouble, you may need to forget the saved password.")
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
            }
        }
    }

    Component {
        id: prosess_com
        Control {
            id: process
            padding: 20
            leftPadding: 50
            rightPadding: 50

            Connections {
                target: wifiMonitor

                // Fired when WiFi connects
                function onWifiConnected(ssid) {
                    showProgress = false
                    const password = control.password.trim()
                    wifiSettings.savePassword(control.currentSsid, password)
                    let saveData = [ ssid, password]
                    dataBase.insertIntoValues = saveData
                    dataBase.qmlInsertWifiData()
                    showAlert(qsTr("Connected successfully!"), Type.Success)
                    pageStack.pop()
                }

                function onErrorMessage(msg) {
                    errorMessage = msg
                    showProgress = false
                    showError = true
                    showAlert(msg, Type.Error)
                }

                function onWifiAuthFailed() {
                    showProgress = false
                    showError = true
                }

                // Fired when WiFi disconnects
                function onWifiDisconnected() {
                    showProgress = false
                    showAlert(qsTr("Disconnected successfully!"), Type.Success)
                    pageStack.pop()
                }

                // Fired on ANY state change
                function onWifiStateChanged(state) {
                    console.log("WiFi State Changed:", state)
                }
            }

            contentItem: ColumnLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                spacing: 20

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }

                PrefsLabel {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    text: qsTr("Connecting to this Wi-Fi network… Please wait.")
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                PrefsBusyIndicator {
                    id: busyIndicator
                    radius: 10
                    running: showProgress
                    visible: running
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
            }
        }
    }
}
