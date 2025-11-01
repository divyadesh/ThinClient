import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import App.Styles 1.0

import "../components"
import "../controls"

BasicPage {
    id: root
    property bool server: false

    background: Rectangle {
        color: Colors.secondaryBackground
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Control {
            Layout.preferredWidth: 300
            Layout.fillHeight: true

            background: Rectangle {
                color: Colors.steelGray

                Rectangle {
                    anchors.right: parent.right
                    height: parent.height
                    width: 1
                    color: Colors.borderColor
                }
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20

                Control {
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                    contentItem: PageLogo {}
                }

                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 25
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    ButtonGroup { id: tabGroup }

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 15

                        PageTabButton {
                            Layout.fillWidth: true
                            ButtonGroup.group: tabGroup
                            checked: true
                            text: root.server ? qsTr("RD Server") : qsTr("RD Windows")
                            onClicked: pageStack.replace(serverSettingsPage, {pageTitle: text })
                        }

                        PageTabButton {
                            Layout.fillWidth: true
                            ButtonGroup.group: tabGroup
                            text: qsTr("Network")
                            onClicked: pageStack.replace(networkPage, {pageTitle: text })
                        }

                        PageTabButton {
                            Layout.fillWidth: true
                            ButtonGroup.group: tabGroup
                            text: qsTr("Display")
                            onClicked: pageStack.replace(displayPage, {pageTitle: text })
                        }

                        PageTabButton {
                            Layout.fillWidth: true
                            ButtonGroup.group: tabGroup
                            text: qsTr("Device Settings")
                            onClicked: pageStack.replace(deviceSettingsPage, {pageTitle: text })
                        }

                        PageTabButton {
                            Layout.fillWidth: true
                            ButtonGroup.group: tabGroup
                            text: qsTr("Device Info")
                            onClicked: pageStack.replace(deviceInfoPage, {pageTitle: text })
                        }

                        Item { Layout.fillHeight: true }
                    }
                }
            }
        }

        BasicPage {
            Layout.fillWidth: true
            Layout.fillHeight: true

            background: Rectangle {
                color: "#333333"
            }

            contentItem: PrefsStackView {
                id: pageStack

                Component.onCompleted:  {
                    pageStack.replace(serverSettingsPage, { pageTitle: qsTr("RD Server") })
                }
            }
        }
    }

    Component {
        id: deviceInfoPage
        DeviceInfoPage {
            onBackToHome: { root.pageStack.pop() }
        }
    }

    Component {
        id: deviceSettingsPage
        DeviceSettingsPage {
            onBackToHome: { root.pageStack.pop() }
        }
    }

    Component {
        id: serverSettingsPage
        ServerSettingsPage {
            onBackToHome: { root.pageStack.pop() }
        }
    }

    Component {
        id: displayPage
        DisplayPage {
            onBackToHome: { root.pageStack.pop() }
        }
    }

    Component {
        id: networkPage
        NetworkPage {
            onBackToHome: { root.pageStack.pop() }
        }
    }
}
