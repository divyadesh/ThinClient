import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0

import "../pages"
import "../controls"

Control {
    id: control
    leftPadding: 20
    rightPadding: 20
    property string pageTitle: ""

    signal backPressed()

    background: Rectangle {
        implicitHeight: 84
        color: Colors.headerColor
    }

    contentItem: RowLayout {
        spacing: 20

        Control {
            id: back_control
            property bool isActiveFocus: activeFocus &&
                                         (focusReason === Qt.TabFocusReason ||
                                          focusReason === Qt.BacktabFocusReason)
            padding: 5
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            activeFocusOnTab: true
            focusPolicy: Qt.StrongFocus
            focus: true

            background: Rectangle {
                border.color: back_control.isActiveFocus ? "#FFFFFF" : "transparent"
                border.width: back_control.isActiveFocus ? 2 : 0
                color: "transparent"
            }

            contentItem: Image {
                scale: mouseArea.pressed ? 0.95 : 1.0
                sourceSize: Qt.size(38, 38)
                source: Qt.resolvedUrl("qrc:/assets/icons/ic_back.svg")
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                onClicked: control.backPressed()
            }
        }

        PrefsLabel {
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            text: control.pageTitle
            font.pixelSize: 28
            font.weight: Font.Bold
        }

        Item { Layout.fillWidth: true }
    }
}
