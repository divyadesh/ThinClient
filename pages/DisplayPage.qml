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
                            id: resolutionComboBox
                            x: resolution.width - width - resolution.rightPadding
                            y: resolution.topPadding + (resolution.availableHeight - height) / 2

                            property bool initialized: true

                            model: [
                                "Auto",
                                "1024 x 768",    // XGA
                                "1280 x 720",    // HD
                                "1366 x 768",    // HD+
                                "1440 x 900",    // WXGA+
                                "1600 x 900",    // HD+
                                "1680 x 1050",   // WSXGA+
                                "1920 x 1080",   // Full HD
                            ]
                            onCurrentIndexChanged: {
                                if(currentIndex > 0) {
                                    persistData.saveData("Resolution", model[currentIndex])
                                }
                            }
                            Component.onCompleted: {
                                let savedResolution = persistData.getData("Resolution")
                                if(savedResolution !== undefined) {
                                    let index = resolutionComboBox.find(savedResolution)
                                    if(index > 0)
                                        resolutionComboBox.currentIndex = index
                                }
                            }
                        }
                    }

                    PrefsItemDelegate {
                        id: orientation
                        Layout.fillWidth: true
                        text: qsTr("Orientation")

                        indicator: PrefsComboBox {
                            id: orientationComboBox
                            x: orientation.width - width - orientation.rightPadding
                            y: orientation.topPadding + (orientation.availableHeight - height) / 2

                            model: [
                                "Landscape",
                                "Portrait",
                                "Landscape (Flipped)",
                                "Portrait (Flipped)"
                            ]
                            onCurrentIndexChanged: {
                                if(currentIndex > 0) {
                                    persistData.saveData("Orientation", model[currentIndex])
                                }
                            }
                            Component.onCompleted: {
                                let savedOrientation = persistData.getData("Orientation")
                                if(savedOrientation !== undefined) {
                                    let index = orientationComboBox.find(savedOrientation)
                                    if(index > 0)
                                        orientationComboBox.currentIndex = index
                                }
                            }
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
                                    id: displayOffComboBox
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
                                        if(currentIndex > 0) {
                                            persistData.saveData("DisplayOff", model[currentIndex])
                                        }
                                    }
                                    Component.onCompleted: {
                                        let savedDisplayOff = persistData.getData("DisplayOff")
                                        if(savedDisplayOff !== undefined) {
                                            let index = displayOffComboBox.find(savedDisplayOff)
                                            if(index > 0)
                                                displayOffComboBox.currentIndex = index
                                        }
                                    }
                                }
                            }

                            ColumnLayout {
                                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                                PrefsLabel {
                                    text: qsTr("Device Off")
                                }

                                PrefsComboBox {
                                    id: deviceOffComboBox
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
                                        if(currentIndex > 0) {
                                            persistData.saveData("DeviceOff", model[currentIndex])
                                        }
                                    }
                                    Component.onCompleted: {
                                        let savedDeviceOff = persistData.getData("DeviceOff")
                                        if(savedDeviceOff !== undefined) {
                                            let index = deviceOffComboBox.find(savedDeviceOff)
                                            if(index > 0)
                                                deviceOffComboBox.currentIndex = index
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
