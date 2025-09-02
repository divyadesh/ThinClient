import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0

import "../components"
import "../controls"

BasicPage {
    id: page
    topPadding: 20
    bottomPadding: 20

    contentItem: Page {
        leftPadding: 20
        rightPadding: 20
        background: null

        contentItem: Control {
            padding: 20

            background: Rectangle {
                radius: 6
                color: "transparent"
                border.color: Colors.secondaryBackground
            }

            contentItem: ColumnLayout {
                spacing: 10

                PrefsButton {
                    Layout.alignment: Qt.AlignLeft
                    text: qsTr("Device Settings")
                }

                ListView {
                    id: availableWifiList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    spacing: 10

                    ScrollBar.vertical: ScrollBar {
                        policy: ScrollBar.AlwaysOn
                    }

                    model: [
                        qsTr("Audio"),
                        qsTr("Device Name"),
                        qsTr("Time Zone"),
                        qsTr("Language"),
                        qsTr("On Screen Keyboard"),
                        qsTr("Password"),
                        qsTr("Touch Screen")
                    ]

                    delegate: Item {
                        width: availableWifiList.width
                        height: 44

                        Control {
                            anchors.margins: 2
                            width: parent.width
                            hoverEnabled: true

                            MouseArea {
                                acceptedButtons: Qt.RightButton
                                anchors.fill: parent
                                onClicked: { }
                            }

                            background: Rectangle {
                                implicitHeight: 40
                                radius: 6
                                color: parent.hovered ? Colors.accentHover : "transparent"
                            }

                            PrefsButton {
                                id: ssidItem
                                anchors {
                                    left: parent.left
                                    right: parent.right
                                    rightMargin: parent.width / 2
                                    verticalCenter: parent.verticalCenter
                                }

                                text: modelData
                            }

                            Item {
                                id: barsItem
                                anchors {
                                    left: ssidItem.right
                                    right: parent.right
                                    verticalCenter: parent.verticalCenter
                                }

                                Loader {
                                    visible: asynchronous ? (status === Loader.Ready) ? true : false : true
                                    width: sourceComponent.width
                                    height: sourceComponent.height
                                    anchors.centerIn: parent

                                    sourceComponent: componentForIndex(index)

                                    function componentForIndex(idx) {
                                        switch (idx) {
                                        case 0:
                                            return comboBoxComponent
                                        case 1:
                                        case 2:
                                        case 3:
                                            return textComponent
                                        case 4:
                                        case 6:
                                            return toggleButtonComponent
                                        default:
                                            return textFieldComponent
                                        }
                                    }

                                    Component {
                                        id: toggleButtonComponent
                                        TCToggleButton { id: toggleButton }
                                    }

                                    Component {
                                        id: textComponent
                                        PrefsButton {
                                            width: 200
                                            text: textForIndex(index)

                                            function textForIndex(idx) {
                                                switch (idx) {
                                                case 1: return qsTr("RDClient")
                                                case 2: return getTimeZone()
                                                case 3: return qsTr("English")
                                                default: return ""
                                                }
                                            }

                                            function getTimeZone() {
                                                var d = new Date()
                                                var offsetMinutes = d.getTimezoneOffset()
                                                var offsetHours = -offsetMinutes / 60
                                                return "UTC" + (offsetHours >= 0 ? "+" : "") + offsetHours
                                            }
                                        }
                                    }

                                    Component {
                                        id: textFieldComponent
                                        TCTextField { }
                                    }

                                    Component {
                                        id: comboBoxComponent
                                        TCComboBox {
                                            model: ["Jack", "USB", "HDMI"]
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        footer: Control {
            leftPadding: 20
            rightPadding: 20
            contentItem: ColumnLayout {
                spacing: 20

                Item {
                    Layout.fillHeight: true
                }

                PrefsButton {
                    Layout.preferredWidth: 250
                    text: qsTr("Device Restore && Upgrade")
                    onClicked: {}
                }

                RowLayout {
                    spacing: 10

                    PrefsButton {
                        text: qsTr("Reset")
                        onClicked: {}
                    }

                    PrefsButton {
                        text: qsTr("Update")
                        onClicked: {}
                    }

                    PrefsButton {
                        text: qsTr("Export Log")
                        onClicked: {}
                    }
                }

                Item {
                    Layout.fillHeight: true
                }
            }
        }
    }
}
