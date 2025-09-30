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
    background: Rectangle {
        color: "#000000"
        opacity: 0.3
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
                text: qsTr("Device Reset")
                font.pixelSize: 24
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        contentItem: Control {
            padding: 20

            contentItem: PrefsLabel {
                text: qsTr("Are you are sure you want to reset the device ?")
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
                    text: qsTr("Yes")
                    radius: height / 2
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: {
                        if (resetManager.reboot()) {
                            console.log("Reboot command executed")
                            pageStack.pop()
                        }else {
                            console.log("Failed to reboot")
                        }
                    }
                }

                PrefsButton {
                    text: qsTr("No")
                    highlighted: true
                    radius: height / 2
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: pageStack.pop()
                }

                PrefsButton {
                    visible: false
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
                    visible: false
                    highlighted: true
                    radius: height / 2
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: {
                        if (resetManager.factoryReset("/usr/bin/factory_reset.sh")) {
                            console.log("Factory reset executed")
                            pageStack.pop()
                        }else {
                            console.log("Factory reset failed")
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
