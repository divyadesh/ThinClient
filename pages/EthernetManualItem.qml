import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import App.Styles 1.0

import "../controls"
import "../components"

Item {
    width: parent.width
    height: iPDataCol.implicitHeight + infoEthernetCol.implicitHeight + 20
    ColumnLayout {
        id: iPDataCol
        RowLayout {
            spacing: 30
            Rectangle {
                height: 30
                width: 150
                color: Colors.secondaryBackground
                radius: 6
                Text {
                    anchors.centerIn: parent
                    text: qsTr("Thin Client IP")
                }
            }
            Rectangle {
                height: 30
                width: 200
                color: Colors.secondaryBackground
                radius: 6
                TCTextField {
                    id: ipAddrTextField
                }
            }
        }
        RowLayout {
            spacing: 30
            Rectangle {
                height: 30
                width: 150
                color: Colors.secondaryBackground
                radius: 6
                Text {
                    anchors.centerIn: parent
                    text: qsTr("Subnet mask")
                }
            }
            Rectangle {
                height: 30
                width: 200
                color: Colors.secondaryBackground
                radius: 6
                TCTextField {
                    id: subnetMaskTextField
                }
            }
        }
        RowLayout {
            spacing: 30
            Rectangle {
                height: 30
                width: 150
                color: Colors.secondaryBackground
                radius: 6
                Text {
                    anchors.centerIn: parent
                    text: qsTr("Gateway")
                }
            }
            Rectangle {
                height: 30
                width: 200
                color: Colors.secondaryBackground
                radius: 6
                TCTextField {
                    id: gatewayTextField
                }
            }
        }
    }
    ColumnLayout {
        id: infoEthernetCol
        Layout.fillWidth: true
        anchors.top: iPDataCol.bottom
        anchors.topMargin: 20
        spacing: 20
        RowLayout {
            id: infoEthernet
            spacing: 30
            Rectangle {
                height: 30
                width: 150
                color: Colors.secondaryBackground
                radius: 6
                Text {
                    anchors.centerIn: parent
                    text: qsTr("Ethernet Mac")
                }
            }
            Rectangle {
                height: 30
                width: 150
                color: Colors.secondaryBackground
                radius: 6
                TextInput {
                    anchors.centerIn: parent
                    text: "Auto read"
                }
            }
        }
        PageTabButton {
            id: saveBttn
            Layout.alignment: Qt.AlignLeft
            Layout.preferredWidth: 150
            text: qsTr("Save")
            checked: true
            //onClicked: ethWifiStack.replace(ethWifiDetails) TODO
        }
    }
}
