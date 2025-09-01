import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0

import "../components"

BasicPage {
    id: deviceSettingsPageRoot
    padding: 20
    ColumnLayout {
        spacing: 25
        Rectangle {
            width: 500
            height: 270
            radius: 4
            border{ color: Colors.secondaryBackground; width: 2 }
            color: "transparent"
            ColumnLayout {
                anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 20;
                    right: parent.right; rightMargin: 20; bottom: parent.bottom; bottomMargin: 20 }
                spacing: 10

                Rectangle {
                    height: 40
                    width: 150
                    color: Colors.secondaryBackground
                    radius: 6
                    Text {
                        anchors.centerIn: parent
                        text: qsTr("Device Settings")
                    }
                }
                ListView {
                    id: availableWifiList
                    width: 470; height: 150
                    clip: true
                    model: [ qsTr("Audio"), qsTr("Device Name"), qsTr("Time Zone"), qsTr("Language"), qsTr("On Screen Keyboard"),
                                                            qsTr("Password"), qsTr("Touch Screen")]
                    delegate: Item {
                        width: availableWifiList.width; height: 44;
                        Rectangle {
                            anchors.margins: 2
                            width: parent.width; height: 40; radius: 6; color: "transparent"
                            Rectangle {
                                id: ssidItem
                                anchors { left: parent.left; right: parent.right; rightMargin: parent.width/2;
                                    verticalCenter: parent.verticalCenter }
                                height: 30
                                radius: 4
                                color: Colors.secondaryBackground //barariya
                                Text {
                                    anchors.centerIn: parent
                                    text: modelData
                                }
                            }
                            Item {
                                id: barsItem
                                anchors { left: ssidItem.right; right: parent.right; verticalCenter: parent.verticalCenter }
                                Loader {
                                    visible: asynchronous ? (status === Loader.Ready) ? true : false : true
                                    width: sourceComponent.width
                                    height: sourceComponent.height
                                    anchors.centerIn: parent
                                    sourceComponent: index === 0 ? comboBoxComponent :
                                                        (index === 1 || index === 2 || index === 3) ? textComponent :
                                                            (index === 4 || index === 6) ? toggleButtonComponent :
                                                            textFieldComponent

                                    Component {
                                        id: toggleButtonComponent
                                        TCToggleButton {
                                            id: toggleButton
                                        }
                                    }
                                    Component {
                                        id: textComponent
                                        Rectangle {
                                            width: 200
                                            height: 30
                                            anchors.centerIn: parent
                                            radius: 4
                                            color: Colors.secondaryBackground
                                            function getTimeZone() {
                                                var d = new Date()
                                                var offsetMinutes = d.getTimezoneOffset()
                                                var offsetHours = -offsetMinutes / 60
                                                return "UTC" + (offsetHours >= 0 ? "+" : "") + offsetHours
                                            }

                                            Text {
                                                anchors.centerIn: parent
                                                text: index === 1 ? qsTr("RDClient") : index === 2 ? parent.getTimeZone() :
                                                                                                     index === 3 ? qsTr("English"): ""
                                            }
                                        }
                                    }
                                    Component {
                                        id: textFieldComponent
                                        TCTextField {
                                        }
                                    }
                                    Component {
                                        id: comboBoxComponent
                                        TCComboBox {
                                            model: ["Jack", "USB", "HDMI"]
                                        }
                                    }
                                }
                            }
                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true
                                onEntered: {
                                    parent.color = Colors.accentHover
                                }
                                onExited: {
                                    parent.color = "transparent"//Colors.secondaryBackground
                                }
                                onClicked: {
                                    //TODO: connect to the wifi network
                                }
                            }
                        }
                    }
                    ScrollBar.vertical: ScrollBar {
                        policy: ScrollBar.AlwaysOn  // or AsNeeded
                    }
                }
            }
        }
        // Rectangle {
        //     color: "transparent"
        //     width: 500
        //     height: 100
        //     radius: 4
        //     border { color: Colors.secondaryBackground; width: 2 }
            ColumnLayout {
                // anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 20;
                //     right: parent.right; rightMargin: 20; bottom: parent.bottom; bottomMargin: 20 }
                spacing: 20

                Rectangle {
                    height: 40
                    width: 250
                    color: Colors.secondaryBackground
                    radius: 6
                    Text {
                        anchors.centerIn: parent
                        text: qsTr("Device Restore & Upgrade")
                    }
                }
                RowLayout {
                    Repeater {
                        model: [qsTr("Reset"), qsTr("Update"), qsTr("Export Log")]
                        Rectangle {
                            id: reset
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignVCenter
                            height: 30
                            radius: 4
                            color: Colors.secondaryBackground //barariya
                            Text {
                                anchors.centerIn: parent
                                text: modelData
                            }
                        }
                    }
                }
            }
        //}
    }
}
