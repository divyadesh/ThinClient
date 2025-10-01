import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0
import AppSecurity 1.0

import "../controls"
import "../components"

BasicPage {
    StackView.visible: true
    background: Image {
        //width : height = 3 : 2 Aspect ratio
        source: Qt.resolvedUrl("qrc:/assets/icons/background.jpg")
    }

    UnlockManager {
        id: unlockManager
    }

    ButtonGroup { id: tabGroup }

    Image {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: -40
        anchors.topMargin: -40
        sourceSize: Qt.size(600, 210)
        source: Qt.resolvedUrl("qrc:/assets/icons/logos/logo.png")
    }

    Control {
        width: parent.width * 0.8
        height: parent.height * 0.6
        anchors.centerIn: parent
        padding: 120

        contentItem: GridView {
            id: grid
            anchors.fill: parent
            cellWidth: 220; cellHeight: 220
            clip: true

            model: ListModel {
                ListElement { name: "Windows"; ip: "192.168.1.15" }
                ListElement { name: "Server"; ip: "192.168.1.24:3228" }
                ListElement { name: "Laptop"; ip: "192.168.1.50" }
                ListElement { name: "Laptop"; ip: "192.168.1.50" }
                ListElement { name: "Laptop"; ip: "192.168.1.50" }
                ListElement { name: "Laptop"; ip: "192.168.1.50" }
                ListElement { name: "Laptop"; ip: "192.168.1.50" }
                ListElement { name: "Laptop"; ip: "192.168.1.50" }
                ListElement { name: "Laptop"; ip: "192.168.1.50" }
            }


            highlight: Rectangle { color: "lightsteelblue"; radius: 8 }
            focus: true

            delegate: Control {
                padding: 20
                implicitWidth: grid.cellWidth
                implicitHeight: grid.cellHeight

                contentItem: HomeTabButton {
                    id: tabButton
                    ButtonGroup.group: tabGroup
                    text: name
                    icon.source: Qt.resolvedUrl("qrc:/assets/icons/rd-client.png")
                    onClicked: {}

                    background: Rectangle {
                        anchors.fill: parent
                        color: tabButton.checked ? Colors.accentHover : Colors.surfaceBackground
                        radius: 8
                    }
                }
            }
        }
    }

    Control {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        padding: 20

        contentItem: ColumnLayout {
            spacing: 20

            Icon {
                id: wifi
                iconWidth: Theme.iconLarge
                iconHeight: Theme.iconLarge
                icon: Qt.resolvedUrl("qrc:/assets/icons/wifi.png")
                scale: 1.0

                onClicked: {}
            }

            Icon {
                id:ethernet
                iconWidth: Theme.iconLarge
                iconHeight: Theme.iconLarge
                icon: Qt.resolvedUrl("qrc:/assets/icons/ethernet.png")
                scale: 1.0

                onClicked: {}
            }

            Icon {
                id: usb
                visible: false
                iconWidth: Theme.iconLarge
                iconHeight: Theme.iconLarge
                icon: Qt.resolvedUrl("qrc:/assets/icons/wifi.png")
                scale: 1.0

                onClicked: {}
            }

            Item {
                Layout.fillHeight: true
            }

            Icon {
                iconWidth: Theme.iconLarge
                iconHeight: Theme.iconLarge
                icon: Qt.resolvedUrl("qrc:/assets/icons/settings.png")

                onClicked: {
                    if(unlockManager.hasPassword) {
                        pageStack.push(loginPage)
                    }else {
                        pageStack.push(dashboardPage)
                    }
                }
            }
        }
    }

    Component {
        id: dashboardPage
        DashboardPage {}
    }

    Component {
        id: loginPage
        LoginPage {
            onLoginSuccess: {
                pageStack.replace(dashboardPage)
            }
        }
    }
}
