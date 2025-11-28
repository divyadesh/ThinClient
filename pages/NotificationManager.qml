// NotificationManager.qml
import QtQuick 2.15

Item {
    id: manager
    anchors.top: parent.top
    anchors.right: parent.right
    anchors.topMargin: 10
    anchors.rightMargin: 10

    property int spacing: 8
    property int maxVisible: 2   // <-- only 2 notifications allowed

    function show(msg, type) {
        let component = Qt.createComponent("NotificationItem.qml");

        if (component.status !== Component.Ready) {
            console.log("Failed to load notification:", component.errorString());
            return;
        }

        // ⚠️ If more than maxVisible → remove oldest
        if (manager.children.length >= maxVisible) {
            let oldest = manager.children[0];
            oldest.close();      // call close() in NotificationItem.qml
        }

        let note = component.createObject(manager, {
            message: msg,
            type: type
        });

        if (!note) return;

        reposition();
    }

    function reposition() {
        let yPos = 0;

        // reposition all notifications
        for (let i = 0; i < manager.children.length; i++) {
            let child = manager.children[i];
            child.y = yPos;
            yPos += child.height + spacing;
        }
    }
}
