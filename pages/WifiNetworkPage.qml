import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import App.Styles 1.0
import App.Enums 1.0

import "../controls"
import "../components"
import "../dialogs"

BasicPage {
    id: page
    StackView.visible: true
    padding: 20

    header: PageHeader {
        pageTitle: page.pageTitle
        onBackPressed: {
            if(pageStack.depth == 1) {
                backToHome()
                return
            }
            pageStack.pop()
        }
    }

    contentItem: Flickable {
        id: flickable
        width: parent.width
        clip: true
        contentHeight: layout.height
        contentWidth: layout.width

        ColumnLayout {
            id: layout
            width: page.width - 40
            spacing: 20
            clip: true

            Control {
                id: wifiControl
                Layout.fillWidth: true
                Layout.maximumHeight: flickable.height - 20
                padding: 20

                property int connectIndex: -1

                onVisibleChanged: {
                    if(visible) {
                        wifiNetworkDetails.getWifiDetails()
                    }
                }

                contentItem: ListView {
                    width: parent.width
                    implicitHeight: contentHeight

                    model: wifiNetworkDetails

                    delegate: PrefsItemDelegate {
                        id: itemDelegate
                        width: ListView.view.width
                        hoverEnabled: true

                        background: Rectangle {
                            implicitHeight: 36
                            radius: itemDelegate.radius
                            color: itemDelegate.hovered ? Colors.steelGray : "transparent"
                            border.width: 1
                            border.color: "transparent"
                        }

                        contentItem: RowLayout {
                            spacing: 10

                            Rectangle {
                                visible: wifiNetworkDetails.activeSsid === wifiDetails.ssid
                                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                                width: 14
                                height: 14
                                radius: height / 2
                                color: Colors.accentHover
                            }

                            ColumnLayout {
                                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                                PrefsLabel {
                                    Layout.maximumWidth: itemDelegate.width / 2
                                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                                    horizontalAlignment: Text.AlignLeft
                                    verticalAlignment: Text.AlignVCenter
                                    text: wifiDetails.ssid
                                }

                                PrefsLabel {
                                    Layout.maximumWidth: itemDelegate.width / 2
                                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                                    horizontalAlignment: Text.AlignLeft
                                    verticalAlignment: Text.AlignVCenter
                                    visible: wifiNetworkDetails.activeSsid === wifiDetails.ssid
                                    color: Colors.accentHover
                                    text: qsTr("Connected")
                                }
                            }

                            Item {Layout.fillWidth: true}
                        }

                        indicator: RowLayout {
                            x: itemDelegate.width - width - itemDelegate.rightPadding
                            y: itemDelegate.topPadding + (itemDelegate.availableHeight - height) / 2

                            RowLayout {
                                spacing: 20
                                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter

                                Connections {
                                    target: wifiNetworkDetails

                                    function onSigConnectionStarted() {
                                        if(index === wifiControl.connectIndex) {
                                            busyIndicator.running = true
                                        }
                                    }

                                    function onSigConnectionFinished() {
                                        if(index === wifiControl.connectIndex) {
                                            busyIndicator.running = false
                                            let ssid = wifiDetails.ssid
                                            let saveData = [ ssid, wifiSettings.getPassword(ssid) ]
                                            dataBase.insertIntoValues = saveData
                                            dataBase.qmlInsertWifiData()
                                            pageStack.pop()
                                        }
                                    }
                                }

                                PrefsButton {
                                    property bool forgotPassword: wifiSettings.hasSavedPassword(wifiDetails.ssid)
                                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                    text: forgotPassword ? qsTr("Forgot") :  qsTr("Connect")
                                    visible: itemDelegate.hovered && (wifiNetworkDetails.activeSsid !== wifiDetails.ssid || forgotPassword)
                                    radius: height / 2
                                    onClicked: {
                                        if(forgotPassword) {
                                            forgotNetwork()
                                        }else {
                                            connectWithSSID()
                                        }
                                    }

                                    function connectWithSSID() {
                                        if(!wifiDetails.security) {
                                            wifiNetworkDetails.connectToSsid(wifiDetails.ssid, "")
                                            wifiControl.connectIndex = index
                                            return
                                        }

                                        if(wifiSettings.hasSavedPassword(wifiDetails.ssid)){
                                            var savedPass = wifiSettings.getPassword(wifiDetails.ssid)
                                            wifiNetworkDetails.connectToSsid(wifiDetails.ssid, savedPass)
                                            wifiControl.connectIndex = index
                                            return
                                        }
                                        pageStack.push(setWifiPassword, {"connection_ssid" : wifiDetails.ssid})
                                        wifiControl.connectIndex = index
                                    }

                                    function forgotNetwork() {
                                        wifiSettings.clearPassword(wifiDetails.ssid)
                                        if(disconnectWifi.visible) {
                                            wifiNetworkDetails.disconnectWifiNetwork(wifiDetails.ssid)
                                        }
                                    }
                                }

                                PrefsBusyIndicator {
                                    id: busyIndicator
                                    radius: 10
                                    running: false
                                    visible: running
                                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                }

                                PrefsDangerButton {
                                    id: disconnectWifi
                                    visible: wifiNetworkDetails.activeSsid === wifiDetails.ssid
                                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                    text: qsTr("Disconnect")
                                    onClicked: {
                                        wifiNetworkDetails.disconnectWifiNetwork(wifiDetails.ssid)
                                    }
                                }
                            }

                            Icon {
                                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                icon: "qrc:/assets/icons/lock.svg"
                                visible: wifiDetails.security

                                iconWidth: 20
                                iconHeight: 20
                                onClicked: {}
                            }

                            Icon {
                                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                property int strength: wifiDetails.bars
                                iconWidth: 20
                                iconHeight: 15
                                icon: {
                                    switch(strength) {
                                    case WifiNetworkDetailsColl.StrengthExcellent:
                                        return Qt.resolvedUrl("qrc:/assets/icons/full.svg")
                                    case WifiNetworkDetailsColl.StrengthGood:
                                        return Qt.resolvedUrl("qrc:/assets/icons/middle.svg")
                                    case WifiNetworkDetailsColl.StrengthFair:
                                        return Qt.resolvedUrl("qrc:/assets/icons/low.svg")
                                    case WifiNetworkDetailsColl.StrengthWeak:
                                        return Qt.resolvedUrl("qrc:/assets/icons/lower.svg")
                                    case WifiNetworkDetailsColl.StrengthNone:
                                        return Qt.resolvedUrl("qrc:/assets/icons/no-signal.svg")
                                    default:
                                        return Qt.resolvedUrl("qrc:/assets/icons/no-wifi.svg")
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Component {
        id: wifiDetailsComponent
        WifiDetails {}
    }
    Component {
        id: setWifiPassword
        SetWifiPassword {}
    }
}
