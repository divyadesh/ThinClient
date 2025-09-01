import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0

BasicPage {
    id: deviceInfoPage
    padding: 20
    Rectangle {
        id: deviceInfoHeader
        height: 40
        width: 250
        color: Colors.secondaryBackground
        radius: 6
        Text {
            anchors.centerIn: parent
            text: qsTr("Device Info")
        }
    }
    ColumnLayout {
        anchors.top: deviceInfoHeader.bottom
        anchors.topMargin: 40
        spacing: 40
        ColumnLayout {
            spacing: 10
            Text {
                text: qsTr("Developed By : ") + qsTr("G1 Thin Client PC")
            }
            Text {
                text: qsTr("Model : ") + qsTr("G1 Rd Client")
            }
        }
        ColumnLayout {
            spacing: 10
            Text {
                text: qsTr("CPU : ") + deviceInfo.cpuDetails
            }
            Text {
                text: qsTr("RAM : ") + (deviceInfo.ramSize != null ? deviceInfo.ramSize : "")
            }
            Text {
                text: qsTr("GPU : ") + qsTr("G1 Rd Client")
            }
        }
        ColumnLayout {
            spacing: 10
            Text {
                text: qsTr("Ethernet : ") + qsTr("G1 Thin Client PC")
            }
            Text {
                text: qsTr("Wifi : ") + qsTr("G1 Rd Client")
            }
        }
        ColumnLayout {
            spacing: 10
            Text {
                text: qsTr("Firmware Version : ") //+ qsTr("G1 Thin Client PC")
            }
            Text {
                text: qsTr("Made in Bharat")
            }
        }
    }
}
