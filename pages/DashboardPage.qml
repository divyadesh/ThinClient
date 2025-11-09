import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import App.Styles 1.0

import "../components"
import "../controls"

BasicPage {
    id: root
    property bool server: false

    background: Rectangle {
        color: Colors.secondaryBackground
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        BasicPage {
            Layout.fillWidth: true
            Layout.fillHeight: true

            background: Rectangle {
                color: "#333333"
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
                    pageStack.replace(serverSettingsPage, {pageTitle: title })
                    break
                case PrefsFooterLayout.TabType.Ethernet:
                    pageStack.replace(networkPage, {pageTitle: title })
                    break
                case PrefsFooterLayout.TabType.Wifi:
                    pageStack.replace(networkPage, {pageTitle: title })
                    break
                case PrefsFooterLayout.TabType.Display:
                    pageStack.replace(displayPage, {pageTitle: title })
                    break
                case PrefsFooterLayout.TabType.Device:
                    pageStack.replace(deviceSettingsPage, {pageTitle: title })
                    break
                case PrefsFooterLayout.TabType.About:
                    pageStack.replace(deviceInfoPage, {pageTitle: title })
                    break
                default:
                    pageStack.replace(serverSettingsPage, {pageTitle: title })
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
        id: networkPage
        NetworkPage {
            onBackToHome: { root.pageStack.pop() }
        }
    }
}
