import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import App.Styles 1.0

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

    ButtonGroup { id: tabGroup }

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

                background: Rectangle {
                    color: Colors.steelGray
                    radius: height /2
                }

                contentItem: RowLayout {
                    spacing: 10

                    PrefsTabButton {
                        id: ethernet
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                        ButtonGroup.group: tabGroup
                        radius: height / 2
                        Layout.fillWidth: true
                        checked: true
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

                ListModel {
                    id: wifiModel

                    ListElement { ssid: "Home_Network" }
                    ListElement { ssid: "CoffeeShop_Wifi" }
                    ListElement { ssid: "Office_Network" }
                    ListElement { ssid: "Mobile_Hotspot" }
                }

                contentItem: ListView {
                    width: parent.width
                    implicitHeight: contentHeight

                    model: wifiModel

                    delegate: PrefsItemDelegate {
                        id: itemDelegate
                        width: ListView.view.width
                        text: modelData.ssid
                        hoverEnabled: true

                        background: Rectangle {
                            implicitHeight: 36
                            radius: height / 2
                            color: itemDelegate.hovered ? Colors.steelGray : "transparent"
                        }

                        contentItem: RowLayout {
                            PrefsLabel {
                                Layout.maximumWidth: itemDelegate.width / 2
                                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                                horizontalAlignment: Text.AlignLeft
                                verticalAlignment: Text.AlignVCenter
                                text: modelData
                            }

                            Item { Layout.fillWidth: true }

                            PrefsButton {
                                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                text: qsTr("Connect")
                                visible: itemDelegate.hovered
                                radius: height / 2
                                onClicked: {}
                            }

                            Icon {
                                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                icon: "qrc:/assets/icons/lock.svg"
                                iconWidth: 20
                                iconHeight: 20
                                onClicked: {}
                            }

                            Icon {
                                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                property int strength: 10
                                iconWidth: 20
                                iconHeight: 15
                                icon: {
                                    switch(strength) {
                                    case 0:
                                        return Qt.resolvedUrl("qrc:/assets/icons/full.svg")
                                    case 1:
                                        return Qt.resolvedUrl("qrc:/assets/icons/middle.svg")
                                    case 2:
                                        return Qt.resolvedUrl("qrc:/assets/icons/low.svg")
                                    case 3:
                                        return Qt.resolvedUrl("qrc:/assets/icons/lower.svg")
                                    case 4:
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
                                onClicked: {}
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

                        background: Rectangle {
                            color: Colors.steelGray
                            radius: height /2
                        }

                        indicator: RowLayout {
                            x: switchDelegate.width - width - switchDelegate.rightPadding
                            y: switchDelegate.topPadding + (switchDelegate.availableHeight - height) / 2
                            spacing: 10

                            Item {
                                Layout.fillWidth: true
                            }

                            RadioButton {
                                id: manualRadio
                                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                                checked: true
                                palette.text: Colors.accentPrimary
                                palette.windowText: Colors.textPrimary
                                text: qsTr("Manual")
                                visible: !!text
                                font.weight: Font.Normal
                            }

                            RadioButton {
                                id: dhcpRadio
                                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                                text: qsTr("DHCP")
                                palette.text: Colors.accentPrimary
                                palette.windowText: Colors.textPrimary
                                visible: !!text
                                font.weight: Font.Normal
                            }
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 20
                    }

                    PrefsItemDelegate {
                        id: enableEthernet
                        Layout.fillWidth: true
                        text: qsTr("Enable Ethernet")

                        indicator: Switch {
                            x: enableEthernet.width - width - enableEthernet.rightPadding
                            y: enableEthernet.topPadding + (enableEthernet.availableHeight - height) / 2
                        }
                    }

                    PrefsItemDelegate {
                        id: ipAddress
                        Layout.fillWidth: true
                        text: qsTr("IP Address")

                        indicator: PrefsTextField {
                            id: ipAddressField
                            x: ipAddress.width - width - ipAddress.rightPadding
                            y: ipAddress.topPadding + (ipAddress.availableHeight - height) / 2

                            placeholderText : qsTr("Enter %1").arg(ipAddress.text)

                            validator: RegExpValidator {
                                regExp:  /^((?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\.){0,3}(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])$/
                            }
                        }
                    }

                    PrefsItemDelegate {
                        id: netmask
                        Layout.fillWidth: true
                        text: qsTr("Netmask")

                        indicator: PrefsTextField {
                            id: netmaskField
                            x: netmask.width - width - netmask.rightPadding
                            y: netmask.topPadding + (netmask.availableHeight - height) / 2

                            placeholderText : qsTr("Enter %1").arg(netmask.text)
                            validator: RegExpValidator {
                                regExp:  /^((?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\.){0,3}(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])$/
                            }
                        }
                    }

                    PrefsItemDelegate {
                        id: gateWay
                        Layout.fillWidth: true
                        text: qsTr("Gateway")

                        indicator: PrefsTextField {
                            id: gateWayField
                            x: gateWay.width - width - gateWay.rightPadding
                            y: gateWay.topPadding + (gateWay.availableHeight - height) / 2

                            placeholderText : qsTr("Enter %1").arg(gateWay.text)
                            validator: RegExpValidator {
                                regExp:  /^((?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\.){0,3}(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])$/
                            }
                        }
                    }

                    PrefsItemDelegate {
                        id: dns
                        Layout.fillWidth: true
                        text: qsTr("DNS")

                        indicator: PrefsTextField {
                            id: dnsField
                            x: dns.width - width - dns.rightPadding
                            y: dns.topPadding + (dns.availableHeight - height) / 2

                            placeholderText : qsTr("Enter %1").arg(dns.text)
                            validator: RegExpValidator {
                                regExp:  /^((?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\.){0,3}(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])$/
                            }
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 20
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 20

                        PrefsButton {
                            text: qsTr("Wifi Mac")
                            radius: height / 2
                            highlighted: true
                            onClicked: {
                            }
                        }

                        PrefsButton {
                            text: qsTr("Auto Read")
                            radius: height / 2
                            highlighted: true
                            onClicked: {
                            }
                        }

                        Item { Layout.fillWidth: true }
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
            color: Qt.rgba(45/ 255, 47/255, 47/255, 1.0)
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

                PrefsButton {
                    id: disconnected
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    text: qsTr("Disconnect")
                    highlighted: true

                    background: Rectangle {
                        implicitHeight: 34
                        implicitWidth: 120
                        radius: height / 2

                        color: {
                            if (!disconnected.enabled) {
                                return disconnected.highlighted ? Colors.accentDisabled : Colors.btnBgDisabled;
                            }
                            return Colors.statusError;
                        }
                    }

                    onClicked: {

                    }
                }
            }
        }
    }
}
