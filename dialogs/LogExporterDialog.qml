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
    property bool isSuccess: false
    background: BackgroundOverlay {}

    UsbLogExportController {
        id: logExporter

        onProgressChanged: {
            // message
        }

        onSuccess: {
            isSuccess = true
            showAlert(message, Type.Success)
        }

        onError: {
            isSuccess = false
            showAlert(message, Type.Error)
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
                text: qsTr("Export Logs to USB")
                font.pixelSize: 24
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        contentItem: ColumnLayout {
            spacing: 10

            PrefsBusyIndicator {
                visible: logExporter.isBusy
                running: logExporter.isBusy
                radius: 10
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            PrefsLabel {
                id: statusText
                padding: 20
                text: logExporter.isBusy ? qsTr("Transferring log files to USB storage. Please wait...") : qsTr("Press the Export Logs button to start exporting logs.")
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
                    enabled: !logExporter.isBusy
                    visible: !isSuccess
                    radius: height / 2
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: {
                        pageStack.pop()
                    }
                }

                PrefsButton {
                    enabled: !logExporter.isBusy && usbMonitor.usbConnected
                    text: isSuccess ? qsTr("Close") : qsTr("Start Update")
                    radius: height / 2
                    highlighted: true
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: {
                        if(isSuccess) {
                            pageStack.pop()
                        }else {
                            logExporter.exportLogs()
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
