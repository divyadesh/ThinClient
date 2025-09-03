import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import App.Styles 1.0

Button {
    id: control
    hoverEnabled: true
    scale: control.pressed ? 0.98 : 1.0
    palette.buttonText: control.enabled ? Colors.textPrimary : Colors.textSecondary

    background: Rectangle {
        implicitHeight: 40
        implicitWidth: 120
        radius: Theme.radiusSmall
        color: {
            if (!control.enabled) {
                return control.highlighted ? Colors.accentDisabled : Colors.btnBgDisabled;
            }

            if (control.highlighted) {
                return control.hovered ? Colors.accentHover : Colors.accentPrimary;
            }

            return Colors.btnBg;
        }
    }
}
