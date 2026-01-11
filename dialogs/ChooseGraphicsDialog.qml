import QtQuick 2.15
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.15

import "../controls"
import "../components"

Popup {
    id: control
    property bool useAVC: true
    property bool enableAnimation: false
    property bool enableGDI: false

    parent: Overlay.overlay
    modal: true
    visible: false
    background: DialogBackground{}

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    width: Math.min(parent.width * 0.8, 480)
    padding: 20
    topPadding: 5
    bottomPadding: 5

    contentItem: Page {
        width: parent.width
        background: null

        header: Control {
            width: parent.width
            implicitHeight: 52
            padding: control.padding

            contentItem: PrefsLabel {
                text: qsTr("Select Graphics")
                font.pixelSize: 24
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        contentItem: ColumnLayout {
            spacing: 20

            Item {
                Layout.fillWidth: true
            }

            PrefsItemDelegate {
                id: graphicsDelegate
                Layout.fillWidth: true
                text: qsTr("Graphics")

                ButtonGroup { id: tabGroup }

                indicator: RowLayout {
                    x: graphicsDelegate.width - width - graphicsDelegate.rightPadding
                    y: graphicsDelegate.topPadding + (graphicsDelegate.availableHeight - height) / 2

                    spacing: 20

                    PrefsTabButton {
                        id: avcGraphics
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                        ButtonGroup.group: tabGroup
                        checked: useAVC
                        text: qsTr("AVC444")
                        visible: !!text
                        font.weight: Font.Normal
                        onToggled: {
                            useAVC = checked
                        }
                    }

                    PrefsTabButton {
                        id: rfxGraphics
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                        ButtonGroup.group: tabGroup
                        checked: !useAVC
                        text: qsTr("RFX")
                        visible: !!text
                        font.weight: Font.Normal
                        onToggled: {
                            useAVC = !checked
                        }
                    }
                }
            }

            PrefsItemDelegate {
                id: other
                Layout.fillWidth: true
                text: qsTr("Other Graphics")

                indicator: RowLayout {
                    x: other.width - width - other.rightPadding
                    y: other.topPadding + (other.availableHeight - height) / 2
                    spacing: 20

                    PrefsButton {
                        id: animation
                        checkable: true
                        text: qsTr("Animation")
                        visible: !!text
                        checked: enableAnimation
                        font.weight: Font.Normal
                        onToggled: {
                            enableAnimation = checked
                        }
                    }

                    PrefsButton {
                        id: gdiHardware
                        checkable: true
                        checked: enableGDI
                        text: qsTr("GDI:HW")
                        visible: !!text
                        font.weight: Font.Normal
                        onToggled: {
                            enableGDI = checked
                        }
                    }
                }
            }

            Item {
                Layout.fillWidth: true
            }
        }

        footer: Control {
            padding: control.padding
            implicitHeight: 72

            contentItem: RowLayout {
                spacing: 20

                Item {
                    Layout.fillWidth: true
                }

                PrefsButton {
                    text: qsTr("Close")
                    radius: height / 2
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: control.close()
                }
            }
        }
    }
}
