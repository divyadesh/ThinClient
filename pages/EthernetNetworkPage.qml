import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import App.Styles 1.0
import App.Enums 1.0
import AddNetworkEnums 1.0

import "../controls"
import "../components"
import "../dialogs"

BasicPage {
    id: page
    padding: 20
    pageTitle: qsTr("Ethernet Details")

    // -------------------------------
    // INTERNAL HELPERS
    // -------------------------------

    // validation helper
    function validateStaticFields() {
        if (ipv4Field.textFieldText.trim().length === 0)
            return "IPv4 address cannot be empty"

        if (subnetMaskField.textFieldText.trim().length === 0)
            return "Subnet mask cannot be empty"

        if (gatewayField.textFieldText.trim().length === 0)
            return "Gateway cannot be empty"

        return "OK"
    }

    Connections {
        target: ethernetNetworkController

        // 1. IPv4 address changed
        function onIpAddressChanged() {
            console.log("[Ethernet] IP changed:", ethernetNetworkController.ipAddress)
            ipv4Field.textFieldText = ethernetNetworkController.ipAddress
        }

        // 2. Link speed changed
        function onLinkSpeedChanged() {
            console.log("[Ethernet] Speed:", ethernetNetworkController.linkSpeed + " Mbps")
        }

        // 3. Log messages from backend
        function onLogMessage(msg) {
            console.log("[Ethernet][LOG]", msg)
        }

        // 4. MAC address changed
        function onMacAddressChanged() {
            console.log("[Ethernet] MAC:", ethernetNetworkController.macAddress)
            macAddressField.textFieldText = ethernetNetworkController.macAddress
        }

        // 5. DNS changed
        function onDnsRecordsChanged() {
            console.log("[Ethernet] DNS changed:", ethernetNetworkController.dnsRecords)
            dns1Field.textFieldText = ethernetNetworkController.dnsRecords[0]
            dns2Field.textFieldText = ethernetNetworkController.dnsRecords[1]
        }

        // 6. Subnet mask changed
        function onSubnetMaskChanged() {
            console.log("[Ethernet] Subnet mask:", ethernetNetworkController.subnetMask)
            subnetMaskField.textFieldText = ethernetNetworkController.subnetMask
        }

        // 7. Gateway changed
        function onGatewayChanged() {
            console.log("[Ethernet] Gateway:", ethernetNetworkController.gateway)
            gatewayField.textFieldText = ethernetNetworkController.gateway
        }
    }

    header: PageHeader {
        pageTitle: page.pageTitle
        onBackPressed: {
            if (pageStack.depth === 1) {
                backToHome()
            }
            else {
                pageStack.pop()
            }
        }
    }

    // -------------------------------
    // MAIN PAGE CONTENT
    // -------------------------------
    contentItem: Flickable {
        width: parent.width
        contentHeight: layout.height
        clip: true

        ColumnLayout {
            id: layout
            width: page.width - 40
            spacing: 20

            // =========================================================
            //  IP CONFIGURATION FRAME
            // =========================================================
            Control {
                Layout.fillWidth: true
                padding: 20

                contentItem: ColumnLayout {
                    spacing: 8

                    PrefsFrame {
                        Layout.fillWidth: true

                        background: Rectangle {
                            radius: 8
                            color: "#2A2A2A"
                        }

                        ColumnLayout {
                            anchors.fill: parent

                            //------------------------------------
                            // IP MODE SELECTOR
                            //------------------------------------
                            PrefsComboBoxDelegate {
                                id: ipModeCombo
                                text: qsTr("IP settings")

                                model: [
                                    { name: "DHCP",  typeId: AppEnums.ipDHCP },
                                    { name: "Static", typeId: AppEnums.ipStatic }
                                ]

                                textRole: "name"
                                valueRole: "typeId"

                                // default: based on saved state
                                currentIndex: persistData
                                              ? (persistData.getData("Ethernet") === "DHCP" ? AppEnums.ipDHCP : AppEnums.ipStatic)
                                              : AppEnums.ipDHCP

                                //------------------------------------
                                // DHCP <-> STATIC MODE SWITCH HANDLER
                                //------------------------------------
                                onCurrentValueChanged: {
                                    console.log("[IP Mode Changed] New mode =", currentValue)
                                }

                                onActivated: function(index) {
                                    if (currentValue === AppEnums.ipDHCP) {
                                        updateIPMode(currentValue);
                                        ethernetNetworkController.enableDhcp()
                                    }
                                }
                            }

                            PrefsSeparator {}

                            //------------------------------------
                            // STATIC IP FIELDS
                            //------------------------------------
                            PrefsTextFieldSubDelegate {
                                id: ipv4Field
                                text: qsTr("IPv4 address")
                                textFieldText: ethernetNetworkController.ipAddress
                                textFieldPlaceholderText: qsTr("192.168.1.100")
                                readOnly: ipModeCombo.currentValue === AppEnums.ipDHCP
                            }

                            PrefsSeparator {}

                            PrefsTextFieldSubDelegate {
                                id: subnetMaskField
                                text: qsTr("Subnet mask")
                                textFieldText: ethernetNetworkController.subnetMask
                                textFieldPlaceholderText: qsTr("255.255.255.0")
                                readOnly: ipModeCombo.currentValue === AppEnums.ipDHCP
                            }

                            PrefsSeparator {}

                            PrefsTextFieldSubDelegate {
                                id: gatewayField
                                text: qsTr("Gateway")
                                textFieldText: ethernetNetworkController.gateway
                                textFieldPlaceholderText: qsTr("192.168.1.1")
                                readOnly: ipModeCombo.currentValue === AppEnums.ipDHCP
                            }

                            PrefsSeparator {}

                            PrefsTextFieldSubDelegate {
                                id: dns1Field
                                text: qsTr("DNS 1")
                                textFieldText: ethernetNetworkController.dnsRecords[0]
                                textFieldPlaceholderText: qsTr("8.8.8.8")
                                readOnly: ipModeCombo.currentValue === AppEnums.ipDHCP
                            }

                            PrefsSeparator {}

                            PrefsTextFieldSubDelegate {
                                id: dns2Field
                                text: qsTr("DNS 2")
                                textFieldText: ethernetNetworkController.dnsRecords[1]
                                textFieldPlaceholderText: qsTr("8.8.4.4")
                                readOnly: ipModeCombo.currentValue === AppEnums.ipDHCP
                            }

                            PrefsSeparator {}

                            PrefsTextFieldSubDelegate {
                                id: macAddressField
                                text: qsTr("MAC address")
                                textFieldText: ethernetNetworkController.macAddress
                                textFieldPlaceholderText: qsTr("AA:BB:CC:DD:EE:FF")
                                readOnly: true
                            }
                        }
                    }

                    // =========================================================
                    // APPLY BUTTON (STATIC MODE ONLY)
                    // =========================================================
                    ItemDelegate {
                        id: saveDelegate
                        Layout.fillWidth: true
                        visible: ipModeCombo.currentValue === AppEnums.ipStatic
                        background: null

                        indicator: PrefsButton {
                            x: saveDelegate.width - width - saveDelegate.rightPadding
                            y: saveDelegate.topPadding + (saveDelegate.availableHeight - height) / 2

                            text: qsTr("Apply")
                            radius: height / 2
                            highlighted: enabled ? true : false

                            enabled: ipv4Field.textFieldText.trim().length > 0 &&
                                     subnetMaskField.textFieldText.trim().length > 0 &&
                                     gatewayField.textFieldText.trim().length > 0

                            onClicked: {
                                let msg = page.validateStaticFields()
                                if (msg !== "OK") {
                                    console.log("Validation FAILED:", msg)
                                    return
                                }

                                ethernetNetworkController.applyStaticConfig(
                                            ipv4Field.textFieldText.trim(),
                                            parseInt(ethernetNetworkController.maskToCidr(subnetMaskField.textFieldText.trim())),
                                            gatewayField.textFieldText.trim(),
                                            dns1Field.textFieldText.trim(),
                                            dns2Field.textFieldText.trim()
                                            )

                                if (persistData){
                                    persistData.saveData("Ethernet", "Static")
                                }

                                console.log("Static IP configuration applied successfully")
                            }
                        }
                    }

                    Item { Layout.fillWidth: true; Layout.preferredHeight: 20 }
                }
            }
        }
    }

    function updateIPMode(typeId) {
        if (typeId === AppEnums.ipDHCP) {
            persistData.saveData("Ethernet", "DHCP")
            console.log("→ Switched to DHCP — restoring original values")
        }
        else {
            persistData.saveData("Ethernet", "Static")
            console.log("→ Switched to Manual — ready for manual input")
        }
    }
}
