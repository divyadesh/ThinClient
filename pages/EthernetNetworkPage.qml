import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import App.Styles 1.0
import App.Enums 1.0
import AddNetworkEnums 1.0
import App.Backend 1.0

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

    function validateStaticFields() {
        const ip      = ipv4Field.textFieldText.trim();
        const mask    = subnetMaskField.textFieldText.trim();
        const gateway = gatewayField.textFieldText.trim();

        if (ip === "")
            return qsTr("IPv4 address cannot be empty");

        if (mask === "")
            return qsTr("Subnet mask cannot be empty");

        if (gateway === "")
            return qsTr("Gateway cannot be empty");

        return "OK";
    }

    Connections {
        target: ethernetNetworkController

        // IPv4 address updated
        function onIpAddressChanged() {
            console.log("[Ethernet] IPv4 Address:", ethernetNetworkController.ipAddress)
            ipv4Field.textFieldText = ethernetNetworkController.ipAddress
        }

        // Link speed updated
        function onLinkSpeedChanged() {
            console.log(`[Ethernet] Link Speed: ${ethernetNetworkController.linkSpeed} Mbps`)
        }

        // Backend log messages
        function onLogMessage(msg) {
            console.log("[Ethernet][LOG]", msg)
        }

        // MAC address updated
        function onMacAddressChanged() {
            console.log("[Ethernet] MAC Address:", ethernetNetworkController.macAddress)
            macAddressField.textFieldText = ethernetNetworkController.macAddress
        }

        // DNS records updated
        function onDnsRecordsChanged() {
            console.log("[Ethernet] DNS Records:", ethernetNetworkController.dnsRecords)
            dns1Field.textFieldText = primaryDnsValue()
            dns2Field.textFieldText = secondaryDnsValue()
        }

        // Subnet mask updated
        function onSubnetMaskChanged() {
            console.log("[Ethernet] Subnet Mask:", ethernetNetworkController.subnetMask)
            subnetMaskField.textFieldText = ethernetNetworkController.subnetMask
        }

        // Gateway updated
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
    contentItem: PrefsFlickable {
        id: formFlickable
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
                                enabled: !ethernetNetworkController.isBusy

                                model: [
                                    { name: "DHCP",  typeId: AppEnums.ipDHCP },
                                    { name: "Static", typeId: AppEnums.ipStatic }
                                ]

                                textRole: "name"
                                valueRole: "typeId"

                                currentIndex: persistData.ethernet === "DHCP" ? AppEnums.ipDHCP : AppEnums.ipStatic
                                onActivated: function(index) {
                                    console.log("[Ethernet] Mode selection activated. Index:", index)

                                    if (currentValue === AppEnums.ipDHCP) {
                                        console.log("[Ethernet] Switching to DHCP mode…")
                                    } else {
                                        console.log("[Ethernet] Static mode selected (no action here).")
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
                                enabled: !readOnly && !ethernetNetworkController.isBusy
                                formField.onActiveFocusChanged: {
                                    if(formField.activeFocus) {
                                        formFlickable.ensureVisible(ipv4Field)
                                    }
                                }
                            }

                            PrefsSeparator {
                                color: ipv4Field.formField.cursorVisible ? Colors.accentPrimary : Colors.btnBg
                            }

                            PrefsTextFieldSubDelegate {
                                id: subnetMaskField
                                text: qsTr("Subnet mask")
                                textFieldText: ethernetNetworkController.subnetMask
                                textFieldPlaceholderText: qsTr("255.255.255.0")
                                readOnly: ipModeCombo.currentValue === AppEnums.ipDHCP
                                enabled: !readOnly && !ethernetNetworkController.isBusy
                                formField.onActiveFocusChanged: {
                                    if(formField.activeFocus) {
                                        formFlickable.ensureVisible(subnetMaskField)
                                    }
                                }
                            }

                            PrefsSeparator {
                                color: subnetMaskField.formField.cursorVisible ? Colors.accentPrimary : Colors.btnBg
                            }

                            PrefsTextFieldSubDelegate {
                                id: gatewayField
                                text: qsTr("Gateway")
                                textFieldText: ethernetNetworkController.gateway
                                textFieldPlaceholderText: qsTr("192.168.1.1")
                                readOnly: ipModeCombo.currentValue === AppEnums.ipDHCP
                                enabled: !readOnly && !ethernetNetworkController.isBusy
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
                                id: dns1Field
                                text: qsTr("DNS 1")
                                textFieldText: primaryDnsValue()
                                textFieldPlaceholderText: qsTr("8.8.8.8")
                                readOnly: ipModeCombo.currentValue === AppEnums.ipDHCP
                                enabled: !readOnly && !ethernetNetworkController.isBusy
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
                                textFieldText: secondaryDnsValue()
                                textFieldPlaceholderText: qsTr("8.8.4.4")
                                readOnly: ipModeCombo.currentValue === AppEnums.ipDHCP
                                enabled: !readOnly && !ethernetNetworkController.isBusy
                                formField.onActiveFocusChanged: {
                                    if(formField.activeFocus) {
                                        formFlickable.ensureVisible(dns2Field)
                                    }
                                }
                            }

                            PrefsSeparator {
                                color: dns2Field.formField.cursorVisible ? Colors.accentPrimary : Colors.btnBg
                            }

                            PrefsTextFieldSubDelegate {
                                id: macAddressField
                                text: qsTr("MAC address")
                                textFieldText: ethernetNetworkController.macAddress
                                textFieldPlaceholderText: qsTr("AA:BB:CC:DD:EE:FF")
                                readOnly: true
                                enabled: !readOnly
                                formField.onActiveFocusChanged: {
                                    if(formField.activeFocus) {
                                        formFlickable.ensureVisible(macAddressField)
                                    }
                                }
                            }
                        }
                    }

                    // =========================================================
                    // APPLY BUTTON (STATIC MODE ONLY)
                    // =========================================================
                    ItemDelegate {
                        id: saveDelegate
                        Layout.fillWidth: true
                        background: null

                        indicator: RowLayout {
                            x: saveDelegate.width - width - saveDelegate.rightPadding
                            y: saveDelegate.topPadding + (saveDelegate.availableHeight - height) / 2
                            spacing: 30

                            PrefsBusyIndicator {
                                id: busyIndicator
                                radius: 16
                                running: ethernetNetworkController.isBusy
                                visible: ethernetNetworkController.isBusy
                            }

                            PrefsButton {
                                visible: !ethernetNetworkController.isBusy
                                text: qsTr("Apply")
                                radius: height / 2
                                highlighted: true
                                enabled: true
                                focus: visible
                                focusPolicy: visible ? Qt.StrongFocus : Qt.NoFocus

                                onClicked: {
                                    if(ipModeCombo.currentValue === AppEnums.ipStatic) {
                                        applySettings()
                                    }else {
                                        ethernetNetworkController.enableDhcpAsync()
                                    }
                                }
                            }
                        }
                    }

                    Item { Layout.fillWidth: true; Layout.preferredHeight: 20 }
                }
            }
        }
    }

    function primaryDnsValue() {
        if (!ethernetNetworkController)
            return "";

        const records = ethernetNetworkController.dnsRecords;

        if (records && records.length > 0 && records[0] !== undefined)
            return records[0];

        return ethernetNetworkController.gateway || "";
    }

    function secondaryDnsValue() {
        if (!ethernetNetworkController)
            return "";

        const records = ethernetNetworkController.dnsRecords;

        if (records && records.length > 1 && records[1] !== undefined)
            return records[1];

        return ethernetNetworkController.gateway || "";
    }


    function applySettings() {
        let msg = page.validateStaticFields()
        if (msg !== "OK") {
            console.log("Validation FAILED:", msg)
            showAlert(msg, Type.Error)
            return
        }

        // Collect user input
        const ipAddress      = ipv4Field.textFieldText.trim();
        const cidrMask       = ethernetNetworkController.maskToCidr(
                                   subnetMaskField.textFieldText.trim());
        const gatewayAddress = gatewayField.textFieldText.trim();
        const dnsPrimary     = dns1Field.textFieldText.trim();
        const dnsSecondary   = dns2Field.textFieldText.trim();

        console.log(`
        ===== Ethernet Static Config Input =====
         IP Address      : ${ipAddress}
         CIDR Mask       : ${cidrMask}
         Gateway         : ${gatewayAddress}
         DNS Primary     : ${dnsPrimary}
         DNS Secondary   : ${dnsSecondary}
        ========================================
        `);

        // Apply configuration
        ethernetNetworkController.applyStaticConfigAsync(
            ipAddress,
            parseInt(cidrMask),
            gatewayAddress,
            dnsPrimary,
            dnsSecondary
        );
    }

    Connections {
        target: ethernetNetworkController

        function onOperationStarted() {
            showAlert(qsTr("Operation started"), Type.Success)
        }

        function onOperationFinished(success, message) {
            if(success) {
                persistData.ethernet = ipModeCombo.currentValue === AppEnums.ipDHCP ? "DHCP" : "Static"
                console.log("Static IP configuration applied successfully")
            }else {
                console.log("Failed to apply static IP configuration");
            }
            showAlert(message, success ? Type.Success : Type.Error)
        }
    }
}
