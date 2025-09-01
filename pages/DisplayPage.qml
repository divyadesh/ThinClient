import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0

import "../components"

BasicPage {
    padding: 20
    Rectangle {
        id: displayHeader
        height: 40
        width: 250
        color: Colors.secondaryBackground
        radius: 6
        Text {
            anchors.centerIn: parent
            text: qsTr("Display")
        }
    }
    ColumnLayout {
        id: resolutionOrientation
        anchors.top: displayHeader.bottom
        anchors.topMargin: 40
        Layout.fillWidth: true
        spacing: 10
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            Rectangle {
                id: resolutionText
                height: 30
                width: 250
                color: Colors.secondaryBackground
                radius: 6
                Text {
                    anchors.centerIn: parent
                    text: qsTr("Resolution")
                }
            }
            Item {
                width: 200
                height: 30
                clip:false
                TCComboBox {
                    anchors.fill: parent
                    model: ["Auto", "USB", "HDMI"]
                }
            }
        }
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            Rectangle {
                id: orientationText
                height: 30
                width: 250
                color: Colors.secondaryBackground
                radius: 6
                Text {
                    anchors.centerIn: parent
                    text: qsTr("Orientation")
                }
            }
            Item {
                width: 200
                height: 30
                clip:false
                TCComboBox {
                    anchors.fill: parent
                    model: ["Landscape", "Portrait", "Landscape (flipped)", "Portrait (flipped)"]
                }
            }
        }
    }
    Rectangle {
        id: powerSaveRect
        height: 40
        width: 250
        anchors.top: resolutionOrientation.bottom
        anchors.topMargin: 50
        color: Colors.secondaryBackground
        radius: 6
        Text {
            anchors.centerIn: parent
            text: qsTr("Power Save")
        }
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
                Rectangle {
                    height: 30
                    width: 250
                    color: Colors.secondaryBackground
                    radius: 6
                    Text {
                        anchors.centerIn: parent
                        text: modelData//qsTr("Device Off")
                    }
                }
                Item {
                    width: 200
                    height: 30
                    clip:false
                    TCComboBox {
                        anchors.fill: parent
                        model: ["Jack", "USB", "HDMI"]
                    }
                }
            }
        }
    }
}
