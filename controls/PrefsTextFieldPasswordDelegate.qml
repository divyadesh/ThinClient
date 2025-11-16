import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.3
import App.Styles 1.0
import AppSecurity 1.0
import "../components"

ItemDelegate {
    id: control
    Layout.fillWidth: true
    background: null
    topPadding: 0
    bottomPadding: 0

    property alias textFieldText: textField.text
    property string textFieldPlaceholderText: textField.text

    contentItem: TextField {
        id: textField
        background: null
        placeholderTextColor: Qt.lighter(Colors.inputText, 0.8)
        color: Colors.inputText
        placeholderText: control.textFieldPlaceholderText
        echoMode: eyeButton.checked ? TextField.Normal : TextField.Password
    }

    indicator: ToolButton {
        id: eyeButton
        x: control.width - width - control.rightPadding
        y: control.topPadding + (control.availableHeight - height) / 2
        checkable: true
        checked: false
        z: 10
        icon.source: checked ? "qrc:/assets/icons/ic_eye-on.svg" : "qrc:/assets/icons/ic_eye-off.svg"
        icon.color: Colors.inputText
        background: Item {
            implicitWidth: 32
            implicitHeight: 32
        }
    }
}
