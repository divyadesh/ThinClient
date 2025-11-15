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
                Layout.fillWidth: true
                padding: 20

                contentItem:  ColumnLayout {
                    spacing: 8

                    PrefsItemDelegate {
                        id: switchDelegate
                        Layout.fillWidth: true
                        padding: 5
                        leftPadding: 20
                        rightPadding: 20
                        radius: height / 2

                        property string ethPersistData: persistData ? persistData.getData("Ethernet") : ""

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
                                    if(checked) {
                                        dnsNetworkInfo.updateInfo()
                                    }
                                }

                                onVisibleChanged: {
                                    if(visible && checked) {
                                        dnsNetworkInfo.updateInfo()
                                    }
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

                        property string manualIpAddress: ""

                        indicator: PrefsTextField {
                            id: ipAddressField
                            x: ipAddress.width - width - ipAddress.rightPadding
                            y: ipAddress.topPadding + (ipAddress.availableHeight - height) / 2

                            placeholderText : qsTr("Enter %1").arg(ipAddress.text)

                            text: dhcpRadio.checked ? dnsNetworkInfo.ipAddress : ipAddress.manualIpAddress
                            onTextChanged: {
                                if (manualRadio.checked) {
                                    ipAddress.manualIpAddress = text;
                                }
                            }
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

                        property string manualNetmask: ""

                        indicator: PrefsTextField {
                            id: netmaskField
                            x: netmask.width - width - netmask.rightPadding
                            y: netmask.topPadding + (netmask.availableHeight - height) / 2

                            placeholderText : qsTr("Enter %1").arg(netmask.text)

                            text: dhcpRadio.checked ? dnsNetworkInfo.netmask : netmask.manualNetmask
                            onTextChanged: {
                                if (manualRadio.checked) {
                                    netmask.manualNetmask = text;
                                }
                            }
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

                        property string manualGateway: ""

                        indicator: PrefsTextField {
                            id: gateWayField
                            x: gateWay.width - width - gateWay.rightPadding
                            y: gateWay.topPadding + (gateWay.availableHeight - height) / 2

                            placeholderText : qsTr("Enter %1").arg(gateWay.text)

                            text: dhcpRadio.checked ? dnsNetworkInfo.gateway : gateWay.manualGateway
                            onTextChanged: {
                                if (manualRadio.checked) {
                                    gateWay.manualGateway = text;
                                }
                            }
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

                        property string manualDns: ""

                        indicator: PrefsTextField {
                            id: dnsField
                            x: dns.width - width - dns.rightPadding
                            y: dns.topPadding + (dns.availableHeight - height) / 2

                            text: dhcpRadio.checked && dnsNetworkInfo.dnsServers.length > 0
                                  ? dnsNetworkInfo.dnsServers[0]
                                  : dns.manualDns
                            placeholderText : qsTr("Enter %1").arg(dns.text)
                            validator: RegularExpressionValidator {
                                regularExpression:  /^((?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\.){0,3}(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])$/
                            }
                        }
                    }

                    PrefsItemDelegate {
                        id: macAddress
                        Layout.fillWidth: true
                        text: qsTr("Mac Address")
                        enabled: dhcpRadio.checked
                        visible: ethernetNetworkController.macAddress.length > 5

                        indicator: PrefsTextField {
                            x: macAddress.width - width - macAddress.rightPadding
                            y: macAddress.topPadding + (macAddress.availableHeight - height) / 2

                            text: ethernetNetworkController.macAddress
                            readOnly: true
                            background: null
                        }
                    }

                    ItemDelegate {
                        id: saveManual
                        Layout.fillWidth: true
                        background: null
                        visible: manualRadio.checked

                        indicator:  PrefsButton {
                            x: saveManual.width - width - saveManual.rightPadding
                            y: saveManual.topPadding + (saveManual.availableHeight - height) / 2

                            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter

                            enabled: ipAddressField.text.length > 0 && netmaskField.text.length > 0 && gateWayField.text.length > 0
                            text: qsTr("Apply")
                            radius: height / 2
                            highlighted: enabled
                            onClicked: {
                                ethernetNetworkController.setManualConfig(ipAddressField.text, netmaskField.text, gateWayField.text, dnsField.text)
                                if(persistData) {
                                    persistData.saveData("Ethernet", "DHCP")
                                }
                            }
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 20
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
