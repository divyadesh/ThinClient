import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3

Flickable {
    id: formFlickable
    width: parent.width
    clip: true
    property real originalContentY: 0

    function ensureVisible(item) {
        if(item && item.readOnly || !persistData.enableOnScreenKeyboard) {
            return
        }

        const margin = 20

        // Convert item position inside Flickable
        const itemY = item.mapToItem(formFlickable.contentItem, 0, 0).y
        const itemBottom = itemY + item.height + margin

        const visibleBottom = formFlickable.contentY + formFlickable.height - panelHeight

        // If the field is below the visible area → scroll up
        if (itemBottom > visibleBottom) {
            formFlickable.contentY = itemBottom - (formFlickable.height - panelHeight)
        }

        // If the field is above the visible area → scroll down
        if (itemY < formFlickable.contentY) {
            formFlickable.contentY = itemY - margin
        }
    }

    Connections {
        target: inputPanel

        function onActiveChanged() {
            if (inputPanel.active) {
                // Save original scroll before keyboard moves it
                originalContentY = formFlickable.contentY
            } else {
                // Reset to original scroll position
                formFlickable.contentY = originalContentY
            }
        }
    }
}
