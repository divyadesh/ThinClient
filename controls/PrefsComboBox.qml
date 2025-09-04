import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls.impl 2.15
import App.Styles 1.0

ComboBox {
    id: control
    width: 260
    property int radius: 6
    hoverEnabled: true
    scale: control.pressed ? 0.99 : 1.0

    // Text color
    palette.buttonText: control.enabled ? "#ECECEC" : "#6B7280"   // light vs disabled gray

    background: Rectangle {
        implicitWidth: 260
        implicitHeight: 36
        radius: control.radius
        color: Colors.btnBg
        border.width: 1
        border.color: control.activeFocus ? Colors.accentPrimary : Colors.transparent
    }

    indicator: ColorImage {
        x: control.width - width - control.padding - 5
        y: control.topPadding + (control.availableHeight - height) / 2
        color: control.palette.dark
        defaultColor: "#353637"
        sourceSize: Qt.size(24, 24)
        source: "qrc:/assets/icons/down.svg"
        opacity: enabled ? 1 : 0.3
    }

    // Drop-down popup
    popup: Popup {
        y: control.height
        width: control.width

        background: Rectangle {
            color: "#343541"       // same as combo background
            radius: control.radius
            border.width: 1
            border.color: "#565869"
        }

        contentItem: ListView {
            clip: true
            implicitHeight: Math.min(contentHeight, 254)
            model: control.delegateModel
            currentIndex: control.highlightedIndex

            delegate: ItemDelegate {
                width: control.width
                text: modelData
                font.pixelSize: 14

                background: Rectangle {
                    radius: 6
                    color: highlighted ? "#202123" : "transparent"   // ChatGPT sidebar highlight
                }

                contentItem: Text {
                    text: modelData
                    color: highlighted ? "#FFFFFF" : "#ECECEC"
                    anchors.verticalCenter: parent.verticalCenter
                    leftPadding: 10
                }
            }
        }
    }
}

