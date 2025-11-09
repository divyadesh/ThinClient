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
        About
    }

    // One signal to emit when any tab is clicked
    signal tabSelected(int tabType, string title)

    PageTabButton {
        implicitHeight: parent.height
        icon.source: "qrc:/icons/fa--dashboard.svg"
        text: qsTr("RD Servers")
        onClicked: root.tabSelected(PrefsFooterLayout.TabType.RDServer, text)
    }
    PageTabButton {
        implicitHeight: parent.height
        icon.source: "qrc:/icons/fa-solid--fan.svg"
        text: qsTr("Ethernet")
        onClicked: root.tabSelected(PrefsFooterLayout.TabType.Ethernet, text)
    }
    PageTabButton {
        implicitHeight: parent.height
        icon.source: "qrc:/icons/lamp-svgrepo-com.svg"
        text: qsTr("Wifi")
        onClicked: root.tabSelected(PrefsFooterLayout.TabType.Wifi, text)
    }
    PageTabButton {
        implicitHeight: parent.height
        icon.source: "qrc:/icons/refrigerator-free-4-svgrepo-com.svg"
        text: qsTr("Display Settings")
        onClicked: root.tabSelected(PrefsFooterLayout.TabType.Display, text)
    }
    PageTabButton {
        implicitHeight: parent.height
        icon.source: "qrc:/icons/maki--communications-tower.svg"
        text: qsTr("Device Settings")
        onClicked: root.tabSelected(PrefsFooterLayout.TabType.Device, text)
    }
    PageTabButton {
        implicitHeight: parent.height
        icon.source: "qrc:/icons/wpf--security-checked.svg"
        text: qsTr("About")
        onClicked: root.tabSelected(PrefsFooterLayout.TabType.About, text)
    }
}
