import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0

import "../controls"
import "../components"

BasicPage {
    background: Image {
        //width : height = 3 : 2 Aspect ratio
        source: Qt.resolvedUrl("qrc:/assets/icons/background.jpg")
    }

    Image {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: -40
        anchors.topMargin: -40
        sourceSize: Qt.size(600, 210)
        source: Qt.resolvedUrl("qrc:/assets/icons/logos/logo.png")
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
                icon: Qt.resolvedUrl("qrc:/assets/icons/wifi.png")

                onClicked: {}
            }

            Item {
                Layout.fillHeight: true
            }

            Icon {
                iconWidth: Theme.iconLarge
                iconHeight: Theme.iconLarge
                icon: Qt.resolvedUrl("qrc:/assets/icons/settings.png")

                onClicked: {}
            }
        }
    }

    Component {
        id: dashboardPage
        DashboardPage {}
    }
}
