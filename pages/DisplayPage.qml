import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0

import "../components"
import "../controls"

BasicPage {
    id: page
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
            spacing: 20
            clip: true

            Control {
                Layout.fillWidth: true
                padding: 20

                background: Rectangle {
                    color: Colors.btnBg
                    radius: 8
                }

                contentItem: ColumnLayout {
                    spacing: 20

                    PrefsItemDelegate {
                        id: resolution
                        Layout.fillWidth: true
                        text: qsTr("Resolution")

                        indicator: PrefsComboBox {
                            x: resolution.width - width - resolution.rightPadding
                            y: resolution.topPadding + (resolution.availableHeight - height) / 2

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
                        id: orientation
                        Layout.fillWidth: true
                        text: qsTr("Orientation")

                        indicator: PrefsComboBox {
                            x: orientation.width - width - orientation.rightPadding
                            y: orientation.topPadding + (orientation.availableHeight - height) / 2

                            model: [
                                "Auto",
                                "Landscape",
                                "Portrait",
                                "Landscape (Flipped)",
                                "Portrait (Flipped)"
                            ]
                        }
                    }

                    PrefsItemDelegate {
                        id: powerSave
                        Layout.fillWidth: true
                        text: qsTr("Power Save")
                        indicator: RowLayout {
                            x: powerSave.width - width - powerSave.rightPadding
                            y: powerSave.topPadding + (powerSave.availableHeight - height) / 2
                            spacing: 20

                            ColumnLayout {
                                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                                PrefsLabel {
                                    text: qsTr("Display Off")
                                }

                                PrefsComboBox {
                                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                                    model: [
                                        "None",
                                        "10 Minutes",
                                        "30 Minutes",
                                        "60 Minutes"
                                    ]

                                    // Default selected = "None"
                                    currentIndex: 0

                                    onCurrentIndexChanged: {
                                        switch (currentIndex) {
                                        case 0: console.log("Display Off: None"); break;
                                        case 1: console.log("Display Off: 10 min"); break;
                                        case 2: console.log("Display Off: 30 min"); break;
                                        case 3: console.log("Display Off: 60 min"); break;
                                        }
                                    }
                                }
                            }

                            ColumnLayout {
                                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                                PrefsLabel {
                                    text: qsTr("Display Off")
                                }

                                PrefsComboBox {
                                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                                    model: [
                                        "None",
                                        "1 Hour",
                                        "5 Hours",
                                        "10 Hours",
                                        "24 Hours",
                                        "48 Hours"
                                    ]
                                    // Default selected = "None"
                                    currentIndex: 0

                                    onCurrentIndexChanged: {
                                        switch (currentIndex) {
                                        case 0: console.log("Device Off: None"); break;
                                        case 1: console.log("Device Off: 1 hour"); break;
                                        case 2: console.log("Device Off: 5 hours"); break;
                                        case 3: console.log("Device Off: 10 hours"); break;
                                        case 4: console.log("Device Off: 24 hours"); break;
                                        case 5: console.log("Device Off: 48 hours"); break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
