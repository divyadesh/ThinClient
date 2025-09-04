import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0

import "../components"
import "../controls"
import "../dialogs"

BasicPage {
    id: page
    StackView.visible: true
    padding: 20
    property bool addNewServer: false

    header: PageHeader {
        pageTitle: page.pageTitle
        onBackPressed: {
            if(pageStack.depth == 1) {
                backToHome()
                return
            }
            pageStack.pop()
        }
    }


    contentItem: Item {
        PrefsButton {
            text: qsTr("Adesh Singh click")
            onClicked: {
                pageStack.push(deviceReset)
            }
        }
    }

    Component {
        id: deviceReset
        DeviceResetDialog {}
    }
}
