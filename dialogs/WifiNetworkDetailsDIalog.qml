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
    property string errorMessage: ""

    signal accepted()
    signal rejected()

    background: BackgroundOverlay {}

    pageTitle: qsTr("Network Details")

    WiFiManager {
        id: wifi
        ssid: wifiSSID
        onLogMessage: {
            console.log("[WiFi]", message)
        }
    }

    Component.onCompleted: {
        wifi.updateIpMode()
        wifi.startAutoRefresh()
    }
    Component.onDestruction: wifi.stopAutoRefresh()

    Page {
        id: page
        anchors.centerIn: parent
        width: control.popupWidth
        height: Math.min(control.popupHeight, layout.height + 125)
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

        PrefsFlickable {
            id: formFlickable
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
                            textFieldPlaceholderText: qsTr("e.g. ThinClient")
                            readOnly: true
                            formField.onActiveFocusChanged: {
                                if(formField.activeFocus) {
                                    formFlickable.ensureVisible(ssidField)
                                }
                            }
                        }

                        PrefsSeparator {}

                        /* ---------------------------
                         * CONNECTION STATUS
                         * --------------------------- */
                        PrefsTextFieldSubDelegate {
                            id: statusField
                            text: qsTr("Status")
                            textFieldText: wifi.status
                            textFieldPlaceholderText: qsTr("e.g. Connected / Disconnected")
                            readOnly: true
                            formField.onActiveFocusChanged: {
                                if(formField.activeFocus) {
                                    formFlickable.ensureVisible(statusField)
                                }
                            }
                        }

                        PrefsSeparator {}

                        /* ---------------------------
                         * SECURITY TYPE
                         * --------------------------- */
                        PrefsTextFieldSubDelegate {
                            id: securityField
                            text: qsTr("Security")
                            textFieldText: wifi.security
                            textFieldPlaceholderText: qsTr("e.g. WPA2-PSK")
                            readOnly: true
                            formField.onActiveFocusChanged: {
                                if(formField.activeFocus) {
                                    formFlickable.ensureVisible(securityField)
                                }
                            }
                        }

                        PrefsSeparator {}


                        /* ---------------------------
                         * MAC ADDRESS
                         * --------------------------- */
                        PrefsTextFieldSubDelegate {
                            id: macField
                            text: qsTr("MAC address")
                            textFieldText: wifi.macAddress
                            textFieldPlaceholderText: qsTr("e.g. AA:BB:CC:DD:EE:FF")
                            readOnly: true
                            formField.onActiveFocusChanged: {
                                if(formField.activeFocus) {
                                    formFlickable.ensureVisible(macField)
                                }
                            }
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
                            formField.onActiveFocusChanged: {
                                if(formField.activeFocus) {
                                    formFlickable.ensureVisible(ipv4Field)
                                }
                            }
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
                            visible: false
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

                        PrefsSeparator {
                            visible: false
                        }

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
                            currentIndex: wifi.isStaticIp ? 1 : 0

                            onCurrentValueChanged: {
                                console.log("[ADD NETWORK] IP Mode changed:", currentValue)

                                if (currentValue === AppEnums.ipDHCP) {
                                    console.log(" → DHCP Enabled")
                                    wifi.startAutoRefresh()
                                } else if (currentValue === AppEnums.ipStatic) {
                                    console.log(" → Static IP Enabled")
                                    wifi.stopAutoRefresh()
                                }
                            }

                            // Set initial mode
                             Component.onCompleted: {
                                 currentIndex = wifi.isStaticIp ? 1 : 0
                             }

                             // Update instantly when WiFiManager changes mode
                             Connections {
                                 target: wifi
                                 function onIsStaticIpChanged() {
                                     ipModeCombo.currentIndex = wifi.isStaticIp ? 1 : 0
                                 }
                             }

                             // NEW: Listen for immediate programmatic change
                             Connections {
                                 target: wifi
                                 function onIpModeChanged() {
                                     ipModeCombo.currentIndex = wifi.isStaticIp ? 1 : 0
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
                                text: qsTr("IP Address")
                                textFieldText: wifi.ipAddress
                                textFieldPlaceholderText: "e.g. 192.168.29.50"
                                formField.onActiveFocusChanged: {
                                    if(formField.activeFocus) {
                                        formFlickable.ensureVisible(ipField)
                                    }
                                }
                            }

                            PrefsSeparator {}

                            PrefsTextFieldSubDelegate {
                                id: gatewayField
                                text: qsTr("Gateway")
                                textFieldText: wifi.gateway
                                textFieldPlaceholderText: "e.g. 192.168.29.1"
                                formField.onActiveFocusChanged: {
                                    if(formField.activeFocus) {
                                        formFlickable.ensureVisible(gatewayField)
                                    }
                                }
                            }

                            PrefsSeparator {}

                            PrefsTextFieldSubDelegate {
                                id: subnetField
                                text: qsTr("Subnet Mask")
                                textFieldText: wifi.subnetMask
                                textFieldPlaceholderText: "e.g. 255.255.255.0"
                                formField.onActiveFocusChanged: {
                                    if(formField.activeFocus) {
                                        formFlickable.ensureVisible(subnetField)
                                    }
                                }
                            }

                            PrefsSeparator {}

                            PrefsTextFieldSubDelegate {
                                id: dns1Field
                                text: qsTr("DNS 1")
                                textFieldText: wifi.dnsServers[0]
                                textFieldPlaceholderText: "e.g. 0.0.0.0"
                                formField.onActiveFocusChanged: {
                                    if(formField.activeFocus) {
                                        formFlickable.ensureVisible(dns1Field)
                                    }
                                }
                            }

                            PrefsSeparator {}

                            PrefsTextFieldSubDelegate {
                                id: dns2Field
                                text: qsTr("DNS 2")
                                textFieldText: wifi.dnsServers[1]
                                textFieldPlaceholderText: "e.g. 0.0.0.0"
                                formField.onActiveFocusChanged: {
                                    if(formField.activeFocus) {
                                        formFlickable.ensureVisible(dns2Field)
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        footer: Page {
            background: Item {
                implicitHeight: 72
            }

            header: Control {
                visible: errorMessage.length > 0
                topPadding: 10
                bottomPadding: 10
                padding: 20

                contentItem: Text {
                    id: errorText
                    font.pixelSize: 12
                    color: Colors.statusError
                    text: errorMessage
                    visible: !!text
                }
            }

            contentItem: RowLayout {
                spacing: 20

                Item {
                    Layout.fillWidth: true
                }

                PrefsButton {
                    text: control.rejectedButtonText
                    radius: height / 2
                    enabled: !busyIndicator.visible
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: {
                        control.rejected()
                        pageStack.pop()
                    }
                }

                PrefsBusyIndicator {
                    id: busyIndicator
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    visible: false
                    radius: 10

                    Connections {
                        target: wifi
                        function onProcessStarted() {
                            busyIndicator.visible  = true
                        }
                        function onProcessEnded() {
                            busyIndicator.visible  = false
                        }
                    }
                }

                PrefsButton {
                    text: control.acceptedButtonText
                    radius: height / 2
                    highlighted: true
                    visible: !busyIndicator.visible
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: {
                        if (ipModeCombo.currentValue === AppEnums.ipStatic) {

                            if (!validateIp(ipField.textFieldText)) {
                                errorMessage = qsTr("Invalid IP address")
                                return
                            }

                            if (!validateIp(subnetField.textFieldText)) {
                                errorMessage = qsTr("Invalid subnet mask")
                                return
                            }

                            if (!validateIp(gatewayField.textFieldText)) {
                                errorMessage = qsTr("Invalid gateway")
                                return
                            }

                            wifi.setStaticIp(
                                ipField.textFieldText.trim(),
                                subnetField.textFieldText.trim(),
                                gatewayField.textFieldText.trim(),
                                [ dns1Field.text.trim(), dns2Field.text.trim() ]
                            )

                        } else {
                            wifi.setDhcp()
                        }

                        Qt.callLater(function() {
                            control.accepted()
                            pageStack.pop()
                            wifi.refresh()
                            wifi.updateIpMode()
                        })
                    }

                    function validateIp(ip) {
                        let parts = ip.split(".")
                        if (parts.length !== 4)
                            return false

                        for (let p of parts) {
                            if (p === "" || isNaN(p))
                                return false

                            let n = Number(p)
                            if (n < 0 || n > 255)
                                return false
                        }

                        return true
                    }
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }
    }
}
