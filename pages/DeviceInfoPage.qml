import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0

import "../controls"
import "../components"

/*
        Text { text: "Developed By: " + deviceInfoSettings.developedBy }
        Text { text: "Model: " + deviceInfoSettings.model }
        Text { text: "CPU: " + deviceInfoSettings.cpu }
        Text { text: "RAM: " + deviceInfoSettings.ram }
        Text { text: "GPU: " + deviceInfoSettings.gpu }
        Text { text: "Ethernet: " + deviceInfoSettings.ethernet }
        Text { text: "Wifi: " + deviceInfoSettings.wifi }
        Text { text: "Firmware version: " + deviceInfoSettings.firmwareVersion }
        Text { text: "Made in " + deviceInfoSettings.madeIn }
*/
BasicPage {
    id: page
    padding: 20

    header: PageHeader {
        pageTitle: page.pageTitle
        onBackPressed: {
            if(pageStack.depth == 1) {
                backToHome()
                return
            }
            pageStack.pop()
        }
    }

    component  PrefsControlLabel: Control {
        id: _control
        property string text: ""

        contentItem: PrefsLabel {
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            text: _control.text
            font.pixelSize: 20
        }
    }

    // Replace the empty Item with a Column for vertical arrangement
    contentItem: ColumnLayout {
        spacing: 40

        // Device Information

        ColumnLayout {
            Layout.topMargin: 20
            Layout.fillWidth: true

            RowLayout {
                spacing: 20
                Layout.fillWidth: true

                PrefsControlLabel {
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    Layout.minimumWidth: firmwareLabel.implicitWidth
                    text: "Developed By :"
                }

                PrefsControlLabel {
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    Layout.fillWidth: true
                    text: deviceInfoSettings.developedBy
                }
            }

            RowLayout {
                spacing: 20
                Layout.fillWidth: true

                PrefsControlLabel {
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    Layout.minimumWidth: firmwareLabel.implicitWidth
                    text: "Model :"
                }

                PrefsControlLabel {
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    Layout.fillWidth: true
                    text: deviceInfoSettings.model
                }
            }
        }
        // Hardware Specs

        ColumnLayout {
            Layout.fillWidth: true
            RowLayout {
                spacing: 20
                Layout.fillWidth: true

                PrefsControlLabel {
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    Layout.minimumWidth: firmwareLabel.implicitWidth
                    text: "CPU :"
                }

                PrefsControlLabel {
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    Layout.fillWidth: true
                    text: deviceInfoSettings.cpu
                }
            }

            RowLayout {
                spacing: 20
                Layout.fillWidth: true

                PrefsControlLabel {
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    Layout.minimumWidth: firmwareLabel.implicitWidth
                    text: "RAM :"
                }

                PrefsControlLabel {
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    Layout.fillWidth: true
                    text: deviceInfoSettings.ram
                }
            }

            RowLayout {
                spacing: 20
                Layout.fillWidth: true

                PrefsControlLabel {
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    Layout.minimumWidth: firmwareLabel.implicitWidth
                    text: "GPU :"
                }

                PrefsControlLabel {
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    Layout.fillWidth: true
                    text: deviceInfoSettings.gpu
                }
            }
        }
        // Network Info

        ColumnLayout {
            Layout.fillWidth: true
            RowLayout {
                spacing: 20
                Layout.fillWidth: true

                PrefsControlLabel {
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    Layout.minimumWidth: firmwareLabel.implicitWidth
                    text: "Ethernet :"
                }

                PrefsControlLabel {
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    Layout.fillWidth: true
                    text: deviceInfoSettings.ethernet
                }
            }

            RowLayout {
                spacing: 20
                Layout.fillWidth: true

                PrefsControlLabel {
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    Layout.minimumWidth: firmwareLabel.implicitWidth
                    text: "Wifi :"
                }

                PrefsControlLabel {
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    Layout.fillWidth: true
                    text: deviceInfoSettings.wifi
                }
            }
        }
        // Firmware

        ColumnLayout {
            Layout.fillWidth: true
            RowLayout {
                spacing: 20
                Layout.fillWidth: true

                PrefsControlLabel {
                    id: firmwareLabel
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    text: "Firmware version :"
                }

                PrefsControlLabel {
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    Layout.fillWidth: true
                    text: deviceInfoSettings.firmwareVersion
                }
            }

            PrefsControlLabel {
                Layout.fillWidth: true
                text: "Made in %1".arg(deviceInfoSettings.madeIn)
            }
        }

        Item { Layout.fillHeight: true }
    }
}
