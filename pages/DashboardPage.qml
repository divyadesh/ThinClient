import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import App.Styles 1.0

import "../components"
import "../controls"

BasicPage {
    id: root
    property bool server: false
    padding: 20

    background: Rectangle {
        color: Colors.secondaryBackground
    }

    RowLayout {
        anchors.fill: parent
        spacing: 20

        Control {
            Layout.preferredWidth: 300
            Layout.fillHeight: true

            background: Rectangle {
                color: Colors.surfaceBackground
                radius: 4
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
                    Layout.preferredHeight: 10
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    ButtonGroup { id: tabGroup }

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 10

                        PageTabButton {
                            Layout.fillWidth: true
                            ButtonGroup.group: tabGroup
                            checked: true
                            text: root.server ? qsTr("RD Server") : qsTr("RD Windows")
                            onClicked: pageStack.replace(serverSettingsPage)
                        }

                        PageTabButton {
                            Layout.fillWidth: true
                            ButtonGroup.group: tabGroup
                            text: qsTr("Network")
                            onClicked: pageStack.replace(networkPage)
                        }

                        PageTabButton {
                            Layout.fillWidth: true
                            ButtonGroup.group: tabGroup
                            text: qsTr("Display")
                            onClicked: pageStack.replace(displayPage)
                        }

                        PageTabButton {
                            Layout.fillWidth: true
                            ButtonGroup.group: tabGroup
                            text: qsTr("Device Settings")
                            onClicked: pageStack.replace(deviceSettingsPage)
                        }

                        PageTabButton {
                            Layout.fillWidth: true
                            ButtonGroup.group: tabGroup
                            text: qsTr("Device Info")
                            onClicked: pageStack.replace(deviceInfoPage)
                        }

                        Item { Layout.fillHeight: true }
                    }
                }
            }
        }

        Control {
            Layout.fillWidth: true
            Layout.fillHeight: true

            background: Rectangle {
                color: Colors.surfaceBackground
                radius: 4
            }

            contentItem: PrefsStackView {
                id: pageStack
            }
        }
    }

    Component {
        id: deviceInfoPage
        DeviceInfoPage {}
    }

    Component {
        id: deviceSettingsPage
        DeviceSettingsPage {}
    }

    Component {
        id: serverSettingsPage
        ServerSettingsPage {}
    }

    Component {
        id: displayPage
        DisplayPage {}
    }

    Component {
        id: networkPage
        NetworkPage {}
    }
}
