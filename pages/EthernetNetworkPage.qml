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

    property string uiMethod: ""
    property string uiIpAddress: ""
    property string uiSubnet: ""
    property string uiGateway: ""
    property string uiDns1: ""
    property string uiDns2: ""

    function refreshFromBackend() {
        uiMethod = ethernetNetworkInfo.method || ""
        uiIpAddress = ethernetNetworkInfo.ipAddress || ""
        uiSubnet    = ethernetNetworkInfo.subnet    || ""
        uiGateway   = ethernetNetworkInfo.gateway   || ""
        uiDns1      = ethernetNetworkInfo.dns1      || ""
        uiDns2      = ethernetNetworkInfo.dns2      || ""
    }

    // -------------------------------
    // INTERNAL HELPERS
    // -------------------------------

    function validateStaticFields() {
        const ip      = ipv4Field.textFieldText.trim();
        const mask    = subnetMaskField.textFieldText.trim();
        const gateway = gatewayField.textFieldText.trim();

        function isValidIPv4(addr) {
            const re =
                /^(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}$/;
            return re.test(addr);
        }


        function isValidSubnetMask(mask) {
            if (!isValidIPv4(mask))
                return false;

            const parts = mask.split(".").map(Number);

            let seenZero = false;

            for (let i = 0; i < 4; i++) {
                let bytes = parts[i];

                for (let bit = 7; bit >= 0; bit--) {
                    const isOne = (bytes & (1 << bit)) !== 0;

                    if (!isOne) {
                        seenZero = true;
                    } else if (seenZero) {
                        // Found a 1 after a 0 → invalid mask
                        return false;
                    }
                }
            }
            return true;
        }

        function ipv4ToInt(addr) {
            return addr.split('.').reduce(function (res, octet) {
                return (res << 8) + parseInt(octet);
            }, 0) >>> 0;
        }

        /* ---- Required fields ---- */
        if (ip === "")
            return qsTr("IPv4 address cannot be empty");

        if (mask === "")
            return qsTr("Subnet mask cannot be empty");

        if (gateway === "")
            return qsTr("Gateway cannot be empty");

        /* ---- Format validation ---- */
        if (!isValidIPv4(ip))
            return qsTr("Invalid IPv4 address");

        if (!isValidSubnetMask(mask))
            return qsTr("Invalid subnet mask");

        if (!isValidIPv4(gateway))
            return qsTr("Invalid gateway address");

        /* ---- Network consistency ---- */
        const ipInt   = ipv4ToInt(ip);
        const maskInt = ipv4ToInt(mask);
        const gwInt   = ipv4ToInt(gateway);

        if ((ipInt & maskInt) !== (gwInt & maskInt))
            return qsTr("IP address and gateway must be in the same subnet");

        return "OK";
    }

    Component.onCompleted: refreshFromBackend()

    Connections {
        target: ethernetNetworkInfo
        function onChanged() {
            refreshFromBackend()
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

                                currentIndex: page.uiMethod === "DHCP" ? AppEnums.ipDHCP : AppEnums.ipStatic
                                onActivated: function(index) {
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
                                textFieldText: ethernetMonitor.connected ? page.uiIpAddress : ""
                                textFieldPlaceholderText: "0.0.0.0"
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
                                textFieldText: ethernetMonitor.connected ? page.uiSubnet : ""
                                textFieldPlaceholderText: "0.0.0.0"
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
                                textFieldText: ethernetMonitor.connected ? page.uiGateway : ""
                                textFieldPlaceholderText: "0.0.0.0"
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
                                textFieldText: ethernetMonitor.connected ? page.uiDns1 : ""
                                textFieldPlaceholderText: "0.0.0.0"
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
                                textFieldText: ethernetMonitor.connected ? page.uiDns2 : ""
                                textFieldPlaceholderText: "0.0.0.0"
                                readOnly: ipModeCombo.currentValue === AppEnums.ipDHCP
                                enabled: !readOnly && !ethernetNetworkController.isBusy
                                formField.onActiveFocusChanged: {
                                    if(formField.activeFocus) {
                                        formFlickable.ensureVisible(dns2Field)
                                    }
                                }
                            }

                            PrefsSeparator {
                                visible: ethernetMonitor.connected
                                color: dns2Field.formField.cursorVisible ? Colors.accentPrimary : Colors.btnBg
                            }

                            PrefsTextFieldSubDelegate {
                                id: macAddressField
                                visible: ethernetMonitor.connected
                                text: qsTr("MAC address")
                                textFieldText: ethernetNetworkInfo.mac
                                textFieldPlaceholderText: "00:00:00:00:00:00"
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
                                        //ethernetNetworkController.enableDhcpAsync()
                                        ethernetNetworkInfo.switchToDhcpAsync()
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
            showAlert(msg, Type.Error)
            return
        }

        ethernetNetworkInfo.switchToStaticAsync(
            ipv4Field.textFieldText.trim(),
            subnetMaskField.textFieldText.trim(),
            gatewayField.textFieldText.trim(),
            dns1Field.textFieldText.trim(),
            dns2Field.textFieldText.trim()
        )
    }

    Connections {
        target: ethernetNetworkController

        function onOperationStarted() {
            showAlert(qsTr("Operation started"), Type.Success)
        }

        function onOperationFinished(success, message) {
            if(success) {
                ethernetNetworkInfo.updateEthernetNetworkInfo()
                persistData.ethernet = ipModeCombo.currentValue === AppEnums.ipDHCP ? "DHCP" : "Static"
                console.log("Static IP configuration applied successfully")
            }else {
                console.log("Failed to apply static IP configuration");
            }
            showAlert(message, success ? Type.Success : Type.Error)
        }
    }

    Connections {
        target: ethernetNetworkInfo
        function onOperationStarted() {
            showAlert(qsTr("Operation started"), Type.Success)
        }

        function onOperationFinished(success, message) {
            if(success){
                ethernetNetworkInfo.updateEthernetNetworkInfo()
            }

            showAlert(message, success ? Type.Success : Type.Error)
        }
    }
}
