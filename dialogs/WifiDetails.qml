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

    property QtObject wifiDetail: null

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
                text: qsTr("Wifi Details")
                font.pixelSize: 24
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        contentItem: Control {
            padding: 20

            contentItem: ColumnLayout {
                Repeater {
                    model: [
                        { key: "Ssid", value: wifiDetail !== null ? wifiDetail.ssid: "" },
                        { key: "Bssid", value: wifiDetail !== null ? wifiDetail.bssid: "" },
                        { key: "Chan",  value: wifiDetail !== null ? wifiDetail.chan: "" },
                        { key: "Rate",  value: wifiDetail !== null ? wifiDetail.rate: "" }
                    ]
                    RowLayout {
                        PrefsLabel {
                            text: modelData.key
                            font.pixelSize: 16
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        Item {
                            Layout.fillWidth: true
                        }
                        PrefsLabel {
                            text: modelData.value
                            font.pixelSize: 16
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
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
                    text: qsTr("Close")
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
