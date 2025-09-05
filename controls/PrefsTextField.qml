import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import App.Styles 1.0
import QtGraphicalEffects 1.15

TextField {
    id: control
    placeholderTextColor: Colors.textSecondary
    selectedTextColor: Colors.textSecondary
    selectionColor: Colors.accentPrimary

    horizontalAlignment: TextField.AlignHCenter
    verticalAlignment: TextField.AlignVCenter

    // background: Rectangle {
    //     implicitWidth: 260
    //     implicitHeight: 40
    //     radius: 8
    //     border.width: control.activeFocus ? 2 : 1
    //     color: control.enabled ? Qt.rgba(46/255, 47/255, 48/255, 1.0) : Qt.rgba(46/255, 47/255, 48/255, 0.4)
    //     border.color: control.activeFocus ? Colors.accentPrimary : Colors.secondaryBackground
    // }

    background: Item {
        implicitWidth: 260
        implicitHeight: 40

        Rectangle {
            id: bg
            anchors.fill: parent
            radius: 8
            color: control.enabled ? Colors.btnBg : Qt.rgba(46/255, 47/255, 48/255, 0.4)
            border.width: control.activeFocus ? 2 : 1
            border.color: control.activeFocus ? Colors.accentPrimary : Colors.secondaryBackground
        }

        // subtle inner shadow
        DropShadow {
            anchors.fill: bg
            source: bg
            radius: 8
            samples: 16
            horizontalOffset: 0
            verticalOffset: 1
            color: Qt.rgba(0,0,0,0.4)
        }
    }
}
