import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import App.Styles 1.0

Button {
    id: control
    hoverEnabled: true
    scale: control.pressed ? 0.98 : 1.0
    font.pixelSize: 16

    background: Rectangle {
        implicitHeight: 40
        implicitWidth: 100
        radius: Theme.radiusSmall
        color: control.enabled ? (control.highlighted ? Colors.accentPrimary : control.hovered ? Colors.accentHover : Colors.inputBorder) : Colors.accentDisabled
    }
}
