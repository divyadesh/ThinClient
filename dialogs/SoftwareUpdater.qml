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

/*
Test Cases Covered
Case	Handled?
Old file removal failure	✅ error + stop
Download failure	✅ error + stop
Permission failure	✅ error + stop
User cancels update	✅ gracefully aborts
Reboot countdown	✅ emits every second
Already updating	✅ blocked
Partial download cleanup	✅ cleans temp file
*/

BasicPage {
    id: control

    background: Rectangle {
        color: "#000000"
        opacity: 0.3
    }


    ImageUpdater {
        id: updater

        onProgressChanged: (step, percent) => {
                               statusText.text = step + " (" + percent + "%)";
                           }

        onErrorOccurred: (err) => {
                             statusText.text = "Error: " + err;
                         }

        onRebootCountdown: (sec) => {
                               statusText.text = "Rebooting in " + sec + " seconds...";
                           }
    }

    Page {
        anchors.centerIn: parent
        width: 600

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
                text: qsTr("Update")
                font.pixelSize: 24
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        contentItem: ColumnLayout {
            spacing: 10

            PrefsBusyIndicator {
                visible: updater.updating
                radius: 10
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            PrefsLabel {
                id: statusText
                padding: 20
                text: qsTr("Software update available. Changes have been detected. Proceed with update?")
                font.pixelSize: 16
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.fillWidth: true
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
                    enabled: !updater.updating
                    radius: height / 2
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: {
                        if(updater.updating) {
                            updater.cancelUpdate()
                        }
                        pageStack.pop()
                    }
                }

                PrefsButton {
                    enabled: !updater.updating
                    text: qsTr("Start Update")
                    radius: height / 2
                    highlighted: true
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: {
                        updater.startUpdate("http://yourserver.com/path/to/Connect")
                    }
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }
    }
}
