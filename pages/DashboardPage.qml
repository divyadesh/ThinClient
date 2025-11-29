import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import App.Styles 1.0
import App.Backend 1.0

import "../components"
import "../controls"

BasicPage {
    id: root
    property bool server: false

    background: Rectangle {
        color: "#0F0E13"// Colors.secondaryBackground
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        BasicPage {
            Layout.fillWidth: true
            Layout.fillHeight: true

            background: Rectangle {
                color: "#0F0E13"//"#333333"
            }

            contentItem: PrefsStackView {
                id: pageStack

                Component.onCompleted:  {
                    pageStack.replace(serverSettingsPage, { pageTitle: qsTr("RD Server") })
                }
            }
        }

        PrefsFooterLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 90
            onTabSelected: function(tabType, title) {
                switch (tabType) {
                case PrefsFooterLayout.TabType.RDServer:
                    pageStack.replace(null, serverSettingsPage, {pageTitle: title })
                    break
                case PrefsFooterLayout.TabType.Ethernet:
                    pageStack.replace(null,ethernetnetworkPage, {pageTitle: title })
                    break
                case PrefsFooterLayout.TabType.Wifi:
                    pageStack.replace(null,wifinetworkPage, {pageTitle: title })
                    break
                case PrefsFooterLayout.TabType.Display:
                    pageStack.replace(null,displayPage, {pageTitle: title })
                    break
                case PrefsFooterLayout.TabType.Device:
                    pageStack.replace(null,deviceSettingsPage, {pageTitle: title })
                    break
                case PrefsFooterLayout.TabType.About:
                    pageStack.replace(null,deviceInfoPage, {pageTitle: title })
                    break
                default:
                    pageStack.replace(null,serverSettingsPage, {pageTitle: title })
                }
            }
        }
    }

    Component {
        id: deviceInfoPage
        DeviceInfoPage {
            onBackToHome: { root.pageStack.pop() }
        }
    }

    Component {
        id: deviceSettingsPage
        DeviceSettingsPage {
            onBackToHome: { root.pageStack.pop() }
        }
    }

    Component {
        id: serverSettingsPage
        ServerSettingsPage {
            onBackToHome: { root.pageStack.pop() }
        }
    }

    Component {
        id: displayPage
        DisplayPage {
            onBackToHome: { root.pageStack.pop() }
        }
    }

    Component {
        id: ethernetnetworkPage
        EthernetNetworkPage {
            onBackToHome: { root.pageStack.pop() }
        }
    }
    Component {
        id: wifinetworkPage
        WifiNetworkPage {
            onBackToHome: { root.pageStack.pop() }
        }
    }
}
