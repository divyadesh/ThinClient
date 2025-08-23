import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0

import "../controls"

Page {
    background: Image {
        //width : height = 3 : 2 Aspect ratio
        source: Qt.resolvedUrl("qrc:/assets/images/background.jpg")
    }

    Control {
        anchors.left: parent.left
        anchors.top: parent.top
        padding: 20

        contentItem: ColumnLayout {
            spacing: 0

            Image {
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                source: Qt.resolvedUrl("qrc:/assets/images/logo.png")
                fillMode: Image.PreserveAspectFit
            }

            RowLayout {
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                Layout.leftMargin: 10

                Text {
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter

                    text: "G"
                    color: Colors.accentHover
                    font.pixelSize: Fonts.body
                }

                Text {
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter

                    text: "for"
                    color: Colors.black
                    font.pixelSize: Fonts.body
                }

                Text {
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter

                    text: "Green Computing ..."
                    color: Colors.accentHover
                    font.pixelSize: Fonts.body
                }
            }
        }
    }

    Control {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        padding: 20

        contentItem: ColumnLayout {

            Icon {
                iconWidth: Theme.iconLarge
                iconHeight: Theme.iconLarge
                icon: Qt.resolvedUrl("qrc:/assets/icons/ic_wifi.svg")

                onClicked: {}
            }

            Item {
                Layout.fillHeight: true
            }

            Icon {
                iconWidth: Theme.iconLarge
                iconHeight: Theme.iconLarge
                icon: Qt.resolvedUrl("qrc:/assets/icons/ic_settings.svg")

                onClicked: {}
            }
        }
    }
}
