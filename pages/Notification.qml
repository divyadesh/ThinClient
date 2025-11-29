// Notification.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.12
import App.Backend 1.0

Popup {
    id: root
    width: Math.min(textItem.paintedWidth + 80, 350)

    // --- TOP RIGHT PLACEMENT ---
    y: 20

    // Start off-screen on the right
    x: parent.width + 20

    opacity: 0
    leftPadding: 10
    rightPadding: 10
    topPadding: 5
    bottomPadding: 5
    visible: true

    property alias message: textItem.text

    property int type: Type.Success

    property color successColor: "#2ecc71"
    property color warningColor: "#f39c12"
    property color infoColor: "#3498db"
    property color errorColor: "#e74c3c"
    property color darkBg: "#2d2f2f"

    property color iconColor: {
        switch(type) {
        case Type.Success: return successColor
        case Type.Warning: return warningColor
        case Type.Info:    return infoColor
        case Type.Error:   return errorColor
        }
    }

    // AUTO CLOSE after 5 seconds
    Timer {
        id: autoCloseTimer
        interval: 5000
        running: true
        repeat: false
        onTriggered: root.close()
    }

    // Background container
    background:Rectangle {
        id: bg
        radius: 10
        color: darkBg
        opacity: 0.95

        // Shadow
        layer.enabled: true
        layer.effect: DropShadow {
            horizontalOffset: 0
            verticalOffset: 5
            radius: 12
            samples: 20
            color: "#80000000"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: root.close()
        }
    }

    contentItem: RowLayout {
        id: row
        spacing: 10

        Rectangle {
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            id: iconBlob
            Layout.preferredWidth: 10; Layout.preferredHeight: width
            radius: width/2
            color: iconColor
        }

        // TEXT
        Text {
            id: textItem
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            text: ""
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            maximumLineCount: 2
            elide: Text.ElideRight
            color: "white"
            font.pixelSize: 14
        }
    }

    // OPEN animation
    SequentialAnimation {
        running: true

        PropertyAnimation { target: root; property: "opacity"; to: 1; duration: 150 }

        // slide into top-right corner
        PropertyAnimation {
            target: root
            property: "x"
            to: root.parent.width - root.width - 20
            duration: 300
            easing.type: Easing.OutCubic
        }
    }

    // CLOSE animation
    SequentialAnimation {
        id: closeAnim
        PropertyAnimation { target: root; property: "x"; to: parent.width + 20; duration: 250 }
        PropertyAnimation { target: root; property: "opacity"; to: 0; duration: 150 }
        ScriptAction { script: root.destroy() }
    }

    function close() {
        closeAnim.start()
    }
}
