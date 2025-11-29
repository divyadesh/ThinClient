import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import App.Styles 1.0

Button {
    id: control
    property int radius: height / 2
    hoverEnabled: true
    leftPadding: 8
    rightPadding: 8
    scale: control.pressed ? 0.98 : 1.0
    palette.buttonText: Colors.textPrimary
    activeFocusOnTab: true
    focusPolicy: Qt.StrongFocus

    background: Rectangle {
        implicitHeight: 34
        implicitWidth: 120
        radius: control.radius
        border.color: control.activeFocus ? "#FFFFFF" : "transparent"
        border.width: control.activeFocus ? 2 : 0

        color: {
            if (!control.enabled) {
                return control.highlighted || control.checked  ? Colors.accentDisabled : Colors.btnBgDisabled;
            }

            if (control.highlighted || control.checked ) {
                return control.hovered ? Colors.accentHover : Colors.accentPrimary;
            }

            return Colors.btnBg;
        }
    }
}
