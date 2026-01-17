/*
Title: Update Password
Message: "Would you like to set a new password or remove the existing one?"
Buttons: Cancel | Set Password | Remove Password
*/

import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

import App.Styles 1.0
import App.Backend 1.0

import "../pages"
import "../components"
import "../controls"

BasicPage {
    id: control
    property string connectionId: ""
    property string autoConnectionName: ""
    property string autoConnectionIp: ""

    signal cancelled()

    background: BackgroundOverlay {}

    Component.onCompleted: {
        autoConnectTimer.start()
    }

    Shortcut {
        sequence: StandardKey.Cancel   // ESC
        onActivated: {
            // handle ESC here
            control.closeDialog()
        }
    }

    function closeDialog() {
        autoConnectTimer.stop()
        cancelled()
        pageStack.pop()
    }

    Timer {
        id: autoConnectTimer
        interval: 5000
        running: false
        repeat: false
        onTriggered: {
            serverInfo.connectRdServer(control.connectionId)
        }
    }

    Page {
        anchors.centerIn: parent
        width: 480
        background: DialogBackground{}

        header: Control {
            implicitHeight: 52
            padding: 10
            topPadding: 16

            contentItem: PrefsLabel {
                text: qsTr("RD Server")
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
                spacing: 16

                // ---- Connection Name ----
                PrefsLabel {
                    Layout.fillWidth: true
                    text: qsTr("Connection Name:")
                    font.pixelSize: 12
                    color: Colors.textSecondary
                    horizontalAlignment: Text.AlignHCenter
                }

                PrefsLabel {
                    Layout.fillWidth: true
                    text: control.autoConnectionName
                    font.pixelSize: 16
                    font.bold: true
                    color: Colors.textPrimary
                    horizontalAlignment: Text.AlignHCenter
                    visible: control.autoConnectionName.length > 0
                }

                // ---- Server IP ----
                PrefsLabel {
                    Layout.fillWidth: true
                    text: qsTr("Server IP:")
                    font.pixelSize: 12
                    color: Colors.textSecondary
                    horizontalAlignment: Text.AlignHCenter
                }

                PrefsLabel {
                    Layout.fillWidth: true
                    text: control.autoConnectionIp
                    font.pixelSize: 14
                    color: Colors.textPrimary
                    horizontalAlignment: Text.AlignHCenter
                    visible: control.autoConnectionIp.length > 0
                }

                // ---- Auto connect text ----
                PrefsLabel {
                    Layout.fillWidth: true
                    font.pixelSize: 12
                    text: qsTr("Auto-connecting to RD Server…")
                    color: Colors.textSecondary
                    horizontalAlignment: Text.AlignHCenter
                }

                // ---- Busy indicator ----
                PrefsBusyIndicator {
                    visible: true
                    radius: 10
                    Layout.alignment: Qt.AlignHCenter
                }

                // ---- Error text ----
                PrefsLabel {
                    id: errorText
                    Layout.fillWidth: true
                    font.pixelSize: 12
                    visible: !!text
                    color: Colors.statusError
                    horizontalAlignment: Text.AlignHCenter
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
                        control.closeDialog()
                    }
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }
    }
}
