import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.3

import App.Styles 1.0
import AppSecurity 1.0
import App.Backend 1.0
import AddNetworkEnums 1.0

import "../pages"
import "../components"
import "../controls"

BaseDialog {
    id: control
    popupHeight: Math.min(parent.height * 0.7, layout.height + 125)
    pageTitle: qsTr("Add Network")
    saveButton.enabled:  addNetworkManager.saveEnabled && !addNetworkManager.busy

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

        console.log("HTTP Proxy:", httpProxyField.textFieldText)
        console.log("HTTPS Proxy:", httpsProxyField.textFieldText)
        console.log("SOCKS Proxy:", socksProxyField.textFieldText)
        console.log("Ignore Hosts:", ignoreHostsField.textFieldText)
        console.log("PAC URL:", pacUrlField.textFieldText)

        console.log("================================")

        addNetworkManager.addNetwork()
    }

    onRejected: {
        control.pageStack.pop()
    }

    // ============================================================================
    // FIRST FRAME — GENERAL WI-FI DETAILS
    // ============================================================================
    PrefsFrame {
        Layout.fillWidth: true

        WiFiAddNetworkManager {
            id: addNetworkManager

            onLogMessage: console.log("[WiFiAddNetwork]", msg)
            onAddNetworkResult: function(success, message) {
                console.log("Add result:", success, message)
                if (success) {
                    control.pageStack.pop()
                }
            }
        }

        ColumnLayout {
            anchors.fill: parent

            // ---------------------------
            // SSID
            // ---------------------------
            PrefsTextFieldDelegate {
                id: ssidField
                textFieldText: addNetworkManager.ssid
                onTextFieldTextChanged: addNetworkManager.ssid = textFieldText
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
                    { name: "None (Open)",              typeId: AppEnums.securityOpen,      nmValue: "none" },
                    { name: "WEP",                      typeId: AppEnums.securityWEP,       nmValue: "wep" },
                    { name: "WPA-PSK",                  typeId: AppEnums.securityWPA,       nmValue: "wpa-psk" },
                    { name: "WPA2-PSK",                 typeId: AppEnums.securityWPA2,      nmValue: "wpa-psk" },
                    { name: "WPA/WPA2 Mixed",           typeId: AppEnums.securityWPAMixed,  nmValue: "wpa-psk" },
                    { name: "WPA3-SAE",                 typeId: AppEnums.securityWPA3,      nmValue: "sae" },
                    { name: "WPA2/WPA3 Mixed",          typeId: AppEnums.securityWPA2WPA3,  nmValue: "wpa-psk sae" },
                    { name: "WPA2-Enterprise (EAP)",    typeId: AppEnums.securityEAP,       nmValue: "eap" },
                    { name: "WPA3-Enterprise (EAP3)",   typeId: AppEnums.securityEAP3,      nmValue: "eap" }
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
                    }

                    addNetworkManager.securityType = currentValue
                }

                onActivated: function(index) {
                    addNetworkManager.securityType = securityCombo.currentValue
                }
            }

            PrefsSeparator {
                visible: securityCombo.currentValue !== AppEnums.securityOpen
            }

            // ---------------------------
            // PASSWORD FIELD
            // ---------------------------
            PrefsTextFieldPasswordDelegate {
                id: passwordField
                visible: securityCombo.currentValue !== AppEnums.securityOpen
                textFieldText: addNetworkManager.password
                onTextFieldTextChanged: addNetworkManager.password = textFieldText
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
                    addNetworkManager.proxyMode = currentValue
                }

                onActivated: function(index) {
                    addNetworkManager.proxyMode = proxyCombo.currentValue
                }
            }

            ColumnLayout {
                visible: proxyCombo.currentValue === AppEnums.proxyManual
                Layout.fillWidth: true

                PrefsSeparator {}

                PrefsTextFieldSubDelegate {
                    id: httpProxyField
                    text: qsTr("HTTP Proxy (host:port)")
                    textFieldText: addNetworkManager.httpProxy
                    onTextFieldTextChanged: addNetworkManager.httpProxy = textFieldText
                    textFieldPlaceholderText: "e.g. 192.168.0.1:8080"
                }

                PrefsSeparator {}

                PrefsTextFieldSubDelegate {
                    id: httpsProxyField
                    text: qsTr("HTTPS Proxy (host:port)")
                    textFieldText: addNetworkManager.httpsProxy
                    onTextFieldTextChanged: addNetworkManager.httpsProxy = textFieldText
                    textFieldPlaceholderText: "e.g. 192.168.0.1:8443"
                }

                PrefsSeparator {}

                PrefsTextFieldSubDelegate {
                    id: socksProxyField
                    text: qsTr("SOCKS Proxy (host:port)")
                    textFieldText: addNetworkManager.socksProxy
                    onTextFieldTextChanged: addNetworkManager.socksProxy = textFieldText
                    textFieldPlaceholderText: "e.g. 192.168.0.1:1080"
                }

                PrefsSeparator {}

                PrefsTextFieldSubDelegate {
                    id: ignoreHostsField
                    text: qsTr("Ignore Hosts")
                    textFieldText: addNetworkManager.ignoreHosts
                    onTextFieldTextChanged: addNetworkManager.ignoreHosts = textFieldText
                    textFieldPlaceholderText: "e.g. localhost, 127.0.0.1"
                }
            }

            ColumnLayout {
                visible: proxyCombo.currentValue === AppEnums.proxyAuto
                Layout.fillWidth: true

                PrefsSeparator {}

                PrefsTextFieldSubDelegate {
                    id: pacUrlField
                    text: qsTr("PAC URL")
                    textFieldText: addNetworkManager.pacUrl
                    onTextFieldTextChanged: addNetworkManager.pacUrl = textFieldText
                    textFieldPlaceholderText: "http://example.com/proxy.pac"
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
                    { name: "DHCP",   typeId: AppEnums.ipDHCP,   nmValue: "auto" },
                    { name: "Static", typeId: AppEnums.ipStatic, nmValue: "manual" }
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
                    addNetworkManager.ipMode = currentValue
                }

                onActivated: function(index) {
                    addNetworkManager.ipMode = ipModeCombo.currentValue
                }
            }

            // ---------------------------
            // IP CONFIG (Visible when Static) // --- STATIC IP BLOCK ---
            // ---------------------------
            ColumnLayout {
                Layout.fillWidth: true
                visible: Number(ipModeCombo.currentValue) === AppEnums.ipStatic

                PrefsSeparator {}

                PrefsTextFieldSubDelegate {
                    id: ipField
                    text: qsTr("IP address")
                    textFieldText: addNetworkManager.ipAddress
                    onTextFieldTextChanged: addNetworkManager.ipAddress = textFieldText
                    textFieldPlaceholderText: "eg. 192.168.10.1"
                }

                PrefsSeparator {}

                PrefsTextFieldSubDelegate {
                    id: routerField
                    text: qsTr("Router")
                    textFieldText: addNetworkManager.gateway
                    onTextFieldTextChanged: addNetworkManager.gateway = textFieldText
                    textFieldPlaceholderText: "eg. 192.168.10.1"
                }

                PrefsSeparator {}

                PrefsTextFieldSubDelegate {
                    id: prefixField
                    text: qsTr("Prefix length")
                    textFieldText: addNetworkManager.prefixLength
                    onTextFieldTextChanged: addNetworkManager.prefixLength = textFieldText
                    textFieldPlaceholderText: "eg. 24"
                }

                PrefsSeparator {}

                PrefsTextFieldSubDelegate {
                    id: dns1Field
                    text: qsTr("DNS 1")
                    textFieldText: addNetworkManager.dns1
                    onTextFieldTextChanged: addNetworkManager.dns1 = textFieldText
                    textFieldPlaceholderText: "eg. 0.0.0.0"
                }

                PrefsSeparator {}

                PrefsTextFieldSubDelegate {
                    id: dns2Field
                    text: qsTr("DNS 2")
                    textFieldText: addNetworkManager.dns2
                    onTextFieldTextChanged: addNetworkManager.dns2 = textFieldText
                    textFieldPlaceholderText: "eg. 0.0.0.0"
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
                    { name: "Use device MAC address", typeId: AppEnums.macDevice, nmValue: "never" },
                    { name: "Use random MAC address", typeId: AppEnums.macRandom, nmValue: "default" }
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
                    addNetworkManager.macMode = currentValue
                }
                onActivated: function(index) {
                    addNetworkManager.macMode = macModeCombo.currentValue
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
                    { name: "Detect automatically", typeId: AppEnums.meteredAuto, nmValue: "auto" },
                    { name: "Metered",              typeId: AppEnums.meteredYes,  nmValue: "yes" },
                    { name: "Unmetered",            typeId: AppEnums.meteredNo,   nmValue: "no"  }
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

                    addNetworkManager.meteredMode = currentValue
                }

                onActivated: function(index) {
                    addNetworkManager.meteredMode = meteredCombo.currentValue
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
                    { name: "No",  typeId: AppEnums.hiddenNo ,  nmValue: "no" },
                    { name: "Yes", typeId: AppEnums.hiddenYes,  nmValue: "yes" }
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

                    addNetworkManager.hiddenMode = currentValue
                }
                onActivated: function(index) {
                    addNetworkManager.hiddenMode = hiddenCombo.currentValue
                }
            }
        }
    }
}
