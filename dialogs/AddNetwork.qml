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

BaseDialog {
    id: control
    popupHeight: parent.height * 0.55
    pageTitle: qsTr("Add Network")

    // ---------------------------
    // FINAL RESULT HANDLER
    // ---------------------------
    onAccepted: {
        console.log("===== ADD NETWORK CONFIG =====")
        console.log("SSID:", ssidField.textFieldText)
        console.log("Security:", securityCombo.currentValue)
        console.log("Password:", passwordField.textFieldText)

        console.log("Proxy:", proxyCombo.currentValue)
        console.log("IP Mode:", ipModeCombo.currentValue)
        console.log("IP Address:", ipField.textFieldText)
        console.log("Gateway:", routerField.textFieldText)
        console.log("Prefix:", prefixField.textFieldText)
        console.log("DNS1:", dns1Field.textFieldText)
        console.log("DNS2:", dns2Field.textFieldText)

        console.log("MAC Mode:", macModeCombo.currentValue)
        console.log("Metered:", meteredCombo.currentValue)
        console.log("Hidden:", hiddenCombo.currentValue)
        console.log("================================")
    }

    // ============================================================================
    // FIRST FRAME — GENERAL WI-FI DETAILS
    // ============================================================================
    PrefsFrame {
        Layout.fillWidth: true

        ColumnLayout {
            anchors.fill: parent

            // ---------------------------
            // SSID
            // ---------------------------
            PrefsTextFieldDelegate {
                id: ssidField
                textFieldText: ""
                textFieldPlaceholderText: qsTr("Hotspot name (required)")
                Layout.fillWidth: true
            }

            PrefsSeparator {}

            // ---------------------------
            // SECURITY TYPE
            // ---------------------------
            PrefsComboBoxDelegate {
                id: securityCombo
                text: qsTr("Security")

                model: [
                    { name: "None (Open)",              typeId: AppEnums.securityOpen },
                    { name: "WEP",                      typeId: AppEnums.securityWEP },
                    { name: "WPA-PSK",                  typeId: AppEnums.securityWPA },
                    { name: "WPA2-PSK",                 typeId: AppEnums.securityWPA2 },
                    { name: "WPA/WPA2 Mixed",           typeId: AppEnums.securityWPAMixed },
                    { name: "WPA3-SAE",                 typeId: AppEnums.securityWPA3 },
                    { name: "WPA2/WPA3 Mixed",          typeId: AppEnums.securityWPA2WPA3 },
                    { name: "WPA2-Enterprise (EAP)",    typeId: AppEnums.securityEAP },
                    { name: "WPA3-Enterprise (EAP)",    typeId: AppEnums.securityEAP3 }
                ]

                textRole: "name"
                valueRole: "typeId"

                onCurrentValueChanged: {
                    console.log("[ADD NETWORK] Security changed:", currentValue)

                    if (currentValue === AppEnums.securityOpen) {
                        console.log(" → Open network (no password)")
                    } else if (currentValue === AppEnums.securityWEP) {
                        console.log(" → WEP")
                    } else if (currentValue === AppEnums.securityWPA) {
                        console.log(" → WPA-PSK")
                    } else if (currentValue === AppEnums.securityWPA2) {
                        console.log(" → WPA2-PSK")
                    } else if (currentValue === AppEnums.securityWPAMixed) {
                        console.log(" → WPA/WPA2 Mixed")
                    } else if (currentValue === AppEnums.securityWPA3) {
                        console.log(" → WPA3-SAE")
                    } else if (currentValue === AppEnums.securityWPA2WPA3) {
                        console.log(" → WPA2/WPA3 Mixed")
                    } else if (currentValue === AppEnums.securityEAP) {
                        console.log(" → WPA2-Enterprise (EAP)")
                    } else if (currentValue === AppEnums.securityEAP3) {
                        console.log(" → WPA3-Enterprise (EAP)")
                    }
                }
            }

            PrefsSeparator {}

            // ---------------------------
            // PASSWORD FIELD
            // ---------------------------
            PrefsTextFieldPasswordDelegate {
                id: passwordField
                textFieldText: ""
                textFieldPlaceholderText: qsTr("Password")
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
