import QtQuick 2.0
import QtQuick.Controls 2.0
import App.Styles 1.0

CheckBox {
    id: root
    property color checkedColor: Colors.accentPrimary

    text: qsTr("CheckBox")
    hoverEnabled: true
    palette.buttonText: enabled ? Colors.textPrimary : Colors.textSecondary
    palette.windowText: enabled ? Colors.textPrimary : Colors.textSecondary

    indicator: Rectangle {
        x: root.leftPadding
        anchors.verticalCenter: parent.verticalCenter
        width: 26; height: width
        antialiasing: true
        radius: 5
        border.width: 2
        border.color: root.checkedColor


        Rectangle {
            anchors.centerIn: parent
            width: parent.width*0.7; height: width
            antialiasing: true
            radius: parent.radius * 0.7
            color: {
                if (checked)
                    return checkedColor


                if (hovered)
                    return "#BDC3C7"


                return "#00000000"
            }
            visible: {
                if (checked)
                    return true


                if (hovered)
                    return true


                return false
            }
            opacity: visible


            Behavior on opacity {
                NumberAnimation {
                    duration: 250
                }
            }
        }
    }
}
