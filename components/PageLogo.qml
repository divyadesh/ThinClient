import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0

Button {
    id: control
    icon.width: Theme.iconExtraLarge
    icon.height: Theme.iconExtraLarge
    display: AbstractButton.TextUnderIcon
    icon.source: Qt.resolvedUrl("qrc:/assets/icons/ic_thinclient.svg")
    icon.color: Colors.textPrimary

    padding: 20
    leftPadding: 50
    rightPadding: 50
    hoverEnabled: true
    palette.buttonText: Colors.textPrimary

    background: Rectangle {
        anchors.fill: parent
        color: Colors.secondaryBackground
        radius: 6
    }
}
