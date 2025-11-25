import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15
import QtQuick.Templates 2.15 as T
import App.Backend 1.0
import App.Styles 1.0

ComboBox {
    id: control

    property string initialTimezone: ""
    property int radius: 6

    textRole: "timeZoneFullDisplay"
    valueRole: "timeZoneId"
    width: 260

    hoverEnabled: true
    palette.buttonText: control.enabled ? "#ECECEC" : "#6B7280"   // light vs disabled gray

    onInitialTimezoneChanged: {
        currentIndex = initialTimezone ? find(initialTimezone, Qt.MatchContains) : -1
    }

    Component.onCompleted: {
        currentIndex = initialTimezone ? find(initialTimezone, Qt.MatchContains) : -1
    }

    background: Control {
        implicitWidth: 260
        implicitHeight: 40

        background: Rectangle {
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
        padding: 10
        leftPadding: 10
        rightPadding: 5
        clip: true

        background: Rectangle {
            color: Colors.btnBg
            radius: control.radius
            border.width: 1
            border.color: Colors.borderColor
        }

        contentItem: ListView {
            id: listView
            clip: true
            focus: true
            implicitHeight: Math.min(254, contentHeight + count)
            width: parent.width
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex
            highlightFollowsCurrentItem: true
            keyNavigationEnabled: true

            ScrollBar.vertical: ScrollBar {
                policy: parent.contentHeight > parent.height ? ScrollBar.AlwaysOn
                    : ScrollBar.AlwaysOff
            }

            section.property: "timeZoneDisplayOffset"

            section.delegate: Control {
                required property string section
                leftPadding: 10
                rightPadding: 10
                height: 30
                width: ListView.view.width

                contentItem: Text {
                    verticalAlignment: Text.AlignVCenter
                    text: section
                    color: "#eaeaea"
                    font.pixelSize: 14
                    font.bold: Font.DemiBold
                    elide: Text.ElideRight
                }
            }

            delegate: ItemDelegate {
                id: deleControl
                width: ListView.view.width
                height: 40
                font.pixelSize: 14

                background: Rectangle {
                    radius: 6
                    color: highlighted ? "#202123" : "transparent"
                }

                contentItem: Text {
                    text: timeZoneDisplayName    // <-- FIXED
                    color: highlighted ? "#FFFFFF" : "#ECECEC"
                    anchors.verticalCenter: parent.verticalCenter
                    leftPadding: 10
                }
            }
        }
    }
}
