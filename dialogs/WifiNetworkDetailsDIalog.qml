import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.3

import App.Styles 1.0
import AppSecurity 1.0
import AddNetworkEnums 1.0
import App.Backend 1.0

import "../pages"
import "../components"
import "../controls"

BasicPage {
    id: control
    StackView.visible: true
    property real popupHeight: parent.height * 0.7
    property real popupWidth: 512

    property string acceptedButtonText: "Save"
    property string rejectedButtonText: "Cancel"
    property string wifiSSID: ""

    signal accepted()
    signal rejected()

    background: BackgroundOverlay {}

    pageTitle: qsTr("Network Details")

    WiFiManager {
        id: wifi
        ssid: wifiSSID
    }

    Component.onCompleted: wifi.startAutoRefresh()
    Component.onDestruction: wifi.stopAutoRefresh()

    Page {
        id: page
        anchors.centerIn: parent
        width: control.popupWidth
        height: control.popupHeight
        background: DialogBackground{}

        header: Control {
            implicitHeight: 52
            padding: 10
            topPadding: 16

            contentItem: PrefsLabel {
                text: control.pageTitle
                font.pixelSize: 24
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        Flickable {
            id: flickable
            anchors.fill: parent
            contentHeight: layout.height
            contentWidth: parent.width
            clip: true

            ColumnLayout {
                id: layout
                width: parent.width
                spacing: 20

                // ============================================================================
                // FIRST FRAME — GENERAL WI-FI DETAILS
                // ============================================================================
                PrefsFrame {
                    id: wifiDetailsFrame
                    Layout.fillWidth: true

                    ColumnLayout {
                        anchors.fill: parent

                        /* ---------------------------
                         * CONNECTION SSID
                         * --------------------------- */
                        PrefsTextFieldSubDelegate {
                            id: ssidField
                            text: qsTr("SSID")
                            textFieldText: wifiSSID
                            textFieldPlaceholderText: qsTr("Thin client ...")
                            readOnly: true
                        }

                        PrefsSeparator {}

                        /* ---------------------------
                         * CONNECTION STATUS
                         * --------------------------- */
                        PrefsTextFieldSubDelegate {
                            id: statusField
                            text: qsTr("Status")
                            textFieldText: wifi.status
                            textFieldPlaceholderText: qsTr("Connected / Disconnected")
                            readOnly: true
                        }

                        PrefsSeparator {}


                        /* ---------------------------
                         * SIGNAL STRENGTH
                         * --------------------------- */
                        PrefsTextFieldSubDelegate {
                            id: signalField
                            text: qsTr("Signal strength")
                            textFieldText: wifi.signalStrength
                            textFieldPlaceholderText: qsTr("-70 dBm")
                            readOnly: true
                        }

                        PrefsSeparator {}


                        /* ---------------------------
                         * LINK SPEED
                         * --------------------------- */
                        PrefsTextFieldSubDelegate {
                            id: linkSpeedField
                            text: qsTr("Link speed")
                            textFieldText: wifi.linkSpeed
                            textFieldPlaceholderText: qsTr("72 Mbps")
                            readOnly: true
                        }

                        PrefsSeparator {}


                        /* ---------------------------
                         * SECURITY TYPE
                         * --------------------------- */
                        PrefsTextFieldSubDelegate {
                            id: securityField
                            text: qsTr("Security")
                            textFieldText: wifi.security
                            textFieldPlaceholderText: qsTr("WPA2-PSK")
                            readOnly: true
                        }

                        PrefsSeparator {}


                        /* ---------------------------
                         * MAC ADDRESS
                         * --------------------------- */
                        PrefsTextFieldSubDelegate {
                            id: macField
                            text: qsTr("MAC address")
                            textFieldText: wifi.macAddress
                            textFieldPlaceholderText: qsTr("AA:BB:CC:DD:EE:FF")
                            readOnly: true
                        }

                        PrefsSeparator {}


                        /* ---------------------------
                         * IPv4 ADDRESS
                         * --------------------------- */
                        PrefsTextFieldSubDelegate {
                            id: ipv4Field
                            text: qsTr("IPv4 address")
                            textFieldText: wifi.ipAddress
                            textFieldPlaceholderText: qsTr("192.168.1.100")
                            readOnly: true
                        }
                    }
                }

                // ============================================================================
                // SECOND FRAME — NETWORK SETTINGS
                // ============================================================================
                PrefsFrame {
                    Layout.fillWidth: true

                    ColumnLayout {
                        anchors.fill: parent

                        // ---------------------------
                        // PROXY SETTINGS
                        // ---------------------------
                        PrefsComboBoxDelegate {
                            id: proxyCombo
                            text: qsTr("Proxy")

                            model: [
                                { name: "None",        typeId: AppEnums.proxyNone },
                                { name: "Manual",      typeId: AppEnums.proxyManual },
                                { name: "Auto-config", typeId: AppEnums.proxyAuto }
                            ]

                            textRole: "name"
                            valueRole: "typeId"

                            onCurrentValueChanged: {
                                console.log("[ADD NETWORK] Proxy changed:", currentValue)

                                if (currentValue === AppEnums.proxyNone) {
                                    console.log(" → No Proxy")
                                } else if (currentValue === AppEnums.proxyManual) {
                                    console.log(" → Manual Proxy")
                                } else if (currentValue === AppEnums.proxyAuto) {
                                    console.log(" → Auto Proxy (PAC)")
                                }
                            }
                        }

                        PrefsSeparator {}

                        // ---------------------------
                        // IP MODE (DHCP / STATIC)
                        // ---------------------------
                        PrefsComboBoxDelegate {
                            id: ipModeCombo
                            text: qsTr("IP settings")

                            model: [
                                { name: "DHCP",  typeId: AppEnums.ipDHCP },
                                { name: "Static", typeId: AppEnums.ipStatic }
                            ]

                            textRole: "name"
                            valueRole: "typeId"

                            onCurrentValueChanged: {
                                console.log("[ADD NETWORK] IP Mode changed:", currentValue)

                                if (currentValue === AppEnums.ipDHCP) {
                                    console.log(" → DHCP Enabled")
                                } else if (currentValue === AppEnums.ipStatic) {
                                    console.log(" → Static IP Enabled")
                                }
                            }
                        }

                        // ---------------------------
                        // IP CONFIG (Visible when Static)
                        // ---------------------------
                        ColumnLayout {
                            Layout.fillWidth: true
                            visible: Number(ipModeCombo.currentValue) === AppEnums.ipStatic

                            PrefsSeparator {}

                            PrefsTextFieldSubDelegate {
                                id: ipField
                                text: qsTr("IP address")
                                textFieldText: wifi.ipAddress
                                textFieldPlaceholderText: "192.168.10.1"
                            }

                            PrefsSeparator {}

                            PrefsTextFieldSubDelegate {
                                id: routerField
                                text: qsTr("Router")
                                textFieldText: wifi.gateway
                                textFieldPlaceholderText: "192.168.10.1"
                            }

                            PrefsSeparator {}

                            PrefsTextFieldSubDelegate {
                                id: prefixField
                                text: qsTr("Prefix length")
                                textFieldText: wifi.subnetMask
                                textFieldPlaceholderText: "24"
                            }

                            PrefsSeparator {}

                            PrefsTextFieldSubDelegate {
                                id: dns1Field
                                text: qsTr("DNS 1")
                                textFieldText: wifi.dnsServers[0]
                                textFieldPlaceholderText: "0.0.0.0"
                            }

                            PrefsSeparator {}

                            PrefsTextFieldSubDelegate {
                                id: dns2Field
                                text: qsTr("DNS 2")
                                textFieldText: wifi.dnsServers[1]
                                textFieldPlaceholderText: "0.0.0.0"
                            }
                        }

                        PrefsSeparator {}

                        // ---------------------------
                        // MAC RANDOMIZATION
                        // ---------------------------
                        PrefsComboBoxDelegate {
                            id: macModeCombo
                            text: qsTr("Privacy")

                            model: [
                                { name: "Use device MAC address", typeId: AppEnums.macDevice },
                                { name: "Use random MAC address", typeId: AppEnums.macRandom }
                            ]

                            textRole: "name"
                            valueRole: "typeId"

                            onCurrentValueChanged: {
                                console.log("[ADD NETWORK] MAC Mode changed:", currentValue)

                                if (currentValue === AppEnums.macDevice) {
                                    console.log(" → Using Device MAC Address")
                                } else if (currentValue === AppEnums.macRandom) {
                                    console.log(" → Using Randomized MAC Address")
                                }
                            }
                        }

                        PrefsSeparator {}

                        // ---------------------------
                        // METERED CONNECTION TYPE
                        // ---------------------------
                        PrefsComboBoxDelegate {
                            id: meteredCombo
                            text: qsTr("Metered")

                            model: [
                                { name: "Detect automatically", typeId: AppEnums.meteredAuto },
                                { name: "Metered",              typeId: AppEnums.meteredYes },
                                { name: "Unmetered",            typeId: AppEnums.meteredNo }
                            ]

                            textRole: "name"
                            valueRole: "typeId"

                            onCurrentValueChanged: {
                                console.log("[ADD NETWORK] Metered changed:", currentValue)

                                if (currentValue === AppEnums.meteredAuto) {
                                    console.log(" → Detect Automatically")
                                } else if (currentValue === AppEnums.meteredYes) {
                                    console.log(" → Metered (Data Sensitive)")
                                } else if (currentValue === AppEnums.meteredNo) {
                                    console.log(" → Unmetered (Unlimited)")
                                }
                            }
                        }

                        PrefsSeparator {}

                        // ---------------------------
                        // HIDDEN NETWORK OPTION
                        // ---------------------------
                        PrefsComboBoxDelegate {
                            id: hiddenCombo
                            text: qsTr("Hidden network")

                            model: [
                                { name: "No",  typeId: AppEnums.hiddenNo },
                                { name: "Yes", typeId: AppEnums.hiddenYes }
                            ]

                            textRole: "name"
                            valueRole: "typeId"

                            onCurrentValueChanged: {
                                console.log("[ADD NETWORK] Hidden Network changed:", currentValue)

                                if (currentValue === AppEnums.hiddenNo) {
                                    console.log(" → Network is Visible")
                                } else if (currentValue === AppEnums.hiddenYes) {
                                    console.log(" → Hidden Network (SSID not broadcast)")
                                }
                            }
                        }
                    }
                }
            }
        }

        footer: Control {
            implicitHeight: 72

            contentItem: RowLayout {
                spacing: 20

                Item {
                    Layout.fillWidth: true
                }

                PrefsButton {
                    text: control.rejectedButtonText
                    radius: height / 2
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: {
                        control.rejected()
                        pageStack.pop()
                    }
                }

                PrefsButton {
                    text: control.acceptedButtonText
                    radius: height / 2
                    highlighted: true
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: {
                        control.accepted()
                        pageStack.pop()
                    }
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }
    }
}
