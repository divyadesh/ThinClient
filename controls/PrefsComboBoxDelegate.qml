import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.3
import App.Styles 1.0
import AppSecurity 1.0
import QtQuick.Controls.impl 2.15
import "../components"

ItemDelegate {
    id: control
    Layout.fillWidth: true
    background: null
    topPadding: 0
    bottomPadding: 0

    property alias model: combo.model
    property alias textRole: combo.textRole
    property alias valueRole: combo.valueRole
    property alias currentIndex: combo.currentIndex
    property alias currentValue: combo.currentValue

    signal activated(int index)

    contentItem: TextField {
        text: control.text
        background: null
        readOnly: true
        placeholderTextColor: Qt.lighter(Colors.inputText, 0.8)
        color: Colors.inputText
    }

    indicator: ComboBox {
        id: combo
        x: control.width - width - control.rightPadding
        y: control.topPadding + (control.availableHeight - height) / 2
        z: 10
        palette.dark: Colors.inputText
        palette.light: Colors.inputText

        onActivated: function(index) {
            control.activated(index)
        }

        contentItem: PrefsLabel {
            text: combo.displayText
            leftPadding: 5
            color: Qt.lighter(Colors.inputText, 0.8)
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }

        background: Item {
            implicitWidth: 120
            implicitHeight: 32
        }
    }
}
