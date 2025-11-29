/*
Title: Update Password
Message: "Would you like to set a new password or remove the existing one?"
Buttons: Cancel | Set Password | Remove Password
*/

import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import AppSecurity 1.0
import App.Styles 1.0
import App.Backend 1.0

import "../pages"
import "../components"
import "../controls"

BasicPage {
    id: control
    signal setPassword()
    signal removePassword()

    background: BackgroundOverlay {}

    UnlockManager {
        id: unlockManager
    }

    Page {
        anchors.centerIn: parent
        width: 600
        background: DialogBackground{}

        header: Control {
            implicitHeight: 52
            padding: 10
            topPadding: 16

            contentItem: PrefsLabel {
                text: qsTr("Update Password")
                font.pixelSize: 24
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        contentItem: Control {
            padding: 20

            contentItem: PrefsLabel {
                text: qsTr("Would you like to set a new password or remove the existing one?")
                font.pixelSize: 16
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
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
                    onClicked: pageStack.pop()
                }

                PrefsButton {
                    text: unlockManager.hasPassword ? qsTr("Update Password") : qsTr("Set Password")
                    radius: height / 2
                    highlighted: true
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: {
                        setPassword()
                    }
                }

                PrefsDangerButton {
                    visible: unlockManager.hasPassword
                    text: qsTr("Remove Password")
                    radius: height / 2
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

                    onClicked: {removePassword()}
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }
    }
}
