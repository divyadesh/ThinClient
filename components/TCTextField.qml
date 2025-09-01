import QtQuick 2.15
import QtQuick.Controls 2.15

TextField {
    id: ipAddrTextField
    width: 200
    height: 30
    anchors.centerIn: parent
    placeholderText: "Enter IP address..."
    color: "white"
    background: Rectangle {
        color: ipAddrTextField.focus ? "#666" : "#444"
        radius: 4
    }
    onAccepted: {
        console.log("::::> accepted")
        ipAddrTextField.focus = false
    }
}
