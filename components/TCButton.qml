import QtQuick 2.15
import App.Styles 1.0

Rectangle {
    id: addServerButton
    height: 30
    width: 150
    radius: 6
    color: mouseArea.pressed ? "#cccccc" : Colors.secondaryBackground

    property alias buttonText: buttnText.text
    signal sigMouseClicked()

    Text {
        id: buttnText
        anchors.centerIn: parent
    }
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: {
            console.log("Button clicked!")
            sigMouseClicked()
            // Call your function or signal here
        }
    }
}
