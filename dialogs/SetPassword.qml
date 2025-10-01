/*
Title: Update Password
Message: "Would you like to set a new password or remove the existing one?"
Buttons: Cancel | Set Password | Remove Password
*/

import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

import App.Styles 1.0
import AppSecurity 1.0

import "../pages"
import "../components"
import "../controls"

BasicPage {
    id: control
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
                text: unlockManager.hasPassword ? qsTr("Update Password") : qsTr("Set Password")
                font.pixelSize: 24
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        contentItem: Control {
            padding: 20
            leftPadding: 50
            rightPadding: 50

            contentItem: ColumnLayout {
                spacing: 20

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

                    PrefsLabel {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        text: unlockManager.hasPassword ? qsTr("Enter Old Password") : qsTr("Enter Password")
                        font.pixelSize: 16
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                    }

                    PrefsTextField {
                        id: enterPasswordField
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        horizontalAlignment: TextField.AlignLeft
                        onTextChanged: errorText.text = ''
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

                    PrefsLabel {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        text: unlockManager.hasPassword ? qsTr("Enter New Password") : qsTr("Confirm Password")
                        font.pixelSize: 16
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                    }

                    PrefsTextField {
                        id: confirmPasswordField
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        horizontalAlignment: TextField.AlignLeft
                        onTextChanged: errorText.text = ''
                    }
                }

                PrefsLabel {
                    id: errorText
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    font.pixelSize: 12
                    visible: !!text
                    color: Colors.statusError
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }


        footer: Control {
            implicitHeight: 72

            contentItem: RowLayout {
                spacing: 20

                Item {
                    Layout.fillWidth: true
                }

                PrefsButton {
                    text: qsTr("Cancel")
                    radius: height / 2
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: pageStack.pop()
                }

                PrefsButton {
                    text: unlockManager.hasPassword ? qsTr("Update") : qsTr("Save")
                    highlighted: true
                    radius: height / 2
                    enabled: enterPasswordField.text.length > 0 && confirmPasswordField.text.length > 0
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

                    onClicked: {
                        if(unlockManager.hasPassword) {
                            changePassword()
                        }else {
                            setPassword()
                        }
                    }

                    function changePassword() {
                        //changePassword(old, new)
                        if(enterPasswordField.text.length ===0 || confirmPasswordField.text.length ===0) {
                            errorText.text = "Field Should not be empty."
                            console.log(errorText.text)
                            return;
                        }

                        if(enterPasswordField.text === confirmPasswordField.text) {
                            pageStack.pop()
                            return;
                        }

                        var result = unlockManager.loadUnlockToken(enterPasswordField.text.trim())
                        if (result === "UNLOCK_OK" && unlockManager.storeUnlockToken(confirmPasswordField.text.trim(), "UNLOCK_OK")) {
                            console.log("Password change successfully")
                            pageStack.pop()
                        }else {
                            errorText.text = "Failed to change password."
                            console.log(errorText.text)
                        }
                    }

                    function setPassword() {
                        if(enterPasswordField.text.length ===0 || confirmPasswordField.text.length ===0) {
                            errorText.text = "Field Should not be empty."
                            console.log(errorText.text.trim())
                            return;
                        }

                        if(enterPasswordField.text.trim() !== confirmPasswordField.text.trim()) {
                            errorText.text = "Enter Password and confirm password should be same."
                            console.log(errorText.text)
                            return;
                        }

                        if (unlockManager.storeUnlockToken(enterPasswordField.text.trim(), "UNLOCK_OK")) {
                            console.log("Password set successfully")
                            pageStack.pop()
                        }else {
                            errorText.text = "Failed to set password (already exists?)"
                            console.log(errorText.text.trim())
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }
    }
}
