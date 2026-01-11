import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import QtQuick.Controls.impl 2.12

import App.Styles 1.0
import App.Backend 1.0

import "../components"
import "../controls"
import "../dialogs"
import "../"

BasicPage {
    id: page
    padding: 20
    StackView.visible: true
    property string connectionId: ""
    property string errorMessage: ""
    property bool hasError: errorMessage.length > 0

    header: PageHeader {
        pageTitle: page.pageTitle
        onBackPressed: {
            if(pageStack.depth === 1) {
                backToHome()
                return
            }
            pageStack.pop()
        }
    }

    ButtonGroup { id: radioGroup }

    component PrefsLink: PrefsLabel {
        id: link
        signal clicked()

        color: linkArea.pressed || linkArea.containsMouse ? Colors.accentHover : Colors.accentPrimary
        font.underline: true
        horizontalAlignment: Label.AlignHCenter
        verticalAlignment: Label.AlignVCenter
        scale: linkArea.pressed ? 0.95 : 1.0

        padding: 4
        leftPadding: 5
        rightPadding: 5

        MouseArea {
            id: linkArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: { link.clicked() }
        }
    }

    contentItem: PrefsFlickable {
        id: formFlickable
        contentHeight: layout.height
        contentWidth: layout.width

        ColumnLayout {
            id: layout
            width: page.width - 40
            spacing: 10
            clip: true

            Control {
                Layout.fillWidth: true
                Layout.preferredHeight: 400
                Layout.maximumHeight: page.height /3
                padding: 20
                contentItem: RDConnectionsView {
                    onPopulateConnection: function(connectionId) {
                        let session = sessionModel.getSessionById(connectionId);

                        if (!session) {
                            console.warn("❌ No session found for ID:", connectionId);
                            return;
                        }

                        console.log("✅ Session Loaded:");
                        console.log("ID:", session.connectionId);
                        console.log("Name:", session.connectionName);
                        console.log("Server:", session.serverIp);
                        console.log("User:", session.userName);
                        console.log("Device:", session.deviceName);
                        console.log("AutoConnect:", session.autoConnect);

                        populateConnectionFields(session);
                    }

                    function populateConnectionFields(session) {

                        if (!session) {
                            console.warn("❌ populateConnectionFields(): session is null");
                            return;
                        }

                        // -------------------------
                        // Core text fields
                        // -------------------------
                        page.connectionId      = session.connectionId       || ""
                        connectionField.text   = session.connectionName     || "";
                        serverIpField.text     = session.serverIp           || "";
                        deviceNameField.text   = session.deviceName         || "";
                        usernameField.text     = session.userName           || "";
                        passwordField.text     = session.password           || "";

                        // -------------------------
                        // Performance radio buttons
                        // "Best" / "Auto"
                        // -------------------------

                        bestButton.checked = (session.performance === "Best");
                        customButton.checked = (session.performance === "Auto");

                        // -------------------------
                        // Feature toggles
                        // -------------------------
                        audioButton.checked        = session.enableAudio;
                        microphoneButton.checked   = session.enableMicrophone;
                        driveButton.checked        = session.redirectDrive;
                        usbDeviceButton.checked    = session.redirectUsbDevice;
                        securityButton.checked     = session.security;

                        // -------------------------
                        // Gateway
                        // -------------------------
                        rdGateWay.checked = session.gateway;

                        if (session.gateway) {
                            gatewayIp.text        = session.gatewayIp        || "";
                            gatewayUserName.text  = session.gatewayUserName  || "";
                            gatewayPassword.text  = session.gatewayPassword  || "";
                        }

                        graphicsDialog.useAVC = session.useAVC
                        graphicsDialog.enableAnimation = session.enableAnimation
                        graphicsDialog.enableGDI = session.enableGDI

                        // -------------------------
                        // Auto-connect (if needed)
                        // -------------------------
                        if (typeof autoConnectSwitch !== "undefined")
                            autoConnectSwitch.checked = session.autoConnect;

                        console.log(`✅ Fields populated for ${session.connectionName} → ${session.serverIp}`);
                    }

                    function isTrue(v) {
                        return v === true || v === "true" || v === 1;
                    }
                }
            }

            PrefsHeader {
                Layout.fillWidth: true
                text: qsTr("Add RD Server")
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 40

                Control {
                    Layout.fillWidth: true
                    contentItem: ColumnLayout {
                        spacing: 10

                        PrefsItemDelegate {
                            id: connection
                            Layout.fillWidth: true
                            text: qsTr("Connection Name")

                            indicator: PrefsTextField {
                                id: connectionField
                                x: connection.width - width - connection.rightPadding
                                y: connection.topPadding + (connection.availableHeight - height) / 2

                                placeholderText : qsTr("Enter %1").arg(connection.text)
                                onTextChanged: page.errorMessage = ""
                                onActiveFocusChanged: {
                                    if (activeFocus) {
                                        // Ensures the field is visible above the keyboard
                                        formFlickable.ensureVisible(connectionField)
                                    }
                                }
                            }
                        }
                        PrefsItemDelegate {
                            id: serverIp
                            Layout.fillWidth: true
                            text: qsTr("RD Server IP")

                            indicator: PrefsTextField {
                                id: serverIpField
                                x: serverIp.width - width - serverIp.rightPadding
                                y: serverIp.topPadding + (serverIp.availableHeight - height) / 2

                                validator: RegularExpressionValidator {
                                    // Accepts:
                                    // - IPv4: 192.168.10.34
                                    // - IPv4 + port: 192.168.10.34:4444
                                    // - IPv6: ::1 or fe80::1
                                    // - IPv6 + port: [::1]:4444 or fe80::1:4444
                                    // ❌ Rejects plain text like "abcd"
                                    regularExpression: /^(((25[0-5]|2[0-4]\d|1\d{2}|[1-9]?\d)\.){3}(25[0-5]|2[0-4]\d|1\d{2}|[1-9]?\d)|(\[?[A-Fa-f0-9]*:[A-Fa-f0-9:]+\]?))(:\d*)?$/
                                }

                                onTextChanged: page.errorMessage = ""
                                placeholderText : qsTr("Enter %1").arg(serverIp.text)
                                onActiveFocusChanged: {
                                    if (activeFocus) {
                                        // Ensures the field is visible above the keyboard
                                        formFlickable.ensureVisible(serverIpField)
                                    }
                                }
                            }
                        }

                        PrefsItemDelegate {
                            id: deviceName
                            Layout.fillWidth: true
                            text: qsTr("Device Name")

                            indicator: PrefsTextField {
                                id: deviceNameField
                                x: deviceName.width - width - deviceName.rightPadding
                                y: deviceName.topPadding + (deviceName.availableHeight - height) / 2

                                onTextChanged: page.errorMessage = ""
                                placeholderText : qsTr("Enter %1").arg(deviceName.text)
                                onActiveFocusChanged: {
                                    if (activeFocus) {
                                        // Ensures the field is visible above the keyboard
                                        formFlickable.ensureVisible(deviceNameField)
                                    }
                                }
                            }
                        }

                        Item {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 5
                        }

                        PrefsItemDelegate {
                            id: username
                            Layout.fillWidth: true
                            text: qsTr("Username")

                            indicator: PrefsTextField {
                                id: usernameField
                                x: username.width - width - username.rightPadding
                                y: username.topPadding + (username.availableHeight - height) / 2

                                onTextChanged: page.errorMessage = ""
                                placeholderText : qsTr("Enter %1").arg(username.text)
                                onActiveFocusChanged: {
                                    if (activeFocus) {
                                        // Ensures the field is visible above the keyboard
                                        formFlickable.ensureVisible(usernameField)
                                    }
                                }

                            }
                        }

                        PrefsItemDelegate {
                            id: password
                            Layout.fillWidth: true
                            text: qsTr("Password")

                            indicator: PrefsTextField {
                                id: passwordField
                                x: password.width - width - password.rightPadding
                                y: password.topPadding + (password.availableHeight - height) / 2

                                onTextChanged: page.errorMessage = ""
                                placeholderText : qsTr("Enter %1").arg(password.text)
                                onActiveFocusChanged: {
                                    if (activeFocus) {
                                        // Ensures the field is visible above the keyboard
                                        formFlickable.ensureVisible(passwordField)
                                    }
                                }

                                echoMode: eyeButton.checked ? TextField.Normal : TextField.Password
                                rightPadding: 40

                                ToolButton {
                                    id: eyeButton
                                    anchors.right: parent.right
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.rightMargin: 6
                                    checkable: true
                                    checked: false
                                    z: 10
                                    icon.source: checked ? "qrc:/assets/icons/ic_eye-on.svg" : "qrc:/assets/icons/ic_eye-off.svg"
                                    icon.color: Colors.textSecondary
                                    background: Item {
                                        implicitWidth: 28
                                        implicitHeight: 28
                                    }
                                }
                            }
                        }
                    }
                }

                Control {
                    Layout.fillWidth: true

                    contentItem: ColumnLayout {
                        spacing: 10

                        PrefsItemDelegate {
                            id: performanceRadioButton
                            Layout.fillWidth: true
                            text: qsTr("Performance")
                            ButtonGroup { id: tabGroup }

                            indicator: RowLayout {
                                x: performanceRadioButton.width - width - performanceRadioButton.rightPadding
                                y: performanceRadioButton.topPadding + (performanceRadioButton.availableHeight - height) / 2

                                spacing: 20

                                PrefsTabButton {
                                    id: bestButton
                                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                                    ButtonGroup.group: tabGroup
                                    checked: true
                                    text: qsTr("Best")
                                    visible: !!text
                                    font.weight: Font.Normal
                                }

                                PrefsTabButton {
                                    id: customButton
                                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                                    ButtonGroup.group: tabGroup
                                    text: qsTr("Custom")
                                    visible: !!text
                                    font.weight: Font.Normal
                                }

                                ToolButton {
                                    id: editButton
                                    visible: customButton.checked
                                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                                    icon.source: "qrc:/assets/icons/ic_edit.svg"
                                    icon.color: Colors.textSecondary
                                    background: Item {
                                        implicitWidth: 28
                                        implicitHeight: 28
                                    }

                                    onClicked: {
                                        if(!graphicsDialog.opened) {
                                            graphicsDialog.open()
                                        }
                                    }
                                }
                            }
                        }

                        PrefsItemDelegate {
                            id: enableDelegate
                            Layout.fillWidth: true
                            text: qsTr("Enable")

                            indicator: RowLayout {
                                x: enableDelegate.width - width - enableDelegate.rightPadding
                                y: enableDelegate.topPadding + (enableDelegate.availableHeight - height) / 2

                                spacing: 20

                                PrefsButton {
                                    id: audioButton
                                    checkable: true
                                    text: qsTr("Audio")
                                    visible: !!text
                                    font.weight: Font.Normal
                                }

                                PrefsButton {
                                    id: microphoneButton
                                    checkable: true
                                    text: qsTr("Microphone")
                                    visible: !!text
                                    font.weight: Font.Normal
                                }
                            }
                        }

                        PrefsItemDelegate {
                            id: redirectDelegate
                            Layout.fillWidth: true
                            text: qsTr("Redirect")

                            indicator: RowLayout {
                                x: redirectDelegate.width - width - redirectDelegate.rightPadding
                                y: redirectDelegate.topPadding + (redirectDelegate.availableHeight - height) / 2
                                spacing: 20

                                PrefsButton {
                                    id: driveButton
                                    checkable: true
                                    text: qsTr("Drive")
                                    visible: !!text
                                    font.weight: Font.Normal
                                }

                                PrefsButton {
                                    id: usbDeviceButton
                                    checkable: true
                                    text: qsTr("USB Device")
                                    visible: !!text
                                    font.weight: Font.Normal
                                }
                            }
                        }

                        PrefsItemDelegate {
                            id: securityDelegate
                            Layout.fillWidth: true
                            text: qsTr("Security")

                            indicator: RowLayout {
                                x: securityDelegate.width - width - securityDelegate.rightPadding
                                y: securityDelegate.topPadding + (securityDelegate.availableHeight - height) / 2

                                PrefsButton {
                                    id: securityButton
                                    checkable: true
                                    text: qsTr("NLA")
                                    visible: !!text
                                    font.weight: Font.Normal
                                }
                            }
                        }

                        PrefsItemDelegate {
                            id: enableGateway
                            Layout.fillWidth: true
                            implicitHeight: 48
                            text: qsTr("Enable RD Gateway")

                            indicator: PrefsSwitch {
                                x: enableGateway.width - width - enableGateway.rightPadding
                                y: enableGateway.topPadding + (enableGateway.availableHeight - height) / 2
                                id: rdGateWay
                                checkable: true
                                rightPadding: 0
                                onClicked: {}
                            }
                        }

                        PrefsItemDelegate {
                            id: rdGateway
                            Layout.fillWidth: true
                            text: qsTr("RD Gateway")
                            visible: rdGateWay.checked

                            indicator: ColumnLayout {
                                x: rdGateway.width - width - rdGateway.rightPadding
                                y: rdGateway.topPadding + (rdGateway.availableHeight - height) / 2
                                spacing: 10

                                PrefsTextField {
                                    id: gatewayIp
                                    Layout.preferredWidth: bottomLayout.implicitWidth
                                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                                    placeholderText: qsTr("Gateway IP")

                                    onActiveFocusChanged: {
                                        if (activeFocus) {
                                            // Ensures the field is visible above the keyboard
                                            formFlickable.ensureVisible(gatewayIp)
                                        }
                                    }
                                }

                                RowLayout {
                                    id: bottomLayout
                                    spacing: 20
                                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                                    PrefsTextField {
                                        id: gatewayUserName
                                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                                        placeholderText: qsTr("Username")

                                        onActiveFocusChanged: {
                                            if (activeFocus) {
                                                // Ensures the field is visible above the keyboard
                                                formFlickable.ensureVisible(gatewayUserName)
                                            }
                                        }
                                    }

                                    PrefsTextField {
                                        id: gatewayPassword
                                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                                        placeholderText: qsTr("Password")
                                        echoMode: TextInput.Password

                                        onActiveFocusChanged: {
                                            if (activeFocus) {
                                                // Ensures the field is visible above the keyboard
                                                formFlickable.ensureVisible(gatewayPassword)
                                            }
                                        }
                                    }
                                }
                            }
                        }


                        PrefsItemDelegate {
                            id: saveButton
                            Layout.fillWidth: true
                            rightPadding: 0
                            leftPadding: 0
                            text: page.hasError ? page.errorMessage : ""
                            palette.text: Colors.statusError

                            background: Item {
                                implicitWidth: 100
                                implicitHeight: 40
                            }

                            indicator: RowLayout {
                                x: saveButton.width - width - saveButton.rightPadding
                                y: saveButton.topPadding + (saveButton.availableHeight - height) / 2

                                spacing: 20

                                PrefsButton {
                                    text: qsTr("Clear")
                                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                    onClicked: {
                                        page.errorMessage = ""
                                        saveBtn.clearEntryFields()
                                    }
                                }

                                PrefsButton {
                                    id: saveBtn
                                    text: qsTr("Save")
                                    highlighted: true
                                    enabled: !page.hasError
                                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter

                                    // property var dummyConnections: [
                                    //     ["Conn 1",  "192.168.0.1",  "PC-01", "user1",  "pass1",  "Best",        true,  true,  false, false, true,  false, "",               "",            ""],
                                    //     ["Conn 2",  "192.168.0.2",  "PC-02", "user2",  "pass2",  "Performance", true,  false, true,  false, false, true,  "gw.local",       "gwuser2",     "gwpass2"],
                                    //     ["Conn 3",  "192.168.0.3",  "PC-03", "user3",  "pass3",  "Low",         false, false, false, true,  true,  false, "",               "",            ""],
                                    //     ["Conn 4",  "192.168.0.4",  "PC-04", "user4",  "pass4",  "Best",        true,  true,  true,  false, false, false, "",               "",            ""],
                                    //     ["Conn 5",  "192.168.0.5",  "PC-05", "user5",  "pass5",  "Performance", true,  false, false, true,  true,  true,  "gateway5.com",  "gwuser5",     "gwpass5"],
                                    //     ["Conn 6",  "192.168.0.6",  "PC-06", "user6",  "pass6",  "Best",        false, true,  true,  false, false, false, "",               "",            ""],
                                    //     ["Conn 7",  "192.168.0.7",  "PC-07", "user7",  "pass7",  "Low",         false, false, false, false, true,  true,  "gw7.net",       "gwuser7",     "gwpass7"],
                                    //     ["Conn 8",  "192.168.0.8",  "PC-08", "user8",  "pass8",  "Best",        true,  true,  false, false, true,  false, "",               "",            ""],
                                    //     ["Conn 9",  "192.168.0.9",  "PC-09", "user9",  "pass9",  "Performance", true,  false, true,  true,  false, false, "",               "",            ""],
                                    //     ["Conn 10", "192.168.0.10", "PC-10", "user10", "pass10", "Low",         false, true,  false, false, true,  true,  "gw10.com",      "gwuser10",    "gwpass10"],

                                    //     ["Conn 11", "192.168.0.11", "PC-11", "user11", "pass11", "Best",        true,  true,  true,  true,  false, false, "",               "",            ""],
                                    //     ["Conn 12", "192.168.0.12", "PC-12", "user12", "pass12", "Performance", true,  false, false, false, true,  false, "",               "",            ""],
                                    //     ["Conn 13", "192.168.0.13", "PC-13", "user13", "pass13", "Low",         false, false, true,  false, false, true,  "gw13.net",      "gwuser13",    "gwpass13"],
                                    //     ["Conn 14", "192.168.0.14", "PC-14", "user14", "pass14", "Best",        true,  true,  false, false, true,  false, "",               "",            ""],
                                    //     ["Conn 15", "192.168.0.15", "PC-15", "user15", "pass15", "Performance", true,  false, true,  true,  false, false, "",               "",            ""],
                                    //     ["Conn 16", "192.168.0.16", "PC-16", "user16", "pass16", "Low",         false, true,  false, false, true,  true,  "gw16.com",      "gwuser16",    "gwpass16"],
                                    //     ["Conn 17", "192.168.0.17", "PC-17", "user17", "pass17", "Best",        true,  true,  true,  false, false, false, "",               "",            ""],
                                    //     ["Conn 18", "192.168.0.18", "PC-18", "user18", "pass18", "Performance", true,  false, false, true,  true,  false, "",               "",            ""],
                                    //     ["Conn 19", "192.168.0.19", "PC-19", "user19", "pass19", "Low",         false, false, false, false, false, true,  "gw19.net",      "gwuser19",    "gwpass19"],
                                    //     ["Conn 20", "192.168.0.20", "PC-20", "user20", "pass20", "Best",        true,  true,  false, false, true,  false, "",               "",            ""]
                                    // ]

                                    // function insertDummy() {
                                    //     for (var i = 0; i < dummyConnections.length; i++) {
                                    //         dataBase.insertIntoValues = dummyConnections[i];
                                    //         dataBase.qmlInsertServerData()
                                    //     }
                                    // }

                                    onClicked: {
                                        // 1️⃣ Validate form input
                                        if (!validateRequiredFields()) {
                                            console.warn("Validation failed — missing required fields.")
                                            return
                                        }

                                        // 2️⃣ Collect all field values in proper order
                                        const newList = [
                                                          connectionField.text.trim(),
                                                          serverIpField.text.trim(),
                                                          deviceNameField.text.trim(),
                                                          usernameField.text.trim(),
                                                          passwordField.text.trim(),
                                                          bestButton.checked ? "Best" : "Auto",
                                                          audioButton.checked,
                                                          microphoneButton.checked,
                                                          driveButton.checked,
                                                          usbDeviceButton.checked,
                                                          securityButton.checked,
                                                          rdGateWay.checked,
                                                          gatewayIp.text.trim(),
                                                          gatewayUserName.text.trim(),
                                                          gatewayPassword.text.trim(),
                                                          graphicsDialog.useAVC,
                                                          graphicsDialog.enableAnimation,
                                                          graphicsDialog.enableGDI
                                                      ]

                                        dataBase.insertIntoValues = newList

                                        // 3️⃣ Check if connection already exists (simple existence check)
                                        const exists = dataBase.serverExists(page.connectionId)

                                        if (exists) {
                                            dataBase.qmlUpdateServerData(page.connectionId)
                                            serverInfo.removeConnection(page.connectionId)
                                            console.log("🔄 Existing connection updated:", connectionField.text.trim())
                                        } else {
                                            dataBase.qmlInsertServerData()
                                            console.log("✅ New connection added:", connectionField.text.trim())
                                        }

                                        // 4️⃣ Refresh server list model
                                        serverInfo.setServerInfo(page.connectionId)

                                        // 5️⃣ Reset form
                                        clearEntryFields()
                                    }

                                    function clearEntryFields() {
                                        // --- Text Fields ---
                                        const textFields = [
                                                             connectionField,
                                                             serverIpField,
                                                             deviceNameField,
                                                             usernameField,
                                                             passwordField,
                                                             gatewayIp,
                                                             gatewayUserName,
                                                             gatewayPassword
                                                         ]

                                        for (const field of textFields)
                                            field.text = ""

                                        // --- Toggle / Check Buttons ---
                                        const toggleButtons = [
                                                                audioButton,
                                                                microphoneButton,
                                                                driveButton,
                                                                usbDeviceButton,
                                                                securityButton,
                                                                rdGateWay
                                                            ]

                                        for (const btn of toggleButtons)
                                            btn.checked = false

                                        // Optional: reset any displayed messages or validation
                                        page.errorMessage = ""
                                        page.connectionId = ""

                                        graphicsDialog.useAVC = true;
                                        graphicsDialog.enableAnimation = false;
                                        graphicsDialog.enableGDI  = false;

                                        bestButton.checked = true;
                                        customButton.checked = false;
                                    }

                                    function validateRequiredFields() {
                                        page.errorMessage = ""

                                        const fields = [
                                                         { value: connectionField.text, label: qsTr("Connection Name") },
                                                         { value: serverIpField.text,  label: qsTr("Server IP") },
                                                         { value: deviceNameField.text, label: qsTr("Device Name") },
                                                         { value: usernameField.text,   label: qsTr("Username") },
                                                         { value: passwordField.text,   label: qsTr("Password") }
                                                     ]

                                        for (const field of fields) {
                                            if (!field.value || field.value.trim() === "") {
                                                page.errorMessage = qsTr("%1 cannot be empty").arg(field.label)
                                                return false
                                            }
                                        }

                                        return true
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Item { Layout.fillHeight: true }
        }
    }

    Connections {
        target: dataBase
        function onRefreshTable() {
            sessionModel.reloadServers()
        }
    }

    ChooseGraphicsDialog {
        id: graphicsDialog
    }
}
