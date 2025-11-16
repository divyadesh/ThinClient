import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.3
import App.Styles 1.0
import AppSecurity 1.0

import "../pages"
import "../components"
import "../controls"

BaseDialog {
    id: control
    pageTitle: qsTr("Add Network")

    onAccepted: {

    }
    onRejected: {

    }

    PrefsFrame {
        Layout.fillWidth: true

        ColumnLayout {
            anchors.fill: parent

            PrefsTextFieldDelegate {
                textFieldText: ""
                textFieldPlaceholderText:  qsTr("Hotspot name (required)")
                Layout.fillWidth: true
            }

            PrefsSeparator {}

            PrefsComboBoxDelegate {
                model: securityModel
                text: qsTr("Security")
                textRole: "name"
                valueRole: "typeId"
            }

            PrefsSeparator {}

            PrefsTextFieldPasswordDelegate {
                textFieldText: ""
                textFieldPlaceholderText: qsTr("Password")
            }
        }
    }

    PrefsFrame{
        Layout.fillWidth: true

        ColumnLayout {
            anchors.fill: parent

            PrefsComboBoxDelegate {
                model: securityModel
                text: qsTr("Proxy")
                textRole: "name"
                valueRole: "typeId"
            }

            PrefsSeparator {}

            PrefsComboBoxDelegate {
                model: securityModel
                text: qsTr("IP settings")
                textRole: "name"
                valueRole: "typeId"
            }

            PrefsSeparator {}

            PrefsTextFieldSubDelegate {
                text: qsTr("IP address")
                textFieldText: ""
                textFieldPlaceholderText: qsTr("192.168.10.1")
            }

            PrefsSeparator {}

            PrefsTextFieldSubDelegate {
                text: qsTr("Router")
                textFieldText: ""
                textFieldPlaceholderText:  qsTr("192.168.10.1")
            }

            PrefsSeparator {}

            PrefsTextFieldSubDelegate {
                text: qsTr("Prefix length")
                textFieldText: ""
                textFieldPlaceholderText: "24"
            }
            PrefsSeparator {}

            PrefsTextFieldSubDelegate {
                text: qsTr("DNS 1")
                textFieldText: ""
                textFieldPlaceholderText:  "0.0.0.0"
            }
            PrefsSeparator {}

            PrefsTextFieldSubDelegate {
                text: qsTr("DNS 2")
                textFieldText: ""
                textFieldPlaceholderText:  "0.0.0.0"
            }

            PrefsSeparator {}

            PrefsComboBoxDelegate {
                model: securityModel
                text: qsTr("Privacy")
                textRole: "name"
                valueRole: "typeId"
            }

            PrefsSeparator {}

            PrefsComboBoxDelegate {
                model: securityModel
                text: qsTr("Metered")
                textRole: "name"
                valueRole: "typeId"
            }

            PrefsSeparator {}

            PrefsComboBoxDelegate {
                model: securityModel
                text: qsTr("Hidden network")
                textRole: "name"
                valueRole: "typeId"
            }
        }
    }
}
