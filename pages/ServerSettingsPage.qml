import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import QtQuick.Controls.impl 2.12
import App.Styles 1.0

import "../components"
import "../controls"
import "../dialogs"

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

    contentItem: Flickable {
        width: parent.width
        clip: true
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

                background: Rectangle {
                    color: Colors.btnBg
                    radius: 8
                }

                contentItem: ListView {
                    id: listView
                    spacing: 8
                    clip: true
                    ScrollBar.vertical: ScrollBar{}
                    ScrollIndicator.vertical: ScrollIndicator { }

                    model: serverModel
                    header: Control {
                        width: listView.width
                        padding: 20

                        contentItem:  RowLayout {
                            spacing: 20

                            Control {
                                Layout.preferredWidth: listView.width / 4

                                PrefsLabel {
                                    anchors.left: parent.left
                                    anchors.verticalCenter: parent.verticalCenter
                                    horizontalAlignment: Label.AlignLeft
                                    verticalAlignment: Label.AlignVCenter
                                    font.weight: Font.DemiBold
                                    text: qsTr("Connection Name")
                                    font.pixelSize: 18
                                }
                            }

                            Control {
                                Layout.preferredWidth: listView.width / 4

                                PrefsLabel {
                                    anchors.left: parent.left
                                    anchors.verticalCenter: parent.verticalCenter
                                    horizontalAlignment: Label.AlignLeft
                                    verticalAlignment: Label.AlignVCenter
                                    text: qsTr("Server IP")
                                    font.pixelSize: 18
                                    font.weight: Font.DemiBold
                                }
                            }

                            Control {
                                Layout.preferredWidth: listView.width / 4

                                PrefsLabel {
                                    anchors.left: parent.left
                                    anchors.verticalCenter: parent.verticalCenter
                                    horizontalAlignment: Label.AlignLeft
                                    verticalAlignment: Label.AlignVCenter
                                    text: qsTr("Auto")
                                    font.pixelSize: 18
                                    font.weight: Font.DemiBold
                                }
                            }

                            Control {
                                Layout.preferredWidth: listView.width / 4

                                PrefsLabel {
                                    anchors.left: parent.left
                                    anchors.verticalCenter: parent.verticalCenter
                                    horizontalAlignment: Label.AlignLeft
                                    verticalAlignment: Label.AlignVCenter
                                    text: qsTr("Manage")
                                    font.pixelSize: 18
                                    font.weight: Font.DemiBold
                                }
                            }
                        }
                    }

                    delegate: Control {
                        id: _controlDelegate
                        width: ListView.view.width
                        padding: 20
                        topPadding: 12
                        bottomPadding: 12
                        hoverEnabled: true

                        background: Rectangle {
                            implicitWidth: 100
                            implicitHeight: 28
                            radius: height / 2
                            color: _controlDelegate.hovered ? Colors.steelGray : "transparent"
                            border.width: 1
                            border.color: _controlDelegate.hovered ? Colors.borderColor : "transparent"
                        }

                        contentItem: RowLayout {
                            spacing: 20

                            // Name
                            Control {
                                Layout.preferredWidth: listView.width / 4
                                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                                PrefsLabel {
                                    anchors.left: parent.left
                                    anchors.verticalCenter: parent.verticalCenter
                                    horizontalAlignment: Label.AlignLeft
                                    verticalAlignment: Label.AlignVCenter
                                    text: connectionName
                                }
                            }

                            // IP
                            Control {
                                Layout.preferredWidth: listView.width / 4
                                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                                RowLayout {
                                    anchors.left: parent.left
                                    anchors.verticalCenter: parent.verticalCenter
                                    spacing: 20

                                    PrefsLabel {
                                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                                        horizontalAlignment: Label.AlignLeft
                                        verticalAlignment: Label.AlignVCenter
                                        text: serverIp
                                    }

                                    PrefsBusyIndicator {
                                        id: busyIndicator
                                        radius: 10
                                        running: false
                                        visible: running
                                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                                    }
                                }
                            }

                            // Switch
                            Control {
                                Layout.preferredWidth: listView.width / 4
                                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                                RadioButton {
                                    id: autoConnectRadioButton
                                    anchors.left: parent.left
                                    anchors.verticalCenter: parent.verticalCenter
                                    palette.text: Colors.accentPrimary
                                    palette.windowText: Colors.textPrimary
                                    text: qsTr("Auto-connect")
                                    visible: !!text
                                    font.weight: Font.Normal
                                    spacing: 10
                                    ButtonGroup.group: radioGroup
                                    checked: autoConnect

                                    onCheckedChanged: {
                                        serverInfo.resetAutoConnect()
                                        autoConnect = checked
                                    }
                                }
                            }

                            // Action buttons

                            Control {
                                Layout.preferredWidth: listView.width / 4
                                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                                contentItem:  RowLayout {
                                    spacing: 10

                                    PrefsLink {
                                        text: qsTr("Connect")
                                        onClicked: {
                                            serverInfo.connectRdServer(connectionId)
                                        }
                                    }

                                    PrefsLink {
                                        text: qsTr("Edit")
                                        onClicked: {
                                            page.connectionId = connectionId
                                            populateConnectionFields()
                                        }

                                        function populateConnectionFields() {
                                            // --- Text fields ---
                                            connectionField.text   = connectionName || ""
                                            serverIpField.text     = serverIp || ""
                                            deviceNameField.text   = deviceName || ""
                                            usernameField.text     = userName || ""
                                            passwordField.text     = password || ""

                                            // --- Performance radio ---
                                            performanceRadioButton.leftButton.checked  = (performance === "Best")
                                            performanceRadioButton.rightButton.checked = (performance === "Auto")

                                            // --- Feature toggles ---
                                            audioButton.checked        = isTrue(enableAudio)
                                            microphoneButton.checked   = isTrue(enableMicrophone)
                                            driveButton.checked        = isTrue(redirectDrive)
                                            usbDeviceButton.checked    = isTrue(redirectUsbDevice)
                                            securityButton.checked     = isTrue(security)
                                            rdGateWay.checked          = isTrue(gatewayValue)

                                            // --- Gateway fields ---
                                            if(gatewayValue) {
                                                gatewayIp.text        = gatewayIpValue || ""
                                                gatewayUserName.text  = gatewayUserNameValue || ""
                                                gatewayPassword.text  = gatewayPasswordValue || ""
                                            }

                                            console.log(`✅ Populated fields for connection: ${connectionName} (${serverIp})`)
                                        }

                                        function isTrue(value) {
                                            return value === "true" || value === true
                                        }
                                    }

                                    PrefsLink {
                                        text: qsTr("Delete")
                                        onClicked: {
                                            page.connectionId = connectionId
                                            pageStack.push(deleteConnection)
                                        }
                                    }

                                    Item {
                                        Layout.fillWidth: true
                                    }
                                }
                            }
                        }
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
                                echoMode: TextInput.Password
                            }
                        }
                    }
                }

                Control {
                    Layout.fillWidth: true

                    contentItem: ColumnLayout {
                        spacing: 10

                        PrefsButtonDelegate {
                            id: performanceRadioButton
                            Layout.fillWidth: true
                            text: qsTr("Performance")
                            leftButtonText: qsTr("Best")
                            rightButtonText: qsTr("Auto")
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
                                }

                                RowLayout {
                                    id: bottomLayout
                                    spacing: 20
                                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                                    PrefsTextField {
                                        id: gatewayUserName
                                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                                        placeholderText: qsTr("Username")
                                    }

                                    PrefsTextField {
                                        id: gatewayPassword
                                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                                        placeholderText: qsTr("Password")
                                        echoMode: TextInput.Password
                                    }
                                }
                            }
                        }


                        PrefsItemDelegate {
                            id: saveButton
                            Layout.fillWidth: true
                            rightPadding: 0
                            leftPadding: 0

                            background: Item {
                                implicitWidth: 100
                                implicitHeight: 40
                            }

                            indicator: PrefsButton {
                                x: saveButton.width - width - saveButton.rightPadding
                                y: saveButton.topPadding + (saveButton.availableHeight - height) / 2

                                text: qsTr("Save")
                                highlighted: true
                                enabled: !page.hasError
                                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter

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
                                                      performanceRadioButton.tabGroup.checkedButton
                                                      ? performanceRadioButton.tabGroup.checkedButton.text
                                                      : "Best",
                                                      audioButton.checked,
                                                      microphoneButton.checked,
                                                      driveButton.checked,
                                                      usbDeviceButton.checked,
                                                      securityButton.checked,
                                                      rdGateWay.checked,
                                                      gatewayIp.text.trim(),
                                                      gatewayUserName.text.trim(),
                                                      gatewayPassword.text.trim()
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

            Item { Layout.fillHeight: true }
        }
    }

    component PrefsButtonDelegate: PrefsItemDelegate {
        id: _control
        property string leftButtonText: ""
        property string rightButtonText: ""
        property alias tabGroup: tabGroup
        property alias leftButton: leftButton
        property alias rightButton: rightButton

        ButtonGroup { id: tabGroup }

        indicator: RowLayout {
            x: _control.width - width - _control.rightPadding
            y: _control.topPadding + (_control.availableHeight - height) / 2

            spacing: 20

            PrefsTabButton {
                id: leftButton
                objectName: "leftButton"
                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                ButtonGroup.group: tabGroup
                checked: true
                text: _control.leftButtonText
                visible: !!text
                font.weight: Font.Normal
            }

            PrefsTabButton {
                id: rightButton
                objectName: "rightButton"
                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                ButtonGroup.group: tabGroup
                text: _control.rightButtonText
                visible: !!text
                font.weight: Font.Normal
            }
        }
    }

    Component {
        id: deleteConnection
        DeleteWifiConnection {
            onSigDelete: {
                dataBase.removeServer(page.connectionId)
                serverInfo.removeConnection(page.connectionId)
                page.connectionId = ""
            }
        }
    }

    Connections {
        target: dataBase
        function onRefreshTable() {
            serverModel.refresh()
        }
    }
}
