import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import App.Styles 1.0

TextField {
    id: control
    placeholderTextColor: Colors.textSecondary
    selectedTextColor: Colors.textSecondary
    selectionColor: Colors.accentPrimary
    color: Colors.textPrimary

    horizontalAlignment: TextField.AlignHCenter
    verticalAlignment: TextField.AlignVCenter

    activeFocusOnTab: true
    background: Item {
        implicitWidth: 260
        implicitHeight: 40

        Rectangle {
            id: bg
            anchors.fill: parent
            radius: 8
            color: control.enabled ? "#232326": Qt.rgba(46/255, 47/255, 48/255, 0.4)
            border.width: control.activeFocus ? 2 : 1
            border.color: control.activeFocus ? Colors.accentPrimary : "#0F0E13"
        }
    }
}
