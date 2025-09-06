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

    header: PageHeader {
        pageTitle: page.pageTitle
        onBackPressed: {
            if(pageStack.depth == 1) {
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

                    model: ListModel {
                        ListElement { name: "Windows"; ip: "192.168.1.15" }
                        ListElement { name: "Server"; ip: "192.168.1.24:3228" }
                        ListElement { name: "Laptop"; ip: "192.168.1.50" }
                        ListElement { name: "Laptop"; ip: "192.168.1.50" }
                        ListElement { name: "Laptop"; ip: "192.168.1.50" }
                        ListElement { name: "Laptop"; ip: "192.168.1.50" }
                        ListElement { name: "Laptop"; ip: "192.168.1.50" }
                        ListElement { name: "Laptop"; ip: "192.168.1.50" }
                        ListElement { name: "Laptop"; ip: "192.168.1.50" }
                    }

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
                                    text: name
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
                                        text: ip
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
                                        onClicked: {  }
                                    }

                                    PrefsLink {
                                        text: qsTr("Delete")
                                        onClicked: { pageStack.push(deleteConnection) }
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
                                    checkable: true
                                    text: qsTr("Audio")
                                    visible: !!text
                                    font.weight: Font.Normal
                                }

                                PrefsButton {
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
                                    checkable: true
                                    text: qsTr("Drive")
                                    visible: !!text
                                    font.weight: Font.Normal
                                }

                                PrefsButton {
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
                                    Layout.preferredWidth: bottomLayout.implicitWidth
                                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                                    placeholderText: qsTr("Gateway IP")
                                }

                                RowLayout {
                                    id: bottomLayout
                                    spacing: 20
                                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                                    PrefsTextField {
                                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                                        placeholderText: qsTr("Username")
                                    }

                                    PrefsTextField {
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
                }
            }
        }
    }

    component PrefsButtonDelegate: PrefsItemDelegate {
        id: _control
        property string leftButtonText: ""
        property string rightButtonText: ""

        ButtonGroup { id: tabGroup }

        indicator: RowLayout {
            x: _control.width - width - _control.rightPadding
            y: _control.topPadding + (_control.availableHeight - height) / 2

            spacing: 20

            PrefsTabButton {
                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                ButtonGroup.group: tabGroup
                checked: true
                text: _control.leftButtonText
                visible: !!text
                font.weight: Font.Normal
            }

            PrefsTabButton {
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
        DeleteWifiConnection {}
    }
}
