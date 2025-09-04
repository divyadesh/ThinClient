import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

import App.Styles 1.0

import "../pages"
import "../components"
import "../controls"

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
            border.color: "#3F3F46"
            border.width: 1
        }

        header: Control {
            implicitHeight: 52
            padding: 10

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
                    highlighted: true
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: pageStack.pop()
                }

                PrefsButton {
                    text: qsTr("No")
                    radius: height / 2
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: pageStack.pop()
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }
    }
}
