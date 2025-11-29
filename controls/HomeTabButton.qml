import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0

TabButton {
    id: control
    icon.width: Theme.iconLarge
    icon.height: Theme.iconLarge
    display: AbstractButton.TextUnderIcon
    padding: 20
    spacing: 20
    activeFocusOnTab: true
    focusPolicy: Qt.StrongFocus

    background: Rectangle {
        anchors.fill: parent
        color: control.checked ? Colors.accentHover : Colors.btnBg
        border.color: control.activeFocus ? "#FFFFFF" : "transparent"
        border.width: control.activeFocus ? 2 : 0
        radius: 6
    }
}
