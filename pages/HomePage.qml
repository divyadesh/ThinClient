import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0

import "../controls"
import "../components"

BasicPage {
    background: Image {
        //width : height = 3 : 2 Aspect ratio
        source: Qt.resolvedUrl("qrc:/assets/images/background.jpg")
    }

    Logo {
        anchors.left: parent.left
        anchors.top: parent.top
        padding: 20
    }

    Control {
        anchors.centerIn: parent
        padding: 40

        background: Rectangle {
            color: Colors.secondaryBackground
            radius: 8
        }

        contentItem: PrefsTabBar {
            spacing: 40

            HomeTabButton {
                text: qsTr("Windows")
                icon.source: Qt.resolvedUrl("qrc:/assets/icons/rd-client.png")
                onClicked: pageStack.push(dashboardPage, {server : false})
            }

            HomeTabButton {
                text: qsTr("Server")
                icon.source: Qt.resolvedUrl("qrc:/assets/icons/rd-client.png")
                onClicked: pageStack.push(dashboardPage, {server : true })
            }
        }
    }

    Control {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        padding: 20

        contentItem: ColumnLayout {

            Icon {
                iconWidth: Theme.iconLarge
                iconHeight: Theme.iconLarge
                icon: Qt.resolvedUrl("qrc:/assets/icons/ic_wifi.svg")

                onClicked: {}
            }

            Item {
                Layout.fillHeight: true
            }

            Icon {
                iconWidth: Theme.iconLarge
                iconHeight: Theme.iconLarge
                icon: Qt.resolvedUrl("qrc:/assets/icons/ic_settings.svg")

                onClicked: {}
            }
        }
    }

    Component {
        id: dashboardPage
        DashboardPage {}
    }
}
