import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import QtQuick.Controls.impl 2.12
import App.Styles 1.0

import "../components"
import "../controls"

BasicPage {
    id: page
    padding: 20
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
                visible: false
                Layout.fillWidth: true
                Layout.maximumHeight: page.height /3

                contentItem:  ListView {
                    id: listView
                    anchors.fill: parent

                    header: Control {
                        width: listView.width
                        padding: 20

                        contentItem: RowLayout {
                            PrefsHeader {
                                Layout.fillWidth: true
                                text: qsTr("Connection Name")
                                font.pixelSize: 18
                            }

                            PrefsHeader {
                                Layout.fillWidth: true
                                text: qsTr("Server Ip")
                                font.pixelSize: 18
                            }

                            PrefsHeader {
                                Layout.fillWidth: true
                                text: qsTr("Auto")
                                font.pixelSize: 18
                            }

                            PrefsHeader {
                                Layout.fillWidth: true
                                text: qsTr("Manage")
                                font.pixelSize: 18
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
                            rightButtonText: qsTr("Custom")
                        }

                        PrefsButtonDelegate {
                            Layout.fillWidth: true
                            text: qsTr("Enable")
                            leftButtonText: qsTr("Audio")
                            rightButtonText: qsTr("Microphone")
                        }

                        PrefsButtonDelegate {
                            Layout.fillWidth: true
                            text: qsTr("Redirect")
                            leftButtonText: qsTr("Drive")
                            rightButtonText: qsTr("USB Device")
                        }

                        PrefsButtonDelegate {
                            Layout.fillWidth: true
                            text: qsTr("Security")
                            leftButtonText: qsTr("NLA")
                        }
                    }
                }

                PrefsItemDelegate {
                    id: rdGateway
                    Layout.fillWidth: true
                    text: qsTr("RD Gateway")

                    indicator: ColumnLayout {
                        x: rdGateway.width - width - rdGateway.rightPadding
                        y: rdGateway.topPadding + (rdGateway.availableHeight - height) / 2
                        spacing: 10

                        RowLayout {
                            Layout.fillWidth: true

                            Item { Layout.fillWidth: true }

                            ColumnLayout {
                                spacing: 20
                                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter

                                PrefsButton {
                                    Layout.preferredWidth: bottomLayout.implicitWidth
                                    text: qsTr("Gateway IP")
                                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                                }

                                RowLayout {
                                    id: bottomLayout
                                    spacing: 20
                                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                                    PrefsButton {
                                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                                        text: qsTr("Username")
                                    }

                                    PrefsButton {
                                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                                        text: qsTr("Password")
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
            color: Qt.rgba(45/ 255, 47/255, 47/255, 1.0)
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
}
