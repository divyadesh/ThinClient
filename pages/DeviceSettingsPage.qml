import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0
import App.Enums 1.0
import G1.ThinClient 1.0
import App.Backend 1.0

import "../components"
import "../controls"
import "../dialogs"

BasicPage {
    id: page
    StackView.visible: true
    padding: 20

    Connections {
        target: languageModel
        function onLanguageChanged() {
        }
    }

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

    contentItem: PrefsFlickable {
        id: formFlickable
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

                contentItem: ColumnLayout {
                    spacing: 10

                    PrefsItemDelegate {
                        id: audio
                        Layout.fillWidth: true
                        text: qsTr("Audio")

                        // keep it simple: store as int (0 Jack, 1 USB, 2 HDMI)
                        property int audioSelection: {
                            const v = persistData.audio;
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
                                        persistData.audio = Audio.Jack
                                        DeviceSettings.setAudioOutput("jack");
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
                                        persistData.audio = Audio.Usb
                                        DeviceSettings.setAudioOutput("usb");
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
                                        persistData.audio = Audio.Hdmi
                                        DeviceSettings.setAudioOutput("hdmi");
                                    }
                                }
                            }
                        }
                    }

                    PrefsItemDelegate {
                        id: timezone
                        Layout.fillWidth: true
                        text: qsTr("Time Zone")

                        indicator:  TimeZoneSelector {
                            id: timeZoneComboBox
                            x: timezone.width - width - timezone.rightPadding
                            y: timezone.topPadding + (timezone.availableHeight - height) / 2

                            initialTimezone: persistData.timeZone
                            model: timezoneModel
                            onActivated: {
                                persistData.timeZone = currentValue
                                timezoneModel.setSystemTimezone(currentValue)
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
                            valueRole: "localeId"

                            Component.onCompleted: {
                                let saved = persistData.language

                                if (!saved || saved === "") {
                                    // Fallback to English
                                    saved = "en_US"
                                }

                                // Step 1: search by valueRole (localeId)
                                let indexFound = -1
                                for (let i = 0; i < languageComboBox.count; i++) {
                                    let obj = languageComboBox.model.get(i)
                                    if (obj.localeId === saved) {
                                        indexFound = i
                                        break
                                    }
                                }

                                if (indexFound >= 0)
                                    languageComboBox.currentIndex = indexFound
                                else {
                                    // fallback: English
                                    languageComboBox.currentIndex = 0
                                }
                            }

                            onActivated: {
                                let obj = languageComboBox.model.get(currentIndex)
                                persistData.language = obj.localeId
                                languageModel.setSystemLanguage(currentValue)
                            }
                        }
                    }

                    Item {
                        Layout.preferredHeight: 20
                        Layout.fillWidth: true
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
                        Layout.preferredHeight: 30
                    }

                    PrefsItemDelegate {
                        Layout.fillWidth: true
                        padding: 20

                        background: Item {
                            implicitWidth: 100
                            implicitHeight: 40
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

                    Item {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 5
                    }
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
        onExportFinished: function(success, msg) {
            messageLabel.text = msg
            if (success) messageLabel.color = "green"
            else messageLabel.color = "red"
        }
    }
}
