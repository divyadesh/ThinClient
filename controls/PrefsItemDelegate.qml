import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import App.Styles 1.0

ItemDelegate {
    text: qsTr("Performance")
    font.pixelSize: 16
    palette.text: Colors.textPrimary
    leftPadding: 20
    rightPadding: 20

    background: Rectangle {
        implicitWidth: 100
        implicitHeight: 40
        radius: 8
        color: Colors.steelGray
        border.width: 1
        border.color: Colors.borderColor
    }
}
