import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0
import App.Enums 1.0
import G1.ThinClient 1.0

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
            if(pageStack.depth === 1) {
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

                        // keep it simple: store as int (0 Jack, 1 USB, 2 HDMI)
                        property int audioSelection: {
                            const v = persistData.getData("Audio");
                            if (v === "" || v === undefined) return Audio.Jack; // default
                            return parseInt(v);
                        }

                        ButtonGroup { id: tabGroup }

                        indicator: RowLayout {
                            x: audio.width - width - audio.rightPadding
                            y: audio.topPadding + (audio.availableHeight - height) / 2
                            spacing: 20

                            PrefsTabButton {
                                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                                ButtonGroup.group: tabGroup
                                checked: audio.audioSelection === Audio.Jack   // ✅ bind to int
                                text: "Jack"
                                visible: !!text
                                font.weight: Font.Normal
                                onClicked: {
                                    if (audio.audioSelection !== Audio.Jack) {
                                        audio.audioSelection = Audio.Jack;
                                        persistData.saveData("Audio", audio.audioSelection); // ✅ persist
                                    }
                                }
                            }

                            PrefsTabButton {
                                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                                ButtonGroup.group: tabGroup
                                checked: audio.audioSelection === Audio.Usb
                                text: "USB"
                                visible: !!text
                                font.weight: Font.Normal
                                onClicked: {
                                    if (audio.audioSelection !== Audio.Usb) {
                                        audio.audioSelection = Audio.Usb;
                                        persistData.saveData("Audio", audio.audioSelection);
                                    }
                                }
                            }

                            PrefsTabButton {
                                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                                ButtonGroup.group: tabGroup
                                checked: audio.audioSelection === Audio.Hdmi
                                text: "HDMI"
                                visible: !!text
                                font.weight: Font.Normal
                                onClicked: {
                                    if (audio.audioSelection !== Audio.Hdmi) {
                                        audio.audioSelection = Audio.Hdmi;
                                        persistData.saveData("Audio", audio.audioSelection);
                                    }
                                }
                            }
                        }
                    }

                    PrefsItemDelegate {
                        id: timezone
                        Layout.fillWidth: true
                        text: qsTr("Time Zone")

                        indicator: PrefsComboBox {
                            id: timeZoneComboBox
                            x: timezone.width - width - timezone.rightPadding
                            y: timezone.topPadding + (timezone.availableHeight - height) / 2

                            model: timezoneModel
                            textRole: "tzId"
                            //imezoneProxyModel.filterString = text

                            Component.onCompleted: {
                                let timeZone = persistData.getData("TimeZone")
                                if(timeZone) {
                                    for (var i = 0; i < timeZoneComboBox.count; ++i) {
                                        if (timezoneModel.get(i).tzId === timeZone) {
                                            currentIndex = i;
                                            break;
                                        }
                                    }
                                }
                            }

                            onActivated: {
                                var obj = timezoneModel.get(currentIndex)
                                persistData.saveData("TimeZone", obj.tzId)
                            }
                        }
                    }

                    PrefsItemDelegate {
                        id: language
                        Layout.fillWidth: true
                        text: qsTr("Language")

                        indicator: PrefsComboBox {
                            id: languageComboBox
                            x: language.width - width - language.rightPadding
                            y: language.topPadding + (language.availableHeight - height) / 2

                            model: languageModel
                            textRole: "displayName"

                            Component.onCompleted: {
                                let language = persistData.getData("Language")
                                if(language) {
                                    for (var i = 0; i < languageComboBox.count; ++i) {
                                        if (languageModel.get(i).displayName === language) {
                                            currentIndex = i;
                                            break;
                                        }
                                    }
                                }
                            }

                            onActivated: {
                                var obj = languageModel.get(currentIndex)
                                persistData.saveData("Language", obj.displayName)
                            }
                        }
                    }

                    Item {
                        Layout.preferredHeight: 20
                        Layout.fillWidth: true
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

                                Component.onCompleted: {
                                    // Load saved setting
                                    let savedValue = persistData.getData("EnableOnScreenKeyboard")
                                    if (savedValue === "true" || savedValue === true) {
                                        checked = true
                                    } else {
                                        checked = false
                                    }
                                }

                                onClicked: {
                                    // Toggle and save persistently
                                    persistData.saveData("EnableOnScreenKeyboard", checked)
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
                                checkable: true
                                implicitWidth: 260
                                text: qsTr("Enable")
                                font.weight: Font.Normal

                                Component.onCompleted: {
                                    let savedValue = persistData.getData("EnableTouchScreen")
                                    if (savedValue === "true" || savedValue === true) {
                                        checked = true
                                    } else {
                                        checked = false
                                    }
                                }

                                onClicked: {
                                    persistData.saveData("EnableTouchScreen", checked)
                                }
                            }
                        }
                    }

                    PrefsItemDelegate {
                        id: updatePwdDelegate
                        Layout.fillWidth: true
                        text: qsTr("Update Password")

                        indicator: PrefsButton {
                            x: updatePwdDelegate.width - width - updatePwdDelegate.rightPadding
                            y: updatePwdDelegate.topPadding + (updatePwdDelegate.availableHeight - height) / 2
                            implicitWidth: 260
                            text: qsTr("Update")
                            onClicked: pageStack.push(updatePassword)
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
                    border.width: 1
                    border.color: Colors.borderColor
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
                            enabled: usbMonitor.usbConnected
                            text: qsTr("Update From USB")
                            onClicked: pageStack.push(updateDialog)
                        }

                        PrefsButton {
                            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                            Layout.fillWidth: true
                            radius: height / 2
                            text: exporter.busy ? "Exporting..." : "Export Logs to USB"
                            enabled: !exporter.busy && usbMonitor.usbConnected
                            onClicked: exporter.exportLogs(usbMonitor.usbStoragePort)
                        }

                        BusyIndicator {
                            running: exporter.busy
                            visible: exporter.busy
                        }
                    }

                    Item {Layout.fillWidth: true}

                    Label {
                        id: messageLabel
                        text: exporter.statusMessage
                        visible: text
                        wrapMode: Text.WordWrap
                        font.pixelSize: 14
                    }

                    Item {Layout.fillWidth: true}
                }
            }
        }
    }

    Component {
        id: deviceReset
        DeviceResetDialog {}
    }

    Component {
        id: updatePassword
        UpdatePasswordDialog {
            onSetPassword: pageStack.replace(setPasswordDialog)
            onRemovePassword: pageStack.replace(removePasswordDailog)
        }
    }

    Component {
        id: setPasswordDialog
        SetPassword {}
    }

    Component {
        id: removePasswordDailog
        RemovePassword {}
    }

    Component {
        id: updateDialog
        SoftwareUpdater {}
    }

    LogExporter {
         id: exporter
         onExportFinished: (success, msg) => {
             messageLabel.text = msg
             if (success) messageLabel.color = "green"
             else messageLabel.color = "red"
         }
     }
}
