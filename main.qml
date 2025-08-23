import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Window 2.15
import App.Styles 1.0

import "pages"

ApplicationWindow {
    id: window
    width: ScreenConfig.screenWidth
    height: ScreenConfig.screenHeight
    visible: true
    title: qsTr("G1 Thin Client pc")

    HomePage {
        anchors.fill: parent
    }
}
