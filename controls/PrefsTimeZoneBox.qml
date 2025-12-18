import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls.impl 2.15
import App.Styles 1.0
import QtGraphicalEffects 1.0

ComboBox {
    id: control
    width: 260
    property int radius: 6
    hoverEnabled: true

    palette.buttonText: control.enabled ? "#ECECEC" : "#6B7280"

    background: Item {
        implicitWidth: 260
        implicitHeight: 40

        Rectangle {
            anchors.fill: parent
            radius: 8
            color: Colors.btnBg
            border.width: control.activeFocus ? 2 : 1
            border.color: control.activeFocus ? Colors.accentPrimary : Colors.secondaryBackground
        }
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

    popup: Popup {
        y: control.height
        width: control.width
        implicitHeight: Math.min(listView.contentHeight, 260)
        padding: 0

        background: Rectangle {
            color: Colors.btnBg
            radius: control.radius
            border.width: 1
            border.color: Colors.borderColor
        }

        contentItem: ListView {
            id: listView
            clip: true
            model: control.delegateModel
            currentIndex: control.highlightedIndex
            interactive: true

            // // --- SECTION HEADER SUPPORT ---
            section.property: "utcOffset"         // use UTC offset from model
            section.criteria: ViewSection.FullString
            section.delegate: Control {
                width: listView.width
                leftPadding: 1
                rightPadding: 1
                background: Rectangle {
                    implicitHeight: 32
                    color: Colors.btnBgDisabled
                }

                contentItem: Text {
                    text: section                // "UTC+05:30", etc.
                    anchors.verticalCenter: parent.verticalCenter
                    leftPadding: 10
                    font.bold: true
                    color: Colors.accentPrimary
                    font.pixelSize: 14
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignLeft
                }
            }

            delegate: ItemDelegate {
                width: listView.width
                visible: !isSection
                text: tzName
                font.pixelSize: 13

                background: Rectangle {
                    radius: 6
                    color: highlighted ? "#202123" : "transparent"
                }

                contentItem: Text {
                    text: tzName
                    color: highlighted ? "#FFFFFF" : "#ECECEC"
                    anchors.verticalCenter: parent.verticalCenter
                    leftPadding: 16
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignLeft
                }

                onClicked: {
                    control.currentIndex = index
                    control.activated(index)
                    control.popup.close()
                }
            }
        }
    }
}
