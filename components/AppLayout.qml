import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0
import App.Backend 1.0

import "../pages"
import "../controls"

BasicPage {
    PrefsStackView {
        id: pageStack
        anchors.fill: parent
        initialItem: homePaage
    }

    Component {
        id: homePaage
        HomePage {}
    }
}
