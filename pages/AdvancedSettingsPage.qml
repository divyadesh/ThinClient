import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0
import App.Enums 1.0
import G1.ThinClient 1.0
import App.Backend 1.0

import "../components"
import "../controls"
import "../dialogs"

BasicPage {
    id: page
    StackView.visible: true
    padding: 20

    header: PageHeader {
        pageTitle: page.pageTitle
        onBackPressed: {
            if(pageStack.depth === 1) {
                backToHome()
                return
            }
            pageStack.pop()
        }
    }

    ButtonGroup { id: tabGroup }

    contentItem: PrefsFlickable {
        id: formFlickable
        contentHeight: layout.height
        contentWidth: layout.width

        ColumnLayout {
            id: layout
            width: page.width - 40
            spacing: 40
            clip: true

            Control {
                Layout.fillWidth: true
                padding: 20

                contentItem: ColumnLayout {
                    spacing: 10

                    PrefsItemDelegate {
                        id: gfxOptions
                        Layout.fillWidth: true
                        text: qsTr("Select GFX Mode")

                        indicator: RowLayout {
                            x: gfxOptions.width - width - gfxOptions.rightPadding
                            y: gfxOptions.topPadding + (gfxOptions.availableHeight - height) / 2
                            spacing: 20

                            PrefsTabButton {
                                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                                ButtonGroup.group: tabGroup
                                checked: persistData.gfxMode === "auto"
                                text: qsTr("Auto")
                                visible: !!text
                                font.weight: Font.Normal
                                onToggled: {
                                    if (persistData.gfxMode !== "auto") {
                                        persistData.gfxMode = "auto"
                                    }
                                }
                            }

                            PrefsTabButton {
                                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                                ButtonGroup.group: tabGroup
                                checked: persistData.gfxMode === "avc444"
                                text: qsTr("AVC444")
                                visible: !!text
                                font.weight: Font.Normal
                                onToggled: {
                                    if (persistData.gfxMode !== "avc444") {
                                        persistData.gfxMode = "avc444"
                                    }
                                }
                            }

                            PrefsTabButton {
                                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                                ButtonGroup.group: tabGroup
                                checked: persistData.gfxMode === "gfx"
                                text: qsTr("GFX")
                                visible: !!text
                                font.weight: Font.Normal
                                onToggled: {
                                    if (persistData.gfxMode !== "gfx") {
                                        persistData.gfxMode = "gfx"
                                    }
                                }
                            }
                        }
                    }

                    PrefsItemDelegate {
                        id: menuAnimation
                        Layout.fillWidth: true
                        text: qsTr("Enable Menu Animation")

                        indicator: PrefsButton {
                            x: menuAnimation.width - width - menuAnimation.rightPadding
                            y: menuAnimation.topPadding + (menuAnimation.availableHeight - height) / 2
                            text: qsTr("Enabled")
                            checkable: true
                            checked: persistData.menuAnimation
                            onToggled: persistData.menuAnimation = checked
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 30
                    }
                }
            }
        }
    }
}
