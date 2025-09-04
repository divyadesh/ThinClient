import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import App.Styles 1.0


TextField {
    id: control
    placeholderTextColor: Colors.textSecondary
    selectedTextColor: Colors.textSecondary
    selectionColor: Colors.accentPrimary

    background: Rectangle {
        implicitWidth: 260
        implicitHeight: 40
        radius: 8
        border.width: control.activeFocus ? 2 : 1
        color: Qt.rgba(46/255, 47/255, 48/255, 1.0)
        border.color: control.activeFocus ? Colors.accentPrimary : Qt.rgba(63/255, 64/255, 65/255, 1.0)
    }
}
