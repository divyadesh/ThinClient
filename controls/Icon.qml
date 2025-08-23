import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0

Control {
    id: control
    property color bgColor: Colors.transparent
    property color bgBorderColor: Colors.transparent
    property string icon: ""
    property int iconWidth: Theme.iconSmall
    property int iconHeight: Theme.iconSmall

    signal clicked
    scale: mouse.pressed ? 0.95 : 1.0

    background: Rectangle {
        implicitWidth: 40
        implicitHeight: 40
        radius: implicitHeight / 2
        color: control.bgColor
        border.color: control.bgBorderColor
    }

    Image {
        anchors.centerIn: parent
        source: control.icon
        sourceSize: Qt.size(control.iconWidth, control.iconHeight)
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        hoverEnabled: true
        onClicked: control.clicked()
    }
}
