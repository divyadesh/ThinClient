import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import App.Styles 1.0
import App.Enums 1.0

import "../controls"
import "../components"

BasicPage {
    id: page
    padding: 20
    property bool addNewServer: false

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

    ButtonGroup {
        id: tabGroup
        onCheckedButtonChanged: {
            if (checkedButton) {
                if(checkedButton.text)
                    persistData.saveData("Network", checkedButton.text)
            }
        }
    }

    ButtonGroup { id: ethernetGroup }

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
                id: header
                Layout.fillWidth: true
                padding: 5

                property string networkPersistData: persistData.getData("Network")

                background: Rectangle {
                    color: Colors.steelGray
                    radius: height /2
                    border.width: 1
                    border.color: Colors.borderColor
                }

                contentItem: RowLayout {
                    spacing: 10

                    PrefsTabButton {
                        id: ethernet
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                        ButtonGroup.group: tabGroup
                        radius: height / 2
                        Layout.fillWidth: true
                        checked: header.networkPersistData === "Ethernet" || header.networkPersistData === ""
                        text: qsTr("Ethernet")
                        visible: !!text
                        font.weight: Font.Normal
                    }

                    PrefsTabButton {
                        id: wifi
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                        ButtonGroup.group: tabGroup
                        Layout.fillWidth: true
                        radius: height / 2
                        checked: header.networkPersistData === "WiFi"
                        text: qsTr("WiFi")
                        visible: !!text
                        font.weight: Font.Normal
                    }
                }
            }

            Control {
                visible: wifi.checked
                Layout.fillWidth: true
                Layout.maximumHeight: flickable.height - header.implicitHeight - 20
                padding: 20

                background: Rectangle {
                    color: Colors.btnBg
                    radius: 8
                }

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
                        //text: modelData.ssid
                        hoverEnabled: true

                        background: Rectangle {
                            implicitHeight: 36
                            radius: height / 2
                            color: itemDelegate.hovered ? Colors.steelGray : "transparent"
                            border.width: 1
                            border.color: itemDelegate.hovered ? Colors.borderColor : "transparent"
                        }

                        contentItem: RowLayout {
                            PrefsLabel {
                                Layout.maximumWidth: itemDelegate.width / 2
                                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                                horizontalAlignment: Text.AlignLeft
                                verticalAlignment: Text.AlignVCenter
                                text: wifiDetails.ssid
                            }

                            Item { Layout.fillWidth: true }

                            RowLayout {
                                spacing: 20
                                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter

                                // PrefsBusyIndicator {
                                //     radius: 10
                                //     running: processing
                                //     visible: running
                                //     Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                // }

                                PrefsDangerButton {
                                    id: disconnectWifi
                                    visible: wifiNetworkDetails.activeSsid === wifiDetails.ssid
                                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                    text: qsTr("Disconnect")
                                    onClicked: {
                                        wifiNetworkDetails.disconnectWifiNetwork(wifiDetails.ssid)
                                    }
                                }

                                PrefsButton {
                                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                    text: qsTr("Connect")
                                    visible: itemDelegate.hovered && wifiNetworkDetails.activeSsid !== wifiDetails.ssid
                                    radius: height / 2
                                    onClicked: {
                                        wifiNetworkDetails.connectToSsid(wifiDetails.ssid, "9013779904");
                                    }
                                }
                            }

                            Icon {
                                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                icon: "qrc:/assets/icons/lock.svg"
                                //visible: secured

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
                                icon: "qrc:/assets/icons/menu.svg"
                                iconWidth: 20
                                iconHeight: 20
                                onClicked: {

                                }
                            }
                        }
                    }
                }
            }


            Control {
                visible: ethernet.checked
                Layout.fillWidth: true
                padding: 20

                background: Rectangle {
                    color: Colors.btnBg
                    radius: 8
                }

                // Ethernet Panel (right) - this is what you asked for
                contentItem:  ColumnLayout {
                    spacing: 8

                    ItemDelegate {
                        id: switchDelegate
                        Layout.fillWidth: true
                        padding: 5
                        leftPadding: 20
                        rightPadding: 20

                        property string ethPersistData: persistData.getData("Ethernet")

                        background: Rectangle {
                            color: Colors.steelGray
                            radius: height /2
                            border.width: 1
                            border.color: Colors.borderColor
                        }

                        indicator: RowLayout {
                            x: switchDelegate.width - width - switchDelegate.rightPadding
                            y: switchDelegate.topPadding + (switchDelegate.availableHeight - height) / 2
                            spacing: 10

                            Item {
                                Layout.fillWidth: true
                            }

                            RadioButton {
                                id: dhcpRadio
                                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                                text: qsTr("DHCP")
                                checked: switchDelegate.ethPersistData === "DHCP" || switchDelegate.ethPersistData === ""
                                palette.text: Colors.accentPrimary
                                palette.windowText: Colors.textPrimary
                                visible: !!text
                                font.weight: Font.Normal
                                onCheckedChanged: {
                                    if(checked)
                                        persistData.saveData("Ethernet", dhcpRadio.text)
                                }
                            }

                            RadioButton {
                                id: manualRadio
                                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                                checked: switchDelegate.ethPersistData === "Manual"
                                palette.text: Colors.accentPrimary
                                palette.windowText: Colors.textPrimary
                                text: qsTr("Manual")
                                visible: !!text
                                font.weight: Font.Normal
                                onCheckedChanged: {
                                    if(checked)
                                        persistData.saveData("Ethernet", manualRadio.text)
                                }
                            }
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 20
                    }

                    PrefsItemDelegate {
                        id: ipAddress
                        Layout.fillWidth: true
                        text: qsTr("IP Address")
                        enabled: manualRadio.checked

                        indicator: PrefsTextField {
                            id: ipAddressField
                            x: ipAddress.width - width - ipAddress.rightPadding
                            y: ipAddress.topPadding + (ipAddress.availableHeight - height) / 2

                            placeholderText : qsTr("Enter %1").arg(ipAddress.text)

                            validator: RegularExpressionValidator {
                                regularExpression:  /^((?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\.){0,3}(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])$/
                            }
                        }
                    }

                    PrefsItemDelegate {
                        id: netmask
                        Layout.fillWidth: true
                        text: qsTr("Netmask")
                        enabled: manualRadio.checked

                        indicator: PrefsTextField {
                            id: netmaskField
                            x: netmask.width - width - netmask.rightPadding
                            y: netmask.topPadding + (netmask.availableHeight - height) / 2

                            placeholderText : qsTr("Enter %1").arg(netmask.text)
                            validator: RegularExpressionValidator {
                                regularExpression:  /^((?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\.){0,3}(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])$/
                            }
                        }
                    }

                    PrefsItemDelegate {
                        id: gateWay
                        Layout.fillWidth: true
                        text: qsTr("Gateway")
                        enabled: manualRadio.checked

                        indicator: PrefsTextField {
                            id: gateWayField
                            x: gateWay.width - width - gateWay.rightPadding
                            y: gateWay.topPadding + (gateWay.availableHeight - height) / 2

                            placeholderText : qsTr("Enter %1").arg(gateWay.text)
                            validator: RegularExpressionValidator {
                                regularExpression:  /^((?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\.){0,3}(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])$/
                            }
                        }
                    }

                    PrefsItemDelegate {
                        id: dns
                        Layout.fillWidth: true
                        text: qsTr("DNS")
                        enabled: manualRadio.checked

                        indicator: PrefsTextField {
                            id: dnsField
                            x: dns.width - width - dns.rightPadding
                            y: dns.topPadding + (dns.availableHeight - height) / 2

                            placeholderText : qsTr("Enter %1").arg(dns.text)
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 20
                    }

                    PrefsLabel {
                        Layout.alignment: Qt.AlignLeft
                        Layout.fillWidth: true
                        font.pixelSize: 20
                        text: qsTr("Wi-Fi Mac Address: 0A:AA:BB:CC:DD:EE")
                    }
                }
            }
        }
    }

    footer: Control {
        padding: 20
        clip: true
        visible: ethernet.checked

        background: Rectangle {
            implicitHeight: 84
            color: Colors.headerColor
        }

        contentItem: ColumnLayout {
            spacing: 10

            RowLayout {
                Layout.fillWidth: true
                spacing: 20

                Item { Layout.fillWidth: true }

                PrefsButton {
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    text: qsTr("Apply")
                    radius: height / 2
                    highlighted: true
                    onClicked: {
                    }
                }

                PrefsDangerButton {
                    id: disconnected
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    text: qsTr("Disconnect")
                    onClicked: {}
                }
            }
        }
    }
}
