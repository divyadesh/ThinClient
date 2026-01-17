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

    property string acceptedButtonText: qsTr("Save")
    property string rejectedButtonText: qsTr("Cancel")
    property string wifiSSID: ""
    property string errorMessage: ""

    background: BackgroundOverlay {}

    pageTitle: qsTr("Network Details")
    signal success()

    WiFiManager {
        id: wifi
        ssid: wifiSSID
        onLogMessage: {
            console.log("[WiFi]", message)
        }

        onProcessStarted: {
            console.log("[WiFi] Started configuration...")
        }

        onConnectionCompleted: function(success, message) {
            showAlert(message, success ? Type.Success : Type.Error)

            if (success) {
                wifiNetworkInfo.updateWifiNetworkInfo()
            }

            Qt.callLater(() => {
                if (success) {
                    pageStack.pop()
                }
            })
        }
    }

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
                            textFieldText: wifiNetworkInfo.status
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
                            textFieldText: wifiNetworkInfo.security
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
                            textFieldText: wifiNetworkInfo.mac
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
                            textFieldText: wifiNetworkInfo.ipAddress
                            textFieldPlaceholderText: "0.0.0.0"
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
                            currentIndex: wifiNetworkInfo.method === "DHCP" ? AppEnums.ipDHCP : AppEnums.ipStatic

                            Component.onCompleted: {
                                currentIndex = Qt.binding(function(){return wifiNetworkInfo.method === "DHCP" ? AppEnums.ipDHCP : AppEnums.ipStatic})
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
                                textFieldText: wifiNetworkInfo.ipAddress
                                textFieldPlaceholderText: "e.g. 0.0.0.0"
                                formField.onActiveFocusChanged: {
                                    if(formField.activeFocus) {
                                        formFlickable.ensureVisible(ipField)
                                    }
                                }
                            }

                            PrefsSeparator {
                                color: ipField.formField.cursorVisible ? Colors.accentPrimary : Colors.btnBg
                            }

                            PrefsTextFieldSubDelegate {
                                id: gatewayField
                                text: qsTr("Gateway")
                                textFieldText: wifiNetworkInfo.gateway
                                textFieldPlaceholderText: "e.g. 0.0.0.0"
                                formField.onActiveFocusChanged: {
                                    if(formField.activeFocus) {
                                        formFlickable.ensureVisible(gatewayField)
                                    }
                                }
                            }

                            PrefsSeparator {
                                color: gatewayField.formField.cursorVisible ? Colors.accentPrimary : Colors.btnBg
                            }

                            PrefsTextFieldSubDelegate {
                                id: subnetField
                                text: qsTr("Subnet Mask")
                                textFieldText: wifiNetworkInfo.subnet
                                textFieldPlaceholderText: "e.g. 0.0.0.0"
                                formField.onActiveFocusChanged: {
                                    if(formField.activeFocus) {
                                        formFlickable.ensureVisible(subnetField)
                                    }
                                }
                            }

                            PrefsSeparator {
                                color: subnetField.formField.cursorVisible ? Colors.accentPrimary : Colors.btnBg
                            }

                            PrefsTextFieldSubDelegate {
                                id: dns1Field
                                text: qsTr("DNS 1")
                                textFieldText: wifiNetworkInfo.dns1
                                textFieldPlaceholderText: "e.g. 0.0.0.0"
                                formField.onActiveFocusChanged: {
                                    if(formField.activeFocus) {
                                        formFlickable.ensureVisible(dns1Field)
                                    }
                                }
                            }

                            PrefsSeparator {
                                color: dns1Field.formField.cursorVisible ? Colors.accentPrimary : Colors.btnBg
                            }

                            PrefsTextFieldSubDelegate {
                                id: dns2Field
                                text: qsTr("DNS 2")
                                textFieldText: wifiNetworkInfo.dns2
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
                    visible: !wifi.isBusy
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: {
                        pageStack.pop()
                    }
                }

                PrefsBusyIndicator {
                    id: busyIndicator
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    running: wifi.isBusy
                    visible: wifi.isBusy
                    radius: 10
                }

                PrefsButton {
                    text: control.acceptedButtonText
                    radius: height / 2
                    highlighted: true
                    visible: !wifi.isBusy
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: {

                        // -------------------------
                        // 1) Validate static IP mode
                        // -------------------------
                        if (ipModeCombo.currentValue === AppEnums.ipStatic) {

                            function fail(msg) { errorMessage = msg; return }

                            const ip      = ipField.textFieldText.trim()
                            const mask    = subnetField.textFieldText.trim()
                            const gateway = gatewayField.textFieldText.trim()
                            // ----- DNS validation -----
                            const dns1 = dns1Field.textFieldText.trim()
                            const dns2 = dns2Field.textFieldText.trim()

                            // Validate IPs
                            if (!validateIp(ip))      return fail(qsTr("Invalid IP address"))
                            if (!validateIp(mask))    return fail(qsTr("Invalid subnet mask"))
                            if (!validateIp(gateway)) return fail(qsTr("Invalid gateway"))
                            if(!isValidDns(dns1))     return fail(qsTr("Invalid DNS 1"))
                            if(!isValidDns(dns2))     return fail(qsTr("Invalid DNS 1"))

                            let dnsList = []
                            if (dns1 !== "") dnsList.push(dns1)
                            if (dns2 !== "") dnsList.push(dns2)

                            // Now dnsList contains valid DNS servers
                            console.log("DNS list:", dnsList)

                            // ----- Apply Static IP -----
                            wifi.setStaticIpAsync(ip, mask, gateway, dnsList)

                        } else {
                            wifi.setDhcpAsync()
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }
    }

    function isValidDns(ip) {
        if (!ip || ip.trim() === "") return true      // allow empty DNS if optional

        const parts = ip.split(".")
        if (parts.length !== 4) return false

        for (let p of parts) {
            // block non-digit entries, leading spaces, characters
            if (!/^\d+$/.test(p)) return false

            const n = parseInt(p)
            if (n < 0 || n > 255) return false
        }

        // block invalid DNS edge values
        if (ip === "0.0.0.0") return false
        if (ip === "255.255.255.255") return false

        return true
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
