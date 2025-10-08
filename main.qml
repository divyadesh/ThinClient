import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Window 2.15
import App.Styles 1.0

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
    width: ScreenConfig.screenWidth
    height: ScreenConfig.screenHeight
    visible: true
    visibility: Window.FullScreen
    flags: Qt.WindowStaysOnTopHint
    title: qsTr("G1 Thin Client pc")

    contentData: AppLayout {
        anchors.fill: parent
    }
}
