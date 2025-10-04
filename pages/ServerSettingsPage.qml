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
    property bool isServer: false
    property bool editConnection: false
    property string connectionName: ""
    property string ipAddr: ""

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

            PrefsHeader {
                Layout.fillWidth: true
                text: qsTr("RD Servers")
            }

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

                    ScrollBar.vertical: ScrollBar{
                        //policy: "AlwaysOn"
                    }
                    ScrollIndicator.vertical: ScrollIndicator { }

                    model: serverInfo
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
                                    text: serverInformation.connectionName
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
                                        text: serverInformation.serverIp
                                    }

                                    PrefsBusyIndicator {
                                        radius: 10
                                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                                    }
                                }
                            }

                            // Switch
                            Control {
                                Layout.preferredWidth: listView.width / 4
                                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                                RadioButton {
                                    anchors.left: parent.left
                                    anchors.verticalCenter: parent.verticalCenter
                                    palette.text: Colors.accentPrimary
                                    palette.windowText: Colors.textPrimary
                                    text: qsTr("Auto-connect")
                                    visible: !!text
                                    font.weight: Font.Normal
                                    spacing: 10
                                    ButtonGroup.group: radioGroup
                                    checked: serverInformation.autoEnable
                                    onToggled: {
                                        serverInfo.resetAutoConnect()
                                        serverInformation.autoEnable = checked
                                        persistData.saveData("AutoConnectIpAddress", serverInformation.serverIp)
                                        persistData.saveData("AutoConnectConnectionName", serverInformation.connectionName)
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
                                        onClicked: {}
                                    }

                                    PrefsLink {
                                        text: qsTr("Edit")
                                        onClicked: {
                                            dataBase.qmlQueryServerTable(serverInformation.connectionName, serverInformation.serverIp)
                                            if(dataBase.queryResultList.length > 0) {
                                                page.editConnection = true
                                                page.connectionName = dataBase.queryResultList[0]
                                                page.ipAddr         = dataBase.queryResultList[1]
                                                connectionField.text                        = dataBase.queryResultList[0]
                                                serverIpField.text                          = dataBase.queryResultList[1]
                                                deviceNameField.text                        = dataBase.queryResultList[2]
                                                usernameField.text                          = dataBase.queryResultList[3]
                                                passwordField.text                          = dataBase.queryResultList[4]
                                                performanceRadioButton.leftButton.checked   = (dataBase.queryResultList[5] === "Best")
                                                performanceRadioButton.rightButton.checked  = (dataBase.queryResultList[5] === "Auto")
                                                audioButton.checked                         = (dataBase.queryResultList[6] === "true")
                                                microphoneButton.checked                    = (dataBase.queryResultList[7] === "true")
                                                driveButton.checked                         = (dataBase.queryResultList[8] === "true")
                                                usbDeviceButton.checked                     = (dataBase.queryResultList[9] === "true")
                                                securityButton.checked                      = (dataBase.queryResultList[10] === "true")
                                                rdGateWay.checked                           = (dataBase.queryResultList[11] === "true")
                                                gatewayIp.text                              = dataBase.queryResultList[12]
                                                gatewayUserName.text                        = dataBase.queryResultList[13]
                                                gatewayPassword.text                        = dataBase.queryResultList[14]
                                            }
                                        }
                                    }

                                    PrefsLink {
                                        text: qsTr("Delete")
                                        onClicked: {
                                            page.connectionName = serverInformation.connectionName
                                            page.ipAddr = serverInformation.serverIp
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
                                    regularExpression: /^((?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\.){0,3}(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])$/
                                }

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
                    }
                }

                PrefsItemDelegate {
                    id: rdGateway
                    Layout.fillWidth: true
                    text: qsTr("RD Gateway")

                    contentItem: RowLayout {
                        PrefsButton {
                            id: rdGateWay
                            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                            checkable: true
                            text: qsTr("RD Gateway")
                            onClicked: {}
                        }
                    }

                    indicator: ColumnLayout {
                        enabled: rdGateWay.checked
                        x: rdGateway.width - width - rdGateway.rightPadding
                        y: rdGateway.topPadding + (rdGateway.availableHeight - height) / 2
                        spacing: 10

                        RowLayout {
                            Layout.fillWidth: true

                            Item { Layout.fillWidth: true }

                            ColumnLayout {
                                spacing: 20
                                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter

                                PrefsTextField {
                                    id: gatewayIp
                                    Layout.preferredWidth: bottomLayout.implicitWidth
                                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                                    placeholderText: qsTr("Gateway IP")
                                    validator: RegularExpressionValidator {
                                        regularExpression:  /^((?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\.){0,3}(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])$/
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
                    }
                }
            }

            Item { Layout.fillHeight: true }
        }
    }

    footer: Control {
        padding: 20
        clip: true

        background: Rectangle {
            implicitHeight: 84
            color: Colors.headerColor
        }

        contentItem: ColumnLayout {
            spacing: 10

            RowLayout {
                Layout.fillWidth: true

                Item { Layout.fillWidth: true }

                PrefsButton {
                    text: qsTr("Save")
                    highlighted: true
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    onClicked: {
                        let newList = [connectionField.text, serverIpField.text, deviceNameField.text, usernameField.text, passwordField.text,
                                       performanceRadioButton.tabGroup.checkedButton.text,
                                       audioButton.checked, microphoneButton.checked, driveButton.checked, usbDeviceButton.checked, securityButton.checked,
                                       rdGateWay.checked, gatewayIp.text, gatewayUserName.text, gatewayPassword.text]
                        dataBase.insertIntoValues = newList
                        if(!page.editConnection) {
                            dataBase.qmlInsertServerData()
                            serverInfo.setServerInfo(connectionField.text, serverIpField.text)
                        }
                        else {
                            page.editConnection = false
                            dataBase.qmlUpdateServerData(page.connectionName, page.ipAddr)
                            serverInfo.removeConnection(page.connectionName, page.ipAddr)
                            serverInfo.setServerInfo(connectionField.text, serverIpField.text)

                            connectionField.text = ""; serverIpField.text = ""; deviceNameField.text = ""; usernameField.text = ""; passwordField.text = "";
                            audioButton.checked = false; microphoneButton.checked = false; driveButton.checked = false; usbDeviceButton.checked = false;
                            securityButton.checked = false; rdGateWay.checked = false; gatewayIp.text = ""; gatewayUserName.text = ""; gatewayPassword.text = "";
                        }
                    }
                }
            }
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
                dataBase.removeServer(page.connectionName, page.ipAddr)
                serverInfo.removeConnection(page.connectionName, page.ipAddr)
                page.connectionName = ""
                page.ipAddr = ""
            }
        }
    }
}
