import QtQuick 2.15
import QtQuick.Controls 2.15

TextField {
    id: ipAddrTextField
    anchors.centerIn: parent
    placeholderText: "Enter IP address..."
    color: "white"

    background: Rectangle {
        implicitWidth: 200
        implicitHeight: 40
        color: ipAddrTextField.focus ? "#666" : "#444"
        radius: 6
    }

    onAccepted: {
        console.log("::::> accepted")
        ipAddrTextField.focus = false
    }
}
