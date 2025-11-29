import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import App.Styles 1.0
import AppSecurity 1.0
import App.Backend 1.0

import "../controls"
import "../components"

BasicPage {
    id: control
    signal loginSuccess()

    background: Rectangle {
        color: "#000000"
        opacity: 0.3
    }

    UnlockManager {
        id: unlockManager
    }

    Page {
        anchors.centerIn: parent
        width: 480

        background: Rectangle {
            radius: 8
            color: Colors.steelGray
            border.width: 1
            border.color: Colors.borderColor
        }

        header: Control {
            implicitHeight: 52
            padding: 10
            topPadding: 16

            contentItem: PrefsLabel {
                text: qsTr("Login")
                font.pixelSize: 24
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        contentItem: Control {
            padding: 20
            leftPadding: 30
            rightPadding: 30

            contentItem: ColumnLayout {
                spacing: 20

                Item {
                    Layout.fillWidth: true
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    spacing: 10

                    PrefsLabel {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        text: qsTr("Enter Password")
                        font.pixelSize: 16
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                    }

                    PrefsTextField {
                        id: passwordField
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        horizontalAlignment: TextField.AlignHCenter
                        focus: true
                        color: Colors.textPrimary

                        onTextChanged: {
                            errorText.text = ""
                        }

                        Keys.onEnterPressed:  {
                            if(passwordField.text.length > 0) {
                                validate()
                            }
                        }

                        Keys.onReturnPressed: {
                            if(passwordField.text.length > 0) {
                                validate()
                            }
                        }

                        Component.onCompleted: passwordField.forceActiveFocus()
                    }
                }

                PrefsLabel {
                    id: errorText
                    Layout.preferredWidth: parent.width
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    font.pixelSize: 14
                    color: Colors.statusError
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }


        footer: Control {
            leftPadding: 30
            rightPadding: 30
            implicitHeight: 72

            contentItem: RowLayout {
                spacing: 20

                Item {
                    Layout.fillWidth: true
                }

                PrefsButton {
                    text: qsTr("Login")
                    enabled: passwordField.text.length > 0
                    highlighted: true
                    radius: height / 2
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: {validate()}
                }
            }
        }
    }

    function validate() {
        var result = unlockManager.loadUnlockToken(passwordField.text.trim())
        if (result === "UNLOCK_OK") {
            console.log("✅ Access granted")
            loginSuccess()
        } else {
            console.log("❌ Access denied")
            errorText.text = qsTr("The password you entered is incorrect. Please try again.")
        }
    }
}

