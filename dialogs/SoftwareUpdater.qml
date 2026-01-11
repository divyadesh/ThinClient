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
    property bool isSuccess: false
    background: BackgroundOverlay {}

    ImageUpdater {
        id: updater

        onProgressChanged: function(step, percent) {
            statusText.text = step + " (" + percent + "%)";
            if(percent === 100 || parseInt(percent, 10) === 100) {
                pageStack.pop()
            }
        }

        onErrorOccurred: function(err) {
            statusText.text = "Error: " + err;
            showAlert(statusText.text, Type.Error);
        }

        onRebootCountdown: function(sec) {
            statusText.text = "Rebooting in " + sec + " seconds...";
            showAlert(statusText.text, Type.Success);
        }
    }

    Connections {
        target: cApplication

        function onPartialUpdateLog(log) {
            statusText.text = log
        }

        function onPartialUpdateStarted() {
            statusText.text  = "Starting update..."
            showAlert(statusText.text, Type.Info);
        }

        function onPartialUpdateSuccess() {
            isSuccess = true;
            statusText.text = "Update completed successfully."
            showAlert(statusText.text, Type.Success);
        }

        function onPartialUpdateFailed(reason) {
            isSuccess = false;
            statusText.text = "ERROR: " + reason
            showAlert(statusText.text, Type.Error);
        }
    }

    Page {
        anchors.centerIn: parent
        width: 600
        background: DialogBackground{}

        header: Control {
            height: 52
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
                visible: cApplication.busy
                running: cApplication.busy
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
                    enabled: !cApplication.busy
                    visible: !isSuccess
                    radius: height / 2
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: {
                        pageStack.pop()
                    }
                }

                PrefsButton {
                    enabled: !cApplication.busy && usbMonitor.usbConnected
                    text: isSuccess ? qsTr("Close") : qsTr("Start Update")
                    radius: height / 2
                    highlighted: true
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: {
                        if(isSuccess) {
                            pageStack.pop()
                        }else {
                            cApplication.partialUpdate()
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }
    }
}
