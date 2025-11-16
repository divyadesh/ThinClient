import QtQuick 2.15

Rectangle {
    color: "#000000"
    opacity: 0.3

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onWheel: function(wheel) {
            wheel.accepted = false
        }
    }
}
