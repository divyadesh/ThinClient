import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0

TabButton {
    id: control
    icon.width: Theme.iconExtraLarge
    icon.height: Theme.iconExtraLarge
    display: AbstractButton.TextUnderIcon
    padding: 20

    background: Rectangle {
        anchors.fill: parent
        color: control.checked ? Colors.accentHover : Colors.surfaceBackground
        radius: 6
    }
}
