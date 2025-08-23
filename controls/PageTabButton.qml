import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0

TabButton {
    id: control
    padding: 10
    checkable: true

    background: Rectangle {
        color: control.checked ? Colors.accentHover : Colors.secondaryBackground
        radius: 6
    }
}
