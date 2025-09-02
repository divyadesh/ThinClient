import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0
import "../controls"

BasicPage {
    id: page
    padding: 20

    Flickable {
        anchors.fill: parent
        contentWidth: page.width
        contentHeight: layout.height
        boundsBehavior: Flickable.StopAtBounds
        clip: true

        ColumnLayout {
            id: layout
            width: page.width
            spacing: 40

            PrefsButton {
                Layout.preferredWidth: 300
                highlighted: true
                text: qsTr("Device Info")
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 10

                PrefsLabel {
                    text: qsTr("Developed By : ") + qsTr("G1 Thin Client PC")
                }
                PrefsLabel {
                    text: qsTr("Model : ") + qsTr("G1 Rd Client")
                }
            }


            ColumnLayout {
                Layout.fillWidth: true
                spacing: 10

                PrefsLabel {
                    text: qsTr("CPU : ") + deviceInfo.cpuDetails
                }

                PrefsLabel {
                    text: qsTr("RAM : ") + (deviceInfo.ramSize != null ? deviceInfo.ramSize : "")
                }

                PrefsLabel {
                    text: qsTr("GPU : ") + qsTr("G1 Rd Client")
                }
            }


            ColumnLayout {
                Layout.fillWidth: true
                spacing: 10

                PrefsLabel {
                    text: qsTr("Ethernet : ") + qsTr("G1 Thin Client PC")
                }

                PrefsLabel {
                    text: qsTr("Wifi : ") + qsTr("G1 Rd Client")
                }
            }


            ColumnLayout {
                Layout.fillWidth: true
                spacing: 10

                PrefsLabel {
                    text: qsTr("Firmware Version : ") //+ qsTr("G1 Thin Client PC")
                }

                PrefsLabel {
                    text: qsTr("Made in Bharat")
                }
            }
        }
    }
}
