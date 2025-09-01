import QtQuick 2.15
import QtQuick.Controls 2.5

ComboBox {
    id: comboBox
    width: 200
    height: 30
    //anchors.centerIn: parent

    // You can also set a default selection
    currentIndex: 0  // Select the first item by default

    onActivated: {
        console.log("Selected: " + comboBox.currentText)  // Prints the selected item
    }
    // You can customize the ComboBox appearance here
    background: Rectangle {
        color: "lightblue"
        radius: 5
    }
}
