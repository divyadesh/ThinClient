import QtQuick 2.15
import QtQuick.Controls 2.5
import "../controls"

TabBar {
    id: root
    width: parent.width
    height: 70
    spacing: 0
    background: null

    // Define your enum
    enum TabType {
        RDServer,
        Ethernet,
        Wifi,
        Display,
        Device,
        Advanced,
        About
    }

    // One signal to emit when any tab is clicked
    signal tabSelected(int tabType, string title)

    PageTabButton {
        implicitHeight: parent.height
        icon.source: "qrc:/assets/settingsIcons/ic_rdserver.svg"
        text: qsTr("RD Servers")
        onClicked: root.tabSelected(PrefsFooterLayout.TabType.RDServer, text)
    }
    PageTabButton {
        implicitHeight: parent.height
        icon.source: "qrc:/assets/settingsIcons/ic_ethernet.svg"
        text: qsTr("Ethernet")
        onClicked: root.tabSelected(PrefsFooterLayout.TabType.Ethernet, text)
    }
    PageTabButton {
        implicitHeight: parent.height
        icon.source: "qrc:/assets/settingsIcons/ic_wifi.svg"
        text: qsTr("Wifi")
        onClicked: root.tabSelected(PrefsFooterLayout.TabType.Wifi, text)
    }
    PageTabButton {
        implicitHeight: parent.height
        icon.source: "qrc:/assets/settingsIcons/ic_device.svg"
        text: qsTr("Display Settings")
        onClicked: root.tabSelected(PrefsFooterLayout.TabType.Display, text)
    }
    PageTabButton {
        implicitHeight: parent.height
        icon.source: "qrc:/assets/settingsIcons/ic_display.svg"
        text: qsTr("Device Settings")
        onClicked: root.tabSelected(PrefsFooterLayout.TabType.Device, text)
    }
    PageTabButton {
        implicitHeight: parent.height
        icon.source: "qrc:/assets/settingsIcons/advanced.svg"
        text: qsTr("Advanced Settings")
        onClicked: root.tabSelected(PrefsFooterLayout.TabType.Advanced, text)
    }
    PageTabButton {
        implicitHeight: parent.height
        icon.source: "qrc:/assets/settingsIcons/ic_about.svg"
        text: qsTr("About")
        onClicked: root.tabSelected(PrefsFooterLayout.TabType.About, text)
    }
}
