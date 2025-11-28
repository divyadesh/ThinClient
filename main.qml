import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Window 2.15
import App.Styles 1.0
import QtQuick.VirtualKeyboard 2.15

import "pages"
import "components"


/*
| Source File          | Installed Path on Board                  | Notes                |
| -------------------- | ---------------------------------------- | -------------------- |
| `ThinClient`         | `/usr/bin/ThinClient`                    | Executable binary    |
| `deviceinfo.json`    | `/usr/share/thinclient/deviceinfo.json`  | Data/resource file   |
| `logo.png`           | `/usr/share/thinclient/logo.png`         | Your logo image      |
| `thinclient.service` | `/lib/systemd/system/thinclient.service` | Systemd service file |
*/

ApplicationWindow {
    id: window
    // FULLSCREEN & FRAMELESS
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    visibility: Window.FullScreen
    visible: true

    // OPTIONAL: If you want to avoid resize flashes
    width: Screen.width
    height: Screen.height

    // OPTIONAL: If your Qt version supports Window.Maximized
    // visibility: Window.Maximized

    // Title is hidden anyway due to frameless
    title: qsTr("G1 Thin Client PC")

    contentData: AppLayout {
        anchors.fill: parent
    }

    NotificationManager {
        id: notifier
    }

    function showAlert(message, type) {
        switch (type) {

        case NotificationItem.Type.Success:
            notifier.show(message, NotificationItem.Type.Success)
            break;

        case NotificationItem.Type.Info:
            notifier.show(message, NotificationItem.Type.Info)
            break;

        case NotificationItem.Type.Warning:
            notifier.show(message, NotificationItem.Type.Warning)
            break;

        case NotificationItem.Type.Error:
            notifier.show(message, NotificationItem.Type.Error)
            break;

        default:
            console.warn("Unknown notification type:", type)
            notifier.show(message, NotificationItem.Type.Info)
            break;
        }
    }

    InputPanel {
        id: inputPanel
        z: 99
        x: 0
        y: window.height
        width: window.width
        visible: persistData.enableOnScreenKeyboard

        states: State {
            name: "visible"
            when: inputPanel.active
            PropertyChanges {
                target: inputPanel
                y: window.height - inputPanel.height
            }
        }
        transitions: Transition {
            from: ""
            to: "visible"
            reversible: true
            ParallelAnimation {
                NumberAnimation {
                    properties: "y"
                    duration: 250
                    easing.type: Easing.InOutQuad
                }
            }
        }
    }
}
