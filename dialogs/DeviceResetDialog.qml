import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

import App.Styles 1.0
import App.Backend 1.0

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

    Connections {
        target: cApplication

        function onFactoryResetStarted() {
            showAlert(qsTr("Factory reset started..."), Type.Info);
        }

        function onFactoryResetFailed(reason) {
            progressLog.text += reason
            showAlert(reason, Type.Error);
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
                text: qsTr("Device Factory Reset")
                font.pixelSize: 24
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        contentItem: ColumnLayout {
            anchors.centerIn: parent
            spacing: 20

            Item {Layout.fillWidth: true}

            PrefsBusyIndicator {
                id: progressBar
                visible: cApplication.busy
                running: cApplication.busy
                Layout.alignment: Qt.AlignHCenter
            }

            PrefsLabel {
                id: progressLog
                text: qsTr("Are you sure you want to reset the device?")
                font.pixelSize: 16
                Layout.alignment: Qt.AlignHCenter
                horizontalAlignment: Text.AlignHCenter
            }

            Item {Layout.fillWidth: true}
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
                    highlighted: true
                    enabled: !cApplication.busy
                    radius: height / 2
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: pageStack.pop()
                }

                PrefsButton {
                    text: qsTr("Factory Reset")
                    radius: height / 2
                    enabled: !cApplication.busy
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: {
                        progressLog.text = qsTr("Factory reset started...")
                        showAlert(progressLog.text, Type.Info);
                        cApplication.factoryReset()
                    }
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }
    }
}
