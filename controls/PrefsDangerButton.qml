import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import App.Styles 1.0

PrefsButton {
    id: control
    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
    highlighted: true

    background: Rectangle {
        implicitHeight: 34
        implicitWidth: 120
        radius: height / 2

        color: {
            if (!control.enabled) {
                return control.highlighted ? Colors.accentDisabled : Colors.btnBgDisabled;
            }
            return Colors.statusError;
        }
    }
}
