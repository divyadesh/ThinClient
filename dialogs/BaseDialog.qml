import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.3
import App.Styles 1.0
import AppSecurity 1.0

import "../pages"
import "../components"
import "../controls"

BasicPage {
    id: control
    StackView.visible: true
    property real popupHeight: 450
    property real popupWidth: 512

    property string acceptedButtonText: "Save"
    property string rejectedButtonText: "Cancel"
    property alias layout: layout
    property alias saveButton: saveButton
    property alias cancelButton: cancelButton

    // Declare default property!
    default property alias content: layout.children

    signal accepted()
    signal rejected()

    background: BackgroundOverlay {}

    Page {
        anchors.centerIn: parent
        width: control.popupWidth
        height: control.popupHeight
        background: DialogBackground{}

        header: Control {
            implicitHeight: 52
            padding: 10
            topPadding: 16

            contentItem: PrefsLabel {
                text: control.pageTitle
                font.pixelSize: 24
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        Flickable {
            anchors.fill: parent
            contentHeight: layout.height
            contentWidth: parent.width
            clip: true

            ColumnLayout {
                id: layout
                width: parent.width
                spacing: 20
            }
        }


        footer: Control {
            implicitHeight: 72

            contentItem: RowLayout {
                spacing: 20

                Item {
                    Layout.fillWidth: true
                }

                PrefsButton {
                    id: cancelButton
                    text: control.rejectedButtonText
                    radius: height / 2
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: {
                        control.rejected()
                    }
                }

                PrefsButton {
                    id: saveButton
                    text: control.acceptedButtonText
                    radius: height / 2
                    highlighted: true
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    onClicked: {
                        control.accepted()
                    }
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }
    }
}
