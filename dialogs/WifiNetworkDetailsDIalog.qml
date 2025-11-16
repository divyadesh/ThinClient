import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.3

import App.Styles 1.0
import AppSecurity 1.0
import AddNetworkEnums 1.0

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

    signal accepted()
    signal rejected()

    background: BackgroundOverlay {}

    pageTitle: qsTr("Network Details")

    // ---------------------------
    // FINAL RESULT HANDLER
    // ---------------------------
    // expose backend
    readonly property var backend: WiFiManager

    onAccepted: {
        // run validation via backend first
        var res = backend.validateInputs();
        if (!res.valid) {
            // show error to user
            // validationDialog.text = res.message
            // validationDialog.open()
            return; // abort accept
        }

        // call backend apply
        backend.apply()
    }

    // notification from backend
    Connections {
        target: backend
        function onApplyResult() {
            if (ok) {
                console.log("[AddNetwork] apply ok:", message)
                // infoDialog.text = qsTr("Network applied successfully")
                // infoDialog.open()
                pageStack.pop()
            } else {
                console.error("[AddNetwork] apply failed:", message)
                // validationDialog.text = message
                // validationDialog.open()
            }
        }

        function onLogMessage() {
            console.debug("[WiFiManager] " + line)
        }
    }

    // ... UI fields like ssidField, securityCombo etc. unchanged
    // Bind fields to backend automatically:
    Component.onCompleted: {
        ssidField.textFieldText = backend.ssid
        passwordField.textFieldText = backend.password ? "••••••••" : ""
        securityField.textFieldText = backend.securityName   // <-- FIX
        statusField.textFieldText = backend.status
        signalField.textFieldText = backend.signalStrength
        linkSpeedField.textFieldText = backend.linkSpeed
        macField.textFieldText = backend.macAddress
        ipv4Field.textFieldText = backend.ipAddress

        ipModeCombo.currentValue = backend.ipMode
        ipField.textFieldText = backend.ipAddress
        routerField.textFieldText = backend.gateway
        prefixField.textFieldText = backend.prefix ? backend.prefix : "24"
        dns1Field.textFieldText = backend.dns1
        dns2Field.textFieldText = backend.dns2
        macModeCombo.currentValue = backend.macMode
        meteredCombo.currentValue = backend.metered
        hiddenCombo.currentValue = backend.hidden
    }

    // ===================================================================
    // SYNC: QML UI → Backend (WiFiManager)
    // ===================================================================

    // PROXY
    Connections {
        target: proxyCombo
        function onCurrentValueChanged() { backend.proxyMode = Number(proxyCombo.currentValue) }
    }

    // IP MODE
    Connections {
        target: ipModeCombo
        function onCurrentValueChanged() { backend.ipMode = Number(ipModeCombo.currentValue) }
    }

    // STATIC IP FIELDS
    Connections { target: ipField; function onTextFieldTextChanged() { backend.ipAddress = ipField.textFieldText } }
    Connections { target: routerField; function onTextFieldTextChanged() { backend.gateway = routerField.textFieldText } }
    Connections { target: prefixField; function onTextFieldTextChanged() { backend.prefix = Number(prefixField.textFieldText) } }
    Connections { target: dns1Field; function onTextFieldTextChanged() { backend.dns1 = dns1Field.textFieldText } }
    Connections { target: dns2Field; function onTextFieldTextChanged() { backend.dns2 = dns2Field.textFieldText } }

    // MAC MODE
    Connections { target: macModeCombo; function onCurrentValueChanged() { backend.macMode = Number(macModeCombo.currentValue) } }

    // METERED
    Connections { target: meteredCombo; function onCurrentValueChanged() { backend.metered = Number(meteredCombo.currentValue) } }

    // HIDDEN
    Connections { target: hiddenCombo; function onCurrentValueChanged() { backend.hidden = Number(hiddenCombo.currentValue) } }

    // Simple user dialogs
    // MessageDialog {
    //     id: validationDialog
    //     title: qsTr("Validation error")
    //     text: ""
    //     icon: StandardIcon.Critical
    // }
    // MessageDialog {
    //     id: infoDialog
    //     title: qsTr("Info")
    //     text: ""
    //     icon: StandardIcon.Information
    // }


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
                            textFieldText: ""
                            textFieldPlaceholderText: qsTr("Thin client ...")
                            readOnly: true
                        }

                        PrefsSeparator {}

                        /* ---------------------------
                         * SIGNAL PASSWORD
                         * --------------------------- */
                        PrefsTextFieldSubDelegate {
                            id: passwordField
                            text: qsTr("Password")
                            textFieldText: ""
                            textFieldPlaceholderText: qsTr("*******")
                            readOnly: true
                        }

                        PrefsSeparator {}

                        /* ---------------------------
                         * CONNECTION STATUS
                         * --------------------------- */
                        PrefsTextFieldSubDelegate {
                            id: statusField
                            text: qsTr("Status")
                            textFieldText: ""
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
                            textFieldText: ""
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
                            textFieldText: ""
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
                            textFieldText: ""
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
                            textFieldText: ""
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
                            textFieldText: ""
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
                                textFieldText: ""
                                textFieldPlaceholderText: "192.168.10.1"
                            }

                            PrefsSeparator {}

                            PrefsTextFieldSubDelegate {
                                id: routerField
                                text: qsTr("Router")
                                textFieldText: ""
                                textFieldPlaceholderText: "192.168.10.1"
                            }

                            PrefsSeparator {}

                            PrefsTextFieldSubDelegate {
                                id: prefixField
                                text: qsTr("Prefix length")
                                textFieldText: ""
                                textFieldPlaceholderText: "24"
                            }

                            PrefsSeparator {}

                            PrefsTextFieldSubDelegate {
                                id: dns1Field
                                text: qsTr("DNS 1")
                                textFieldText: ""
                                textFieldPlaceholderText: "0.0.0.0"
                            }

                            PrefsSeparator {}

                            PrefsTextFieldSubDelegate {
                                id: dns2Field
                                text: qsTr("DNS 2")
                                textFieldText: ""
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
