import QtQuick 2.15
import App.Styles 1.0

Rectangle {
    id: toggleButton
    width: 45
    height: 20
    radius: 10
    border.color: Colors.secondaryBackground
    color: toggleEnabled ? "blue" : Colors.secondaryBackground

    property bool toggleEnabled: true

    Rectangle {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: !toggleEnabled ? 3 : 28
        Behavior on anchors.leftMargin {
            NumberAnimation { duration: 150; easing.type: Easing.InOutQuad }
        }
        height: 14
        width: 14
        radius: 7
        color: Colors.surfaceBackground
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            console.log("::::> toggleEnabled1="+toggleEnabled)
            toggleEnabled = !toggleEnabled
            console.log("::::> toggleEnabled2="+toggleEnabled)
        }
    }
}
