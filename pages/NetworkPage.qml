import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import App.Styles 1.0
import "../controls"

BasicPage {
    visible: true
    padding: 20

    property string wifiStatus: qsTr("Connected")
    property string wifiName: wifiNetworkDetails.activeSsid
    property string wifiIcon: wifiNetworkDetails.activeBars

    RowLayout {
        id: tabBar
        spacing: 1
        TabButton {
            id: screenTabOne
            text: "Ethernet"
            checked: true
            autoExclusive: true
            width: implicitWidth
            // background: Rectangle {
            //     //height: screenTabOne.checked ? 23 : 21
            //     color: Colors.secondaryBackground
            // }
            onClicked: {
                console.log("::::> height="+height)
                stackLayout.currentIndex = 0
            }
        }
        TabButton {
            id: screenTabTwo
            text: "Wireless"
            autoExclusive: true
            width: implicitWidth
            // background: Rectangle {
            //     //height: screenTabTwo.checked ? 23 : 21
            //     color: Colors.secondaryBackground
            // }
            onClicked: stackLayout.currentIndex = 1
        }
    }
    Rectangle {
        width: 500
        height: parent.height-93
        border.color: Colors.secondaryBackground
        border.width: 2
        color: "transparent"
        anchors { top: tabBar.bottom }

        StackLayout {
            id: stackLayout
            Layout.fillWidth: true
            anchors { top: parent.top; topMargin: 20; left: parent.left; leftMargin: 20;
                      right: parent.right; rightMargin: 20 }
            height: manualDhcpData.height

            Item {
                id: manualDhcpData
                height: ethernetRect.implicitHeight + ethernetTab.implicitHeight + 20
                width: parent.width
                RowLayout {
                    id: ethernetTab
                    spacing: 1
                    TabButton {
                        id: manualTab
                        text: "Manual"
                        checked: true
                        width: implicitWidth
                        // background: Rectangle {
                        //     //height: screenTabOne.checked ? 23 : 21
                        //     color: Colors.secondaryBackground
                        // }
                        onClicked: {
                            console.log("::::> height="+height)
                            stackLayout.currentIndex = 0
                        }
                    }
                    TabButton {
                        id: dhcpTab
                        text: "DHCP"
                        width: implicitWidth
                        // background: Rectangle {
                        //     //height: screenTabTwo.checked ? 23 : 21
                        //     color: Colors.secondaryBackground
                        // }
                        onClicked: stackLayout.currentIndex = 1
                    }
                }
                Rectangle {
                    id: ethernetRect
                    color: "transparent"
                    border.color: Colors.secondaryBackground
                    border.width: 2
                    height: ehetnetStackLayout.implicitHeight + 40
                    width: parent.width
                    anchors.top: ethernetTab.bottom
                    StackLayout {
                        id: ehetnetStackLayout
                        Layout.fillWidth: true
                        anchors { top: parent.top; topMargin: 20; left: parent.left; leftMargin: 20;
                                  right: parent.right; rightMargin: 20 }
                        EthernetManualItem {
                            id: ethManualItem
                        }
                    }
                }
            }

            //Wifi
            ColumnLayout {
                id: wifiCol
                Layout.fillWidth: true
                spacing: 25
                Rectangle {
                    Layout.fillWidth: true
                    color: Colors.accentHover
                    height: 40
                    radius: 6
                    Text {
                        anchors.centerIn: parent
                        text: wifiStatus + " | " + wifiName + " " + wifiIcon
                    }
                }
                ColumnLayout {
                    id: wifiListCol
                    Layout.fillWidth: true
                    //width: 500
                    spacing: 10
                    Rectangle {
                        Layout.fillWidth: true; height: 40; radius: 6; color: Colors.surfaceBackground
                        Text {
                            anchors.centerIn: parent
                            text: qsTr("Available Networks")
                        }
                    }
                    ListView {
                        id: availableWifiList
                        width: 470; height: 150
                        clip: true
                        model: wifiNetworkDetails
                        delegate: Item {
                            width: availableWifiList.width; height: 44;
                            Rectangle {
                                anchors.margins: 2
                                width: parent.width; height: 40; radius: 6; color: Colors.secondaryBackground
                                Item {
                                    id: ssidItem
                                    anchors { left: parent.left; right: parent.right; rightMargin: parent.width/2;
                                        verticalCenter: parent.verticalCenter }
                                    Text {
                                        anchors.centerIn: parent
                                        text: wifiDetails.ssid
                                    }
                                }
                                Item {
                                    id: barsItem
                                    anchors { left: ssidItem.right; right: parent.right; verticalCenter: parent.verticalCenter }
                                    Text {
                                        anchors.centerIn: parent
                                        text: wifiDetails.bars
                                    }
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onEntered: {
                                        parent.color = Colors.accentHover
                                    }
                                    onExited: {
                                        parent.color = Colors.secondaryBackground
                                    }
                                    onClicked: {
                                        //TODO: connect to the wifi network
                                    }
                                }
                            }
                        }
                        ScrollBar.vertical: ScrollBar {
                            policy: ScrollBar.AlwaysOn  // or AsNeeded
                        }
                    }
                }
            }
        }
    }
}
