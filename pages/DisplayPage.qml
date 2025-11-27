import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0
import App.Backend 1.0

import "../components"
import "../controls"

BasicPage {
    id: page
    padding: 20

    DisplaySettings {
        id: backend
    }

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

    ListModel {
        id: orientationModel

        ListElement { text: "Landscape";              value: 0;   apply: "normal" }
        ListElement { text: "Portrait";               value: 90;  apply: "rotate-90" }
        ListElement { text: "Landscape (Flipped)";    value: 180; apply: "rotate-180" }
        ListElement { text: "Portrait (Flipped)";     value: 270; apply: "rotate-270" }
    }

    ListModel {
        id: displayOffModel

        ListElement { text: "None";         value: 0 }
        ListElement { text: "10 Minutes";   value: 10 }
        ListElement { text: "30 Minutes";   value: 30 }
        ListElement { text: "60 Minutes";   value: 60 }
    }

    ListModel {
        id: devicePowerOffModel

        ListElement { text: "None";      value: 0 }
        ListElement { text: "1 Hour";    value: 1 }
        ListElement { text: "5 Hours";   value: 5 }
        ListElement { text: "10 Hours";  value: 10 }
        ListElement { text: "24 Hours";  value: 24 }
        ListElement { text: "48 Hours";  value: 48 }
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

                            model: resolutionListModel
                            textRole: "text"
                            valueRole: "value"

                            onActivated: {
                                logSelection("Resolution", currentIndex, resolutionListModel)
                            }

                            Component.onCompleted: {
                                for (var i = 0; i < resolutionComboBox.count; i++) {
                                    if (resolutionListModel.get(i).value === backend.resolution) {
                                        resolutionComboBox.currentIndex = i
                                        break
                                    }
                                }
                            }
                        }
                    }

                    PrefsItemDelegate {
                        id: addCustomResolution
                        Layout.fillWidth: true
                        implicitHeight: resolution.implicitHeight
                        text: qsTr("Add Custom Resolution")

                        indicator: Image {
                            x: addCustomResolution.width - width - addCustomResolution.rightPadding
                            y: addCustomResolution.topPadding + (addCustomResolution.availableHeight - height) / 2
                            source: "qrc:/assets/icons/ic_aux.svg"
                            sourceSize: Qt.size(24, 24)
                        }

                        onClicked: {
                            const w = resolutionListModel.get(resolutionComboBox.currentIndex).width
                            const h = resolutionListModel.get(resolutionComboBox.currentIndex).height

                            pageStack.push(addCustomResolutionPage, {resolutionWidth: w, resolutionHeight: h});
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

                            model: orientationModel

                            textRole: "text"
                            valueRole: "value"

                            onActivated: {
                                logSelection("Orientation", currentIndex, orientationModel)
                            }

                            Component.onCompleted: {
                                for (var i = 0; i < orientationModel.count; i++) {
                                    if (orientationModel.get(i).apply === backend.orientation) {
                                        orientationComboBox.currentIndex = i
                                        break
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
                                    model: displayOffModel
                                    textRole: "text"
                                    valueRole: "value"

                                    onActivated: {
                                        logSelection("Display-Off", currentIndex, displayOffModel)
                                    }

                                    Component.onCompleted: {
                                        const previous_minutes = backend.idleTimeSeconds / 60
                                        for (let i = 0; i < displayOffModel.count; i++) {
                                            let minutes = displayOffModel.get(i).value
                                            if (minutes === previous_minutes) {
                                                displayOffComboBox.currentIndex = i
                                                break
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
                                    model: devicePowerOffModel
                                    textRole: "text"
                                    valueRole: "value"

                                    onActivated: {
                                        logSelection("Device-Off", currentIndex, devicePowerOffModel)
                                    }

                                    Component.onCompleted: {
                                        const hours = persistData.deviceOff / (60 * 60 * 1000)
                                        for (var i = 0; i < devicePowerOffModel.count; i++) {
                                            if (devicePowerOffModel.get(i).value === hours) {
                                                deviceOffComboBox.currentIndex = i
                                                break
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // --- Enable On Screen Keyboard ---
                    PrefsItemDelegate {
                        id: enableOnScreenKeyboard
                        Layout.fillWidth: true
                        text: qsTr("Enable On Screen Keyboard")

                        indicator: RowLayout {
                            x: enableOnScreenKeyboard.width - width - enableOnScreenKeyboard.rightPadding
                            y: enableOnScreenKeyboard.topPadding + (enableOnScreenKeyboard.availableHeight - height) / 2

                            PrefsButton {
                                checkable: true
                                implicitWidth: 260
                                text: qsTr("Enable")
                                font.weight: Font.Normal
                                checked: persistData.enableOnScreenKeyboard

                                onClicked: {
                                    persistData.enableOnScreenKeyboard = checked
                                }
                            }
                        }
                    }

                    // --- Enable Touch Screen ---
                    PrefsItemDelegate {
                        id: enableTouchScreen
                        Layout.fillWidth: true
                        text: qsTr("Enable Touch Screen")

                        indicator: RowLayout {
                            x: enableTouchScreen.width - width - enableTouchScreen.rightPadding
                            y: enableTouchScreen.topPadding + (enableTouchScreen.availableHeight - height) / 2

                            PrefsButton {
                                id: enableTouch
                                checkable: true
                                implicitWidth: 260
                                text: qsTr("Enable")
                                font.weight: Font.Normal
                                checked: persistData.enableTouchScreen
                            }
                        }
                    }

                    PrefsItemDelegate {
                        id: saveButton
                        Layout.fillWidth: true
                        rightPadding: 0
                        leftPadding: 0

                        background: Item {
                            implicitWidth: 100
                            implicitHeight: 40
                        }

                        indicator: PrefsButton {
                            id: saveBtn
                            x: saveButton.width - width - saveButton.rightPadding
                            y: saveButton.topPadding + (saveButton.availableHeight - height) / 2
                            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                            text: qsTr("Save")
                            highlighted: true
                            onClicked: {

                                console.log("=== APPLY SETTINGS START ===")

                                /* -----------------------------
                                   RESOLUTION
                                ----------------------------- */
                                const resolution = resolutionListModel.get(resolutionComboBox.currentIndex).value
                                backend.resolution = resolution
                                persistData.saveData("Resolution", resolution)
                                console.log("Resolution:", resolution)

                                /* -----------------------------
                                   ORIENTATION
                                ----------------------------- */
                                const orientation = orientationModel.get(orientationComboBox.currentIndex).apply
                                backend.orientation = orientation
                                persistData.saveData("Orientation", orientation)
                                console.log("Orientation:", orientation)

                                /* -----------------------------
                                   DISPLAY OFF (Minutes)
                                ----------------------------- */
                                const displayOffMin = displayOffModel.get(displayOffComboBox.currentIndex).value
                                const displayOffSec = displayOffMin * 60
                                backend.idleTimeSeconds = displayOffMin
                                persistData.displyOff = displayOffMin
                                console.log("Display Idle (min, sec):", displayOffMin, displayOffSec)

                                /* -----------------------------
                                   DEVICE AUTO POWER-OFF (Hours)
                                ----------------------------- */
                                const deviceOffHours = devicePowerOffModel.get(deviceOffComboBox.currentIndex).value
                                const deviceOffMSSec = deviceOffHours * 60 * 60 * 1000

                                backend.deviceAutoPowerOffHours = deviceOffMSSec
                                ActivityMonitor.idleTimeoutMs = deviceOffMSSec
                                persistData.deviceOff = deviceOffMSSec

                                console.log("Device Auto Power-Off (hours, seconds):", deviceOffHours, deviceOffMSSec)

                                /* -----------------------------
                                   TOUCH ENABLE
                                ----------------------------- */
                                backend.touchEnabled = enableTouch.checked
                                persistData.enableTouchScreen = enableTouch.checked
                                console.log("Touch Enabled:", enableTouch.checked)

                                /* -----------------------------
                                   APPLY TO WESTON
                                ----------------------------- */
                                const ok = backend.applyDisplaySettings(
                                             backend.resolution,
                                             backend.orientation,
                                             backend.idleTimeSeconds * 60,   // convert minutes → seconds
                                             backend.touchEnabled
                                             )

                                if (!ok) {
                                    console.warn("❌ Failed to apply settings")
                                } else {
                                    console.log("Settings applied successfully")
                                }

                                console.log("=== APPLY SETTINGS END ===")
                            }
                        }
                    }
                }
            }
        }
    }

    function logSelection(category, index, model) {
        if (index < 0 || index >= model.count) {
            console.warn("[UI][" + category + "] Invalid index:", index)
            return
        }

        const item = model.get(index)

        console.log(
                    "\n==============================",
                    "\n  UI Selection Changed",
                    "\n------------------------------",
                    "\n Category:", category,
                    "\n Index   :", index,
                    "\n Text    :", item.text,
                    "\n Value   :", item.value !== undefined ? item.value : "(none)",
                    "\n Apply   :", item.apply !== undefined ? item.apply : "(none)",
                    "\n==============================\n"
                    )
    }

    Component {
        id: addCustomResolutionPage
        AddCustomResolution {
            onBackToHome: { root.pageStack.pop() }
            onCreated: {
                resolutionListModel.reloadFromDatabase()
            }
        }
    }

    // Catch ALL signals from C++ class
    Connections {
        target: ActivityMonitor

        // User Activity
        function onUserActivity() {
            log("User activity detected: keys/mouse/touch.")
        }

        // User Idle Before Suspend
        function onUserIdle() {
            log("Inactivity detected → system will suspend soon.")
        }

        // Application Inactive
        function onAppInactive() {
            log("Application declared inactive before suspend.")
        }

        // Application Active After Resume
        function onAppActive() {
            log("Application resumed and active after system wake.")
        }

        // System Suspend Triggered
        function onPowerSuspend() {
            log("C++ notified: system suspend initiated.")
        }

        // System Resume Triggered
        function onPowerResume() {
            log("C++ notified: system resumed from suspend.")
        }

        // Idle Timeout Updated (from UI)
        function onIdleTimeoutMsChanged() {
            log("Idle timeout updated: " + ActivityMonitor.idleTimeoutMs + " ms")
        }
    }

    function log(msg) {
        console.log("[ActivityMonitor][QML]", msg)
    }

}
