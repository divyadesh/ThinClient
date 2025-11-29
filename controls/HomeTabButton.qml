import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0

TabButton {
    id: control
    property bool isActiveFocus: activeFocus &&
                                 (focusReason === Qt.TabFocusReason ||
                                  focusReason === Qt.BacktabFocusReason)
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
        border.color: control.isActiveFocus ? "#FFFFFF" : "transparent"
        border.width: control.isActiveFocus ? 2 : 0
        radius: 6
    }
}
