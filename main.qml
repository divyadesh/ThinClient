import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Window 2.15
import App.Styles 1.0
import QtQuick.VirtualKeyboard 2.15
import App.Backend 1.0

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

    property real panelHeight: inputPanel.height
    property alias inputPanel: inputPanel
    property var activeNotification: null
    // OPTIONAL: If your Qt version supports Window.Maximized
    // visibility: Window.Maximized

    // Title is hidden anyway due to frameless
    title: qsTr("G1 Thin Client PC")

    contentData: AppLayout {
        anchors.fill: parent
    }

    function notify(msg, type) {

        // 1️⃣ Close previous notification if exists
        if (activeNotification) {
            console.log("Closing previous notification...")
            activeNotification.close()   // triggers close animation & destroy
            activeNotification = null
        }

        // 2️⃣ Create new component
        let component = Qt.createComponent("qrc:/pages/Notification.qml");

        if (component.status !== Component.Ready) {
            console.error("Failed to load notification:", component.errorString());
            return;
        }

        let note = component.createObject(window, {
            message: msg,
            type: type
        });

        if (!note) {
            console.error("Failed to create notification instance");
            return;
        }

        // 3️⃣ Store reference
        activeNotification = note

        console.log("Notification created:", msg)
    }

    function showAlert(message, type) {
        switch (type) {

        case Type.Success:
            notify(message, Type.Success)
            break;

        case Type.Info:
            notify(message, Type.Info)
            break;

        case Type.Warning:
            notify(message, Type.Warning)
            break;

        case Type.Error:
            notify(message, Type.Error)
            break;

        default:
            console.warn("Unknown notification type:", type)
            notify(message, Type.Info)
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
