import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0

Button {
    id: control
    icon.width: Theme.iconExtraLarge
    icon.height: Theme.iconExtraLarge
    display: AbstractButton.TextUnderIcon
    icon.source: Qt.resolvedUrl("qrc:/assets/icons/rd-client.png")
    icon.color: Colors.transparent

    padding: 20
    leftPadding: 50
    rightPadding: 50
    hoverEnabled: true
    palette.buttonText: Colors.textPrimary
    background: null
}
