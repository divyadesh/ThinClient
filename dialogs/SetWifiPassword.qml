import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

import App.Styles 1.0
import AppSecurity 1.0

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
            border.width: 1
            border.color: Colors.borderColor
        }

        header: Control {
            implicitHeight: 52
            padding: 10
            topPadding: 16

            contentItem: PrefsLabel {
                text: qsTr("Wifi Password")
                font.pixelSize: 24
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        contentItem: Control {
            padding: 20
            leftPadding: 50
            rightPadding: 50

            contentItem: RowLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

                PrefsLabel {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    text: qsTr("Enter Password")
                    font.pixelSize: 16
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }
                Item {
                    Layout.fillWidth: true
                }
                PrefsTextField {
                    id: enterPasswordField
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    horizontalAlignment: TextField.AlignLeft
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
                    text: qsTr("Ok")
                    radius: height / 2
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: {
                        wifiPassword = enterPasswordField.text
                        page.sigOkPressed()
                        pageStack.pop()
                    }
                }

                PrefsButton {
                    text: qsTr("Cancel")
                    highlighted: true
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
