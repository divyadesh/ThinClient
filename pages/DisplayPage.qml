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
        contentWidth: parent.width
        ColumnLayout {
            id: layout
            width: page.width - 40
            spacing: 40
            clip: true

            Control {
                Layout.fillWidth: true
                padding: 20

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

                            // ✅ Updated model: each item has a label (text) and numeric value
                            model: [
                                { text: "Landscape", value: 0 },
                                { text: "Portrait", value: 90 },
                                { text: "Landscape (Flipped)", value: 180 },
                                { text: "Portrait (Flipped)", value: 270 }
                            ]

                            // ✅ Tell ComboBox which property to display
                            textRole: "text"
                            valueRole: "value"

                            // ✅ Save numeric value when changed
                            onActivated: {
                                persistData.saveData("Orientation", currentValue)
                            }

                            // ✅ Restore saved numeric orientation
                            Component.onCompleted: {
                                let savedOrientation = persistData.getData("Orientation")
                                if (savedOrientation !== undefined) {
                                    let index = model.findIndex(item => item.value === parseInt(savedOrientation, 10))
                                    if (index >= 0) {
                                        orientationComboBox.currentIndex = index
                                    }
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

                                    // ✅ Updated model: each item has a label (text) and numeric value (in minutes)
                                    model: [
                                        { text: "None", value: 0 },
                                        { text: "10 Minutes", value: 10 },
                                        { text: "30 Minutes", value: 30 },
                                        { text: "60 Minutes", value: 60 }
                                    ]

                                    // ✅ Tell ComboBox which roles to use
                                    textRole: "text"
                                    valueRole: "value"

                                    // Default selection = "None"
                                    currentIndex: 0

                                    // ✅ When user changes the selection
                                    onActivated: {
                                        let selectedValue = model[currentIndex].value
                                        console.log("Display Off:", selectedValue === 0 ? "None" : `${selectedValue} min`)
                                        persistData.saveData("DisplayOff", selectedValue)
                                    }

                                    // ✅ Restore saved setting
                                    Component.onCompleted: {
                                        let savedDisplayOff = persistData.getData("DisplayOff")
                                        console.log("Restored DisplayOff:", savedDisplayOff)
                                        if (savedDisplayOff !== undefined) {
                                            // 🔍 Find matching index by numeric value
                                            let index = model.findIndex(item => item.value === parseInt(savedDisplayOff, 10))
                                            if (index >= 0) {
                                                displayOffComboBox.currentIndex = index
                                            }
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

                                    // ✅ Each item has a display label (text) and numeric value (in hours)
                                    model: [
                                        { text: "None", value: 0 },
                                        { text: "1 Hour", value: 1 },
                                        { text: "5 Hours", value: 5 },
                                        { text: "10 Hours", value: 10 },
                                        { text: "24 Hours", value: 24 },
                                        { text: "48 Hours", value: 48 }
                                    ]

                                    textRole: "text"
                                    valueRole: "value"

                                    // Default = "None"
                                    currentIndex: 0

                                    // ✅ Handle user change
                                    onActivated: {
                                        let selectedValue = model[currentIndex].value
                                        console.log("Device Off:", selectedValue === 0 ? "None" : `${selectedValue} hour(s)`)
                                        persistData.saveData("DeviceOff", selectedValue)
                                    }

                                    // ✅ Restore saved value
                                    Component.onCompleted: {
                                        let savedDeviceOff = persistData.getData("DeviceOff")
                                        console.log("Restored DeviceOff:", savedDeviceOff)
                                        if (savedDeviceOff !== undefined) {
                                            // Find index based on stored numeric value
                                            let index = model.findIndex(item => item.value === parseInt(savedDeviceOff, 10))
                                            if (index >= 0) {
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
}
