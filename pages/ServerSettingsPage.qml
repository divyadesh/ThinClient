import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0

import "../components"

BasicPage {
    id: serverSettingsPage
    padding: 20

    property bool addNewServer: false

    Loader {
        visible: asynchronous ? (status === Loader.Ready) ? true : false : true
        anchors.left: parent.left
        anchors.right: parent.right
        //width: sourceComponent.width
        height: sourceComponent.height
        anchors.centerIn: parent
        sourceComponent: !addNewServer ? showServerSettingsComponent : addNewServerComponent
    }
    Component {
        id: showServerSettingsComponent
        ColumnLayout {
            spacing: 20
            Layout.fillWidth: true
            Rectangle {
                id: displayHeader
                height: 40
                width: 250
                color: Colors.secondaryBackground
                radius: 6
                Text {
                    anchors.centerIn: parent
                    text: root.server ? qsTr("RD Server") : qsTr("RD Windows")
                }
            }
            RowLayout {
                id: heading
                // anchors.top: displayHeader.bottom
                // anchors.topMargin: 20
                Layout.fillWidth: true
                spacing: 10
                Rectangle {
                    height: 30
                    width: 150
                    color: Colors.secondaryBackground
                    radius: 6
                    Text {
                        anchors.centerIn: parent
                        text: qsTr("Connection Name")
                    }
                }
                Rectangle {
                    height: 30
                    width: 150
                    color: Colors.secondaryBackground
                    radius: 6
                    Text {
                        anchors.centerIn: parent
                        text: qsTr("Server Ip")
                    }
                }
                Rectangle {
                    height: 30
                    width: 150
                    color: Colors.secondaryBackground
                    radius: 6
                    Text {
                        anchors.centerIn: parent
                        text: qsTr("Auto")
                    }
                }
            }
            ListView {
                id: serverList
                // anchors.top: heading.bottom
                // anchors.topMargin: 20
                //Layout.fillWidth: true
                width: 510
                height: 150
                clip: true
                model: serverInfo

                property int selectedIndex: -1

                delegate: Item {
                    Layout.fillWidth: true
                    width: heading.width
                    height: 34
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.margins: 2
                        spacing: 10
                        Rectangle {
                            height: 30
                            width: 150
                            color: Colors.secondaryBackground
                            radius: 6
                            Text {
                                anchors.centerIn: parent
                                text: serverInformation.connectionName
                            }
                        }
                        Rectangle {
                            height: 30
                            width: 150
                            color: Colors.secondaryBackground
                            radius: 6
                            Text {
                                anchors.centerIn: parent
                                text: serverInformation.serverIp
                            }
                        }
                        Item {
                            height: 30
                            width: 150
                            TCToggleButton {
                                anchors.centerIn: parent
                                toggleEnabled: serverInformation.autoEnable
                            }
                        }
                        RadioButton {
                            id: radio
                            text: ""
                            Layout.alignment: Qt.AlignVCenter
                            checked: serverList.selectedIndex === index
                            onClicked: serverList.selectedIndex = index
                            indicator: Rectangle {
                                implicitWidth: 16
                                implicitHeight: 16
                                radius: width / 2
                                border.color: Colors.secondaryBackground
                                border.width: 2
                                color: radio.checked ? Colors.secondaryBackground : "transparent"

                                Rectangle {
                                    anchors.centerIn: parent
                                    width: parent.width / 2
                                    height: parent.height / 2
                                    radius: width / 2
                                    color: radio.checked ? Colors.surfaceBackground : "transparent"
                                }
                            }
                        }
                    }
                }
            }
            RowLayout {
                id: operations
                // anchors.top: serverList.bottom
                // anchors.topMargin: 20
                Layout.fillWidth: true
                spacing: 10
                TCButton {
                    buttonText: qsTr("Connect")
                }
                TCButton {
                    buttonText: qsTr("Edit")
                }
                TCButton {
                    buttonText: qsTr("Delete")
                }
            }
            TCButton {
                // anchors.bottom: parent.bottom
                // anchors.bottomMargin: 20
                // anchors.right: parent.right
                Layout.alignment: Qt.AlignRight
                buttonText: qsTr("Add RD Server")
                onSigMouseClicked: {
                    addNewServer = true
                }
            }
        }
    }
    Component {
        id: addNewServerComponent
        ColumnLayout {
            spacing: 20
            Layout.fillWidth: true
            Rectangle {
                id: displayHeader
                height: 40
                width: 250
                color: Colors.secondaryBackground
                radius: 6
                Text {
                    anchors.centerIn: parent
                    text: qsTr("Add RD Server")
                }
            }
            RowLayout {
                id: connectionName
                Layout.fillWidth: true
                spacing: 10
                Rectangle {
                    height: 30
                    width: 150
                    color: Colors.secondaryBackground
                    radius: 6
                    Text {
                        anchors.centerIn: parent
                        text: qsTr("Connection Name")
                    }
                }
                Rectangle {
                    height: 30
                    width: 200
                    color: Colors.secondaryBackground
                    radius: 6
                    TCTextField {
                        id: subnetMaskTextField
                    }
                }
            }
            RowLayout {
                id: heading
                Layout.fillWidth: true
                spacing: 10
                Rectangle {
                    height: 30
                    width: 150
                    color: Colors.secondaryBackground
                    radius: 6
                    Text {
                        anchors.centerIn: parent
                        text: qsTr("RD Server IP")
                    }
                }
                Rectangle {
                    height: 30
                    width: 200
                    color: Colors.secondaryBackground
                    radius: 6
                    TCTextField {
                        id: serverIPTextField
                    }
                }
            }
        }
    }
}
