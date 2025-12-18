import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.3
import App.Styles 1.0
import AppSecurity 1.0

import "../pages"
import "../controls"

Item {
    id: bgItem
    width: parent.width
    height: parent.height

    Rectangle {
        id: dialogRect
        anchors.fill: parent
        radius: 12
        color: "#2A2A2A"
        border.color: "#77767E"
    }

    DropShadow {
        anchors.fill: dialogRect
        source: dialogRect
        radius: 18
        samples: 32
        color: "#80000000"
        horizontalOffset: 0
        verticalOffset: 4
    }
}
