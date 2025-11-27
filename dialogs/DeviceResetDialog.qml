import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

import App.Styles 1.0

import "../pages"
import "../components"
import "../controls"

/*
  resetManager.reboot()
  resetManager.shutdown()
  resetManager.factoryReset("/usr/bin/factory_reset.sh")
*/

BasicPage {
    id: control
    background: BackgroundOverlay {}

    // Show progress log
    Connections {
        target: cApplication

        function onResetStarted() {
            console.log("Reset started")
            progressLog.text = ""
            progressBar.visible = true
        }

        function onResetProgress(message) {
            progressLog.text += message + "\n"
        }

        function onResetFinished(success) {
            progressLog.text += (success ? "Reset completed.\n" : "Reset failed.\n")
            if(!success) {
                progressBar.visible = false
            }
        }

        function onResetRebooting() {
            progressLog.text += "Rebooting...\n"
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
                text: qsTr("Device Reset")
                font.pixelSize: 24
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        contentItem: Control {
            padding: 20

            contentItem: ColumnLayout {
                spacing: 20

                PrefsBusyIndicator {
                    id: progressBar
                    visible: false
                    running: true
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                PrefsLabel {
                    id: progressLog
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    text: qsTr("Are you are sure you want to reset the device ?")
                    font.pixelSize: 16
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
                    text: qsTr("No")
                    radius: height / 2
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: pageStack.pop()
                }

                PrefsButton {
                    text: qsTr("Shutdown Device")
                    highlighted: true
                    radius: height / 2
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: {
                        if (resetManager.shutdown()) {
                            console.log("Shutdown command executed")
                            pageStack.pop()
                        }else {
                            console.log("Failed to shutdown")
                        }
                    }
                }

                PrefsButton {
                    text: qsTr("Factory Reset")
                    highlighted: true
                    radius: height / 2
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: {
                        cApplication.resetAllAsync()
                    }
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }
    }
}
