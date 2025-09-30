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
                        id: timezone
                        Layout.fillWidth: true
                        text: qsTr("Time Zone")

                        indicator: PrefsComboBox {
                            x: timezone.width - width - timezone.rightPadding
                            y: timezone.topPadding + (timezone.availableHeight - height) / 2

                            model: timeZoneModel
                            textRole: "tzName"

                            Component.onCompleted: {
                                for (var i = 0; i < model.count; ++i) {
                                    if (model.get(i).tzId === appSettings.selectedTimeZone) {
                                        currentIndex = i
                                        break
                                    }
                                }
                            }

                            onCurrentIndexChanged: {
                                var obj = model.get(currentIndex)
                                appSettings.selectedTimeZone = obj.tzId
                            }
                        }
                    }

                    PrefsItemDelegate {
                        id: language
                        Layout.fillWidth: true
                        text: qsTr("Language")

                        indicator: PrefsComboBox {
                            x: language.width - width - language.rightPadding
                            y: language.topPadding + (language.availableHeight - height) / 2


                            model: languageModel
                            textRole: "langName"

                            Component.onCompleted: {
                                var idx = languageModel.indexForCode(appSettings.selectedLanguage)
                                if (idx >= 0) currentIndex = idx
                            }

                            onCurrentIndexChanged: {
                                var obj = languageModel.get(currentIndex)
                                appSettings.selectedLanguage = obj.langCode
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
}
