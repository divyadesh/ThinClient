import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0

import "../components"
import "../controls"
import "../dialogs"

BasicPage {
    id: page
    StackView.visible: true
    padding: 20
    property bool addNewServer: false

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

    contentItem: Flickable {
        width: parent.width
        clip: true
        contentHeight: layout.height
        contentWidth: layout.width

        ColumnLayout {
            id: layout
            width: page.width - 40
            spacing: 40
            clip: true


            Control {
                Layout.fillWidth: true
                padding: 20

                background: Rectangle {
                    color: Colors.btnBg
                    radius: 8
                }

                contentItem: ColumnLayout {
                    spacing: 10

                    PrefsLabel {
                        font.pixelSize: 24
                        text: qsTr("Device Settings")
                    }

                    Item {Layout.fillWidth: true}

                    PrefsItemDelegate {
                        id: audio
                        Layout.fillWidth: true
                        text: qsTr("Audio")

                        ButtonGroup { id: tabGroup }

                        indicator: RowLayout {
                            x: audio.width - width - audio.rightPadding
                            y: audio.topPadding + (audio.availableHeight - height) / 2

                            spacing: 20

                            PrefsTabButton {
                                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                                ButtonGroup.group: tabGroup
                                checked: true
                                text: "Jack"
                                visible: !!text
                                font.weight: Font.Normal
                            }

                            PrefsTabButton {
                                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                                ButtonGroup.group: tabGroup
                                text: "USB"
                                visible: !!text
                                font.weight: Font.Normal
                            }

                            PrefsTabButton {
                                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                                ButtonGroup.group: tabGroup
                                text: "HDMI"
                                visible: !!text
                                font.weight: Font.Normal
                            }
                        }
                    }

                    PrefsItemDelegate {
                        id: connection
                        Layout.fillWidth: true
                        text: qsTr("Device Name")

                        indicator: PrefsTextField {
                            id: connectionField
                            x: connection.width - width - connection.rightPadding
                            y: connection.topPadding + (connection.availableHeight - height) / 2

                            placeholderText : qsTr("Enter %1").arg(connection.text)
                        }
                    }

                    PrefsItemDelegate {
                        id: timezone
                        Layout.fillWidth: true
                        text: qsTr("Time Zone")

                        indicator: PrefsComboBox {
                            x: timezone.width - width - timezone.rightPadding
                            y: timezone.topPadding + (timezone.availableHeight - height) / 2

                            model: [
                                "Auto",
                                "640 x 480",     // VGA
                                "800 x 600",     // SVGA
                                "1024 x 768",    // XGA
                                "1280 x 720",    // HD
                                "1280 x 800",    // WXGA
                                "1366 x 768",    // HD+
                                "1440 x 900",    // WXGA+
                                "1600 x 900",    // HD+
                                "1600 x 1200",   // UXGA
                                "1680 x 1050",   // WSXGA+
                                "1920 x 1080",   // Full HD
                                "1920 x 1200",   // WUXGA
                                "2048 x 1152",   // 2K-ish
                                "2560 x 1080",   // UltraWide FHD
                                "2560 x 1440",   // QHD
                                "2560 x 1600",   // WQXGA
                                "3440 x 1440",   // UltraWide QHD
                                "3840 x 1600",   // UltraWide 4K-ish
                                "3840 x 2160"    // 4K UHD
                            ]
                        }
                    }

                    PrefsItemDelegate {
                        id: language
                        Layout.fillWidth: true
                        text: qsTr("Language")

                        indicator: PrefsComboBox {
                            x: language.width - width - language.rightPadding
                            y: language.topPadding + (language.availableHeight - height) / 2

                            model: [
                                "Device Language",
                                "English",
                                "Hindi"
                            ]
                        }
                    }


                    Item {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 5
                    }
                }
            }

            Control {
                Layout.fillWidth: true
                padding: 20

                background: Rectangle {
                    color: Colors.steelGray
                    radius: 8
                }

                contentItem: ColumnLayout {
                    spacing: 10

                    PrefsLabel {
                        font.pixelSize: 24
                        text: qsTr("Device Restore & Upgrade")
                    }

                    Item {Layout.fillWidth: true}

                    RowLayout {
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        Layout.fillWidth: true
                        spacing: 150

                        PrefsButton {
                            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                            Layout.fillWidth: true
                            radius: height / 2
                            text: qsTr("Reset")
                            onClicked: pageStack.push(deviceReset)
                        }

                        PrefsButton {
                            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                            Layout.fillWidth: true
                            radius: height / 2
                            text: qsTr("Update")
                        }

                        PrefsButton {
                            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                            Layout.fillWidth: true
                            radius: height / 2
                            text: qsTr("Export Log")
                        }
                    }
                }
            }
        }
    }

    Component {
        id: deviceReset
        DeviceResetDialog {}
    }
}
