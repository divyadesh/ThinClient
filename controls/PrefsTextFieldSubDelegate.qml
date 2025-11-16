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

    contentItem: ColumnLayout {
        TextField {
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            text: control.text
            background: null
            readOnly: true
            color: Colors.inputText
            horizontalAlignment: TextField.AlignLeft
            verticalAlignment: TextField.AlignVCenter
        }

        TextField {
            id: textField
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            background: null
            placeholderTextColor: Qt.lighter(Colors.inputText, 0.8)
            color: Colors.inputText
            placeholderText: control.textFieldPlaceholderText
            horizontalAlignment: TextField.AlignLeft
            verticalAlignment: TextField.AlignVCenter
        }
    }
}
