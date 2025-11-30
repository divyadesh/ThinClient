import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import App.Styles 1.0
import App.Enums 1.0
import App.Backend 1.0

import "../controls"
import "../components"
import "../dialogs"

BasicPage {
    id: page
    StackView.visible: true
    padding: 20

    Connections {
        target: ethernetMonitor
        function onConnectedChanged() {
            if (ethernetMonitor.connected) {
                showAlert("Wi-Fi disconnected because an Ethernet connection is established.", Type.Info);
                removeWifiPopupPagesIfActive()
            }else {
                showAlert("Wi-Fi has been enabled because the Ethernet connection is disconnected.", Type.Info);
            }
            wifiNetworkDetails.scanWifiNetworksAsync()
        }
    }

    PrefsLabel {
        visible: ethernetMonitor.connected
        anchors.centerIn: parent
        leftPadding: 50
        rightPadding: 50
        width: parent.width
        text: qsTr("Wi-Fi cannot be enabled while an active Ethernet connection is detected. Disconnect Ethernet to proceed with Wi-Fi connectivity.")
        font.pixelSize: 16
        font.weight: Font.Normal
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        color: Colors.statusError
    }

    header: PageHeader {
        pageTitle: page.pageTitle
        onBackPressed: {
            if(pageStack.depth == 1) {
                backToHome()
                return
            }
            pageStack.pop()
        }

        Control {
            visible: !ethernetMonitor.connected
            anchors.right: parent.right
            anchors.rightMargin: 60
            anchors.verticalCenter: parent.verticalCenter

            contentItem: Image {
                scale: mouseArea.pressed ? 0.95 : 1.0
                sourceSize: Qt.size(38, 38)
                source: Qt.resolvedUrl("qrc:/assets/icons/ic_refresh.svg")
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                onClicked: {
                    wifiNetworkDetails.scanWifiNetworksAsync()
                }
            }
        }
    }

    contentItem: PrefsFlickable {
        id: formFlickable
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
                Layout.maximumHeight: formFlickable.height - 20
                padding: 20

                property int connectIndex: -1

                onVisibleChanged: {
                    if(visible) {
                        wifiNetworkDetails.getWifiDetails()
                    }
                }

                contentItem: ListView {
                    id: ssidListView
                    width: parent.width
                    implicitHeight: contentHeight

                    model: sortedWifiModel

                    delegate: PrefsItemDelegate {
                        id: itemDelegate
                        width: ListView.view.width
                        hoverEnabled: true

                        onClicked: {
                            const connected =  wifiNetworkDetails.activeSsid === wifiDetails.ssid
                            pageStack.push(setWifiPassword, {"currentSsid" : wifiDetails.ssid, connectRequested : !connected, hasSecurity: wifiDetails.security})
                        }

                        background: Rectangle {
                            implicitHeight: 36
                            radius: height / 2
                            color: itemDelegate.hovered ? "#2A2A2A" : "transparent"
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

                            Icon {
                                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                icon: wifiNetworkDetails.activeSsid === wifiDetails.ssid ? "qrc:/assets/icons/menu.svg" : ""
                                iconWidth: 20
                                iconHeight: 20
                                onClicked: {
                                    if(wifiNetworkDetails.activeSsid === wifiDetails.ssid) {
                                        pageStack.push(ssidDetailsPage, {"wifiSSID": wifiDetails.ssid})
                                    }
                                }
                            }

                        }
                    }

                    footer:  PrefsItemDelegate {
                        id: addSSIDDelegate
                        width: ListView.view.width
                        hoverEnabled: true
                        visible: !ethernetMonitor.connected && ssidListView.count > 0
                        text: qsTr("Add Network")

                        background: Rectangle {
                            implicitHeight: 36
                            radius: height / 2
                            color: addSSIDDelegate.hovered ? "#2A2A2A" : "transparent"
                        }

                        indicator: PrefsButton {
                            x: addSSIDDelegate.width - width - addSSIDDelegate.rightPadding
                            y: addSSIDDelegate.topPadding + (addSSIDDelegate.availableHeight - height) / 2
                            text: qsTr("Add")
                            onClicked: {
                                pageStack.push(addNetwork)
                            }
                        }
                    }

                }
            }
        }
    }

    function removeWifiPopupPagesIfActive() {
        const current = pageStack.currentItem

        if (!current) {
            console.warn("[WiFiPopup] No current page → nothing to remove")
            return
        }

        const popupNames = [
            "WifiDetails",
            "SetWifiPassword",
            "AddNetwork",
            "WifiNetworkDetailsDIalog"
        ]

        console.log("[WiFiPopup] Current page =", current.objectName)

        if (popupNames.indexOf(current.objectName) !== -1) {
            console.log("[WiFiPopup] Popup detected → closing:", current.objectName)
            pageStack.pop()
        } else {
            console.log("[WiFiPopup] No matching popup active → no action")
        }
    }

    Component {
        id: wifiDetailsComponent
        WifiDetails {
            objectName: "WifiDetails"
        }
    }
    Component {
        id: setWifiPassword
        SetWifiPassword {
            objectName: "SetWifiPassword"
        }
    }

    Component {
        id: addNetwork
        AddNetwork {
            objectName: "AddNetwork"
        }
    }

    Component {
        id: ssidDetailsPage
        WifiNetworkDetailsDIalog {
            objectName: "WifiNetworkDetailsDIalog"
        }
    }
}
