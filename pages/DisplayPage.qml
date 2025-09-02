import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0

import "../components"
import "../controls"

BasicPage {
    id: page
    padding: 20

    PrefsButton {
        id: displayHeader
        width: 250
        text: qsTr("Display")
    }

    Control {
        id: resolutionOrientation
        anchors.top: displayHeader.bottom
        anchors.topMargin: 40
        width: parent.width

        contentItem: ColumnLayout {
            spacing: 10

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                PrefsButton {
                    id: resolutionText
                    Layout.minimumWidth: 250
                    text: qsTr("Resolution")
                }

                TCComboBox {
                    model: ["Auto", "USB", "HDMI"]
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                PrefsButton {
                    id: orientationText
                    Layout.minimumWidth: 250
                    text: qsTr("Orientation")
                }

                TCComboBox {
                    model: ["Landscape", "Portrait", "Landscape (flipped)", "Portrait (flipped)"]
                }
            }
        }
    }

    PrefsButton {
        id: powerSaveRect
        width: 250
        text: qsTr("Power Save")
        anchors.top: resolutionOrientation.bottom
        anchors.topMargin: 50
    }

    ColumnLayout {
        anchors.top: powerSaveRect.bottom
        anchors.topMargin: 40
        Layout.fillWidth: true
        spacing: 10

        Repeater {
            model: [qsTr("Display Off"), qsTr("Device Off")]

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                PrefsButton {
                    Layout.alignment: Qt.AlignVCenter
                    Layout.minimumWidth: 250
                    text: qsTr("Power Save")
                }

                PrefsButton {
                    Layout.alignment: Qt.AlignVCenter
                    Layout.minimumWidth: 250
                    text: modelData
                }

                TCComboBox {
                    Layout.alignment: Qt.AlignVCenter
                    model: ["Jack", "USB", "HDMI"]
                }
            }
        }
    }
}
