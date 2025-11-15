import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import App.Styles 1.0

ItemDelegate {
    id: control
    property int radius: 8
    font.pixelSize: 16
    palette.text: Colors.textPrimary
    leftPadding: 20
    rightPadding: 20

    background: Rectangle {
        implicitWidth: 100
        implicitHeight: 40
        radius: control.radius
        color: "#2A2A2A"//Colors.steelGray
    }
}
