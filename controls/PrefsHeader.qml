import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import App.Styles 1.0

Control {
    id: control
    property string text: ""
    padding: 10
    leftPadding: 0
    font.pixelSize: 24

    contentItem: PrefsLabel {
        font: control.font
        text: control.text
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
    }
}
