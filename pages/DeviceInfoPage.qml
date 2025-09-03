import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0

import "../controls"
import "../components"

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
            font.pixelSize: 24
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
                    text: "G1 Thin Client Pc"
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
                    text: "G1 Rd Client"
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
                    text: "ARM Quad-core @ 1.5 Ghz"
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
                    text: "2 GB"
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
                    text: "Mali 450 | H264/AVC444 Decoding"
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
                    text: "1 Gbps"
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
                    text: "Built in 2.4 GHz"
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
                    text: "G1RD250105"
                }
            }

            PrefsControlLabel {
                Layout.fillWidth: true
                text: "Made in Bharat"
            }
        }

        Item { Layout.fillHeight: true }
    }
}
