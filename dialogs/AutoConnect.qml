/*
Title: Update Password
Message: "Would you like to set a new password or remove the existing one?"
Buttons: Cancel | Set Password | Remove Password
*/

import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

import App.Styles 1.0

import "../pages"
import "../components"
import "../controls"

BasicPage {
    id: control
    property string ipServer: ""
    property string connectionName: ""

    signal cancelled()

    background: Rectangle {
        color: "#000000"
        opacity: 0.3
    }

    Component.onCompleted: {
        autoConnectTimer.start()
    }

    Timer {
        id: autoConnectTimer
        interval: 5000
        running: false
        repeat: false
        onTriggered: {
            serverInfo.connectRdServer(control.ipServer, control.connectionName)
        }
    }

    Page {
        anchors.centerIn: parent
        width: 480

        background: Rectangle {
            radius: 8
            color: Colors.steelGray
            border.width: 1
            border.color: Colors.borderColor
        }

        header: Control {
            implicitHeight: 52
            padding: 10
            topPadding: 16

            contentItem: PrefsLabel {
                text: qsTr("Auto Connect RD Server")
                font.pixelSize: 24
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        contentItem: Control {
            padding: 20
            leftPadding: 50
            rightPadding: 50

            contentItem: ColumnLayout {
                spacing: 20

                PrefsLabel {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    font.pixelSize: 12
                    text: qsTr("Auto Connecting RD Server ...")
                    visible: !!text
                    color: Colors.textPrimary
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                PrefsBusyIndicator {
                    visible: true
                    radius: 10
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                PrefsLabel {
                    id: errorText
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    font.pixelSize: 12
                    visible: !!text
                    color: Colors.statusError
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }


        footer: Control {
            implicitHeight: 72

            contentItem: RowLayout {
                spacing: 20

                Item {
                    Layout.fillWidth: true
                }

                PrefsButton {
                    text: qsTr("Cancel")
                    radius: height / 2
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: {
                        autoConnectTimer.stop()
                        cancelled()
                        pageStack.pop()
                    }
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }
    }
}
