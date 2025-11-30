import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.15         // Qt5 Graphical Effects
import Qt.labs.qmlmodels 1.0           // Works the same in Qt5
import QtQuick.Controls.impl 2.12
import App.Styles 1.0
import "controls"
import "dialogs"

Item {
    id: table

    readonly property int bottomOffset: 70
    readonly property int columnHeight: 60
    property var columnSource: sessionModel.columnSizes()
    property var searchFocusItem: null

    signal populateConnection(string connectionId);

    onWidthChanged: {
        tableView.forceLayout()
        headerView.forceLayout()
    }

    ButtonGroup { id: radioGroup }

    QtObject {
        id: d
        property bool tableResize: false

        function handleKeyEvent(event, tableView, paginationFocus) {
            switch (event.key) {
            case Qt.Key_Down:
                tableView.moveFocus("down")
                event.accepted = true
                break
            case Qt.Key_Up:
                tableView.moveFocus("up")
                event.accepted = true
                break
            case Qt.Key_Right:
                tableView.moveFocus("right")
                event.accepted = true
                break
            case Qt.Key_Left:
                tableView.moveFocus("left")
                event.accepted = true
                break
            case Qt.Key_Tab:
                if (paginationFocus)
                    paginationFocus.forceActiveFocus()
                else
                    tableView.nextItemInFocusChain().forceActiveFocus()
                event.accepted = true
                break
            }
        }
    }

    Component {
        id: com_column_text
        Control {
            width: Math.max(table.width / 4, 100)
            padding: 10
            rightPadding: column == 3 ? 20 : 10

            focus: false
            activeFocusOnTab: false
            focusPolicy: Qt.NoFocus
            onActiveFocusChanged: {
                if(activeFocus){
                    nextItemInFocusChain(true).forceActiveFocus()
                }
            }

            contentItem: PrefsLabel {
                text: sessionModel.headerData(column,
                                              Qt.Horizontal,
                                              Qt.DisplayRole)
                horizontalAlignment: (column == 0 || column == 3) ? (column == 3 ? Text.AlignRight : Text.AlignLeft) : Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Label.ElideRight
                font.pixelSize: 18
                font.weight: Font.DemiBold
            }
        }
    }

    // ---------- Cell delegates (per column type) ----------

    Component {
        id: name_com
        TableDelegate {
            contentItem: PrefsLabel {
                // inside TableView delegate: "model" holds the row's roles
                text: model.connectionName
                horizontalAlignment: column == 0 ? Text.AlignLeft : Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Label.ElideRight
                font.pixelSize: 16
            }
        }
    }

    Component {
        id: ip_com
        TableDelegate {
            contentItem: PrefsLabel {
                text: model.serverIp
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Label.ElideRight
                font.pixelSize: 16
            }
        }
    }

    Component {
        id: auto_com

        Control {
            id: auto_control
            width: Math.max(table.width / 4, 100)
            padding: 10
            onActiveFocusChanged: {
                if(activeFocus) {
                    radio_control.forceActiveFocus()
                }
            }

            contentItem: Item {
                RadioButton {
                    id: radio_control
                    visible: !showProgress.visible
                    anchors.centerIn: parent
                    palette.text: Colors.accentPrimary
                    palette.windowText: Colors.textPrimary
                    font.weight: Font.Normal
                    spacing: 10
                    ButtonGroup.group: radioGroup
                    checked: model.autoConnect
                    display: AbstractButton.IconOnly
                    onToggled: autoConnect = checked
                }

                PrefsBusyIndicator {
                    id: showProgress
                    anchors.centerIn: parent
                    radius: 12
                    visible: false
                }

                Connections {
                    target: serverInfo

                    // 1️⃣ Connecting started
                    function onRdpSessionStarted(id) {
                        if (id !== connectionId) return
                        showProgress.visible = true
                    }

                    // 2️⃣ Connected successfully
                    function onRdpConnected(id) {
                        if (id !== connectionId) return
                        showProgress.visible = false
                    }

                    // 3️⃣ Connection failed
                    function onRdpConnectionFailed(id, reason) {
                        if (id !== connectionId) return
                        showProgress.visible = false
                    }

                    // 4️⃣ Server disconnected
                    function onRdpDisconnected(id) {
                        if (id !== connectionId) return
                        showProgress.visible = false
                    }
                }
            }
        }
    }

    Component {
        id: manage_com
        Control {
            width: Math.max(table.width / 4, 100)
            padding: 10

            contentItem: RowLayout {
                Item { Layout.fillWidth: true }

                TableDelegate {
                    leftPadding: 4
                    rightPadding: 4
                    contentItem: PrefsLink {
                        text: qsTr("Connect")
                        onClicked: {
                            serverInfo.connectRdServer(model.connectionId)
                        }
                    }
                }

                TableDelegate {
                    leftPadding: 4
                    rightPadding: 4
                    contentItem: PrefsLink {
                        text: qsTr("Edit")
                        onClicked: {
                            table.populateConnection(model.connectionId)
                        }
                    }
                }

                TableDelegate {
                    leftPadding: 4
                    rightPadding: 4
                    contentItem: PrefsLink {
                        text: qsTr("Delete")
                        onClicked: {
                            pageStack.push(deleteConnection, {connectionId: model.connectionId})
                        }
                    }
                }
            }
        }
    }


    Component {
        id: deleteConnection
        DeleteWifiConnection {
            onSigDelete: {
                dataBase.removeServer(connectionId)
                serverInfo.removeConnection(connectionId)
            }
        }
    }

    // Delegate chooser based on columnType role from C++ model
    DelegateChooser {
        id: fullView
        role: "columnType"

        DelegateChoice {
            roleValue: "name"
            delegate: name_com
        }

        DelegateChoice {
            roleValue: "ip"
            delegate: ip_com
        }

        DelegateChoice {
            roleValue: "auto"
            delegate: auto_com
        }

        DelegateChoice {
            roleValue: "manage"
            delegate: manage_com
        }
    }

    // ---------- Header view ----------
    HorizontalHeaderView {
        id: headerView
        anchors.top: parent.top
        anchors.left: scrollArea.left
        clip: true
        syncView: tableView
        syncDirection: Qt.Horizontal
        implicitWidth: syncView ? syncView.width : 0
        implicitHeight: Math.max(1, contentHeight)
        boundsBehavior: Flickable.StopAtBounds
        columnSpacing: 0
        rowSpacing: 0
        reuseItems: true

        property int adjustDone: 0

        delegate: Item {
            id: column_item_control
            readonly property real cellPadding: 8
            property bool canceled: false
            property int columnIndex: column
            property real old_width: 0

            width: Math.max(table.width / 4, 100)
            implicitWidth: 100
            implicitHeight: table.columnHeight

            Loader {
                id: item_column_loader
                property int column: column_item_control.columnIndex

                width: parent.width
                height: parent.height
                sourceComponent: com_column_text
            }
        }
    }

    // ---------- Body area ----------
    MouseArea {
        id: scrollArea
        hoverEnabled: true
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: headerView.bottom
        anchors.bottom: parent.bottom

        TableView {
            id: tableView

            property int hoveredRow: -1
            property int tableOffset: 0
            property bool headerLoaded: false
            property var sizes: model.columnSizes()

            property int focusedRow: -1
            property int focusedColumn: -1
            property var skipColumns: []

            anchors.fill: parent

            model: sessionModel
            clip: true
            boundsBehavior: Flickable.StopAtBounds

            ScrollBar.horizontal: ScrollBar {
                id: scroll_bar_h
            }

            function nextValidColumn(current, step, max) {
                let next = current + step
                while (skipColumns.indexOf(next) !== -1 && next >= 0 && next < max) {
                    next += step
                }
                return Math.max(0, Math.min(next, max - 1))
            }

            function moveFocus(direction) {
                const rowCount = sessionModel.rowCount()
                const columnCount = sessionModel.columnCount()

                let row = Math.max(tableView.focusedRow, 0)
                let column = Math.max(tableView.focusedColumn, 0)

                switch (direction) {
                case "up":
                    row = Math.max(row - 1, 0)
                    break
                case "down":
                    row = Math.min(row + 1, rowCount - 1)
                    break
                case "left":
                    column = nextValidColumn(column, -1, columnCount)
                    break
                case "right":
                    column = nextValidColumn(column, 1, columnCount)
                    break
                }

                const index = tableView.index(row, column)
                const item = tableView.itemAtIndex(index)
                if (item) {
                    tableView.focusedRow = row
                    tableView.focusedColumn = column
                    item.forceActiveFocus()
                }
            }

            columnWidthProvider: function (column) {
                if (table.width <= 1)    // table not sized yet
                    return 100           // safe fallback

                return Math.max(table.width / 4, 100)
            }

            rowHeightProvider: function (row) {
                if (row >= sessionModel.rowCount())
                    return 0
                return table.columnHeight
            }

            // Our DelegateChooser that picks the proper component based on the "columnType" role
            delegate: fullView
        }
    }

    component PrefsLink: PrefsLabel {
        id: link
        signal clicked()

        color: linkArea.pressed || linkArea.containsMouse ? Colors.accentHover : Colors.accentPrimary
        font.underline: true
        horizontalAlignment: Label.AlignHCenter
        verticalAlignment: Label.AlignVCenter
        scale: linkArea.pressed ? 0.95 : 1.0
        elide: Label.ElideRight
        font.pixelSize: 16

        padding: 4
        leftPadding: 5
        rightPadding: 5

        MouseArea {
            id: linkArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: { link.clicked() }
        }
    }

    component TableDelegate: Control {
        id: _control
        property bool isActiveFocus: activeFocus &&
                                     (focusReason === Qt.TabFocusReason ||
                                      focusReason === Qt.BacktabFocusReason)
        width: table.width / 4
        padding: 10

        activeFocusOnTab: true
        focusPolicy: Qt.StrongFocus
        focus: true

        background: Rectangle {
            border.color: _control.isActiveFocus ? "#FFFFFF" : "transparent"
            border.width: _control.isActiveFocus ? 2 : 0
            color: "transparent"
        }
    }
}
