import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0

Control {
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

