import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0
import App.Backend 1.0

import "../components"
import "../controls"

BasicPage {
    id: page
    pageTitle: qsTr("Add Custom Resolution")
    padding: 20

    // Tracking user inputs
    property int resolutionWidth: 1920
    property int resolutionHeight: 1080

    signal created()

    function log(msg) {
        console.log("[ResolutionPage] " + msg)
    }

    header: PageHeader {
        pageTitle: page.pageTitle
        onBackPressed: {
            page.log("Back pressed")
            pageStack.pop()
        }
    }

    contentItem: Flickable {
        width: parent.width
        clip: true
        contentHeight: layout.height
        contentWidth: parent.width

        ColumnLayout {
            id: layout
            width: page.width - 40
            spacing: 40
            clip: true

            Control {
                Layout.fillWidth: true
                padding: 20

                contentItem: ColumnLayout {
                    spacing: 20

                    /* ------------------------------
                     * Resolution Input Block
                     * ------------------------------ */
                    PrefsItemDelegate {
                        id: resolution
                        Layout.fillWidth: true
                        text: qsTr("Input Resolution")

                        indicator: RowLayout {
                            x: resolution.width - width - resolution.rightPadding
                            y: resolution.topPadding + (resolution.availableHeight - height) / 2
                            spacing: 10

                            /* Width input */
                            SpinBox {
                                id: id_width
                                from: resolutionListModel.minimumSize.width
                                to: resolutionListModel.maximumSize.width
                                value: page.resolutionWidth
                                editable: true
                                inputMethodHints: Qt.ImhDigitsOnly

                                palette.text: "black"
                                onValueModified: {
                                    page.log("Width changed → " + value +
                                             " (range: " + from + "–" + to + ")")
                                    resolutionListModel.hasError = false
                                    resolutionListModel.errorMessage = ""
                                }

                                validator: IntValidator {
                                    locale: id_width.locale.name
                                    bottom: Math.min(id_width.from, id_width.to)
                                    top: Math.max(id_width.from, id_width.to)
                                }
                            }

                            /* Height input */
                            SpinBox {
                                id: id_height
                                from: resolutionListModel.minimumSize.height
                                to: resolutionListModel.maximumSize.height
                                value: page.resolutionHeight
                                editable: true
                                palette.text: "black"
                                inputMethodHints: Qt.ImhDigitsOnly

                                onValueModified: {
                                    page.log("Height changed → " + value +
                                             " (range: " + from + "–" + to + ")")
                                    resolutionListModel.hasError = false
                                    resolutionListModel.errorMessage = ""
                                }

                                validator: IntValidator {
                                    locale: id_height.locale.name
                                    bottom: Math.min(id_height.from, id_height.to)
                                    top: Math.max(id_height.from, id_height.to)
                                }
                            }
                        }
                    }

                    /* ------------------------------
                     * Save Custom Resolution Block
                     * ------------------------------ */
                    PrefsItemDelegate {
                        id: addCustomResolution
                        Layout.fillWidth: true
                        implicitHeight: resolution.implicitHeight
                        text: resolutionListModel.errorMessage
                        palette.text: Colors.statusError

                        background: Item {
                            implicitWidth: 100
                            implicitHeight: 40
                        }

                        indicator: PrefsButton {
                            x: addCustomResolution.width - width - addCustomResolution.rightPadding
                            y: addCustomResolution.topPadding + (addCustomResolution.availableHeight - height) / 2
                            highlighted: true
                            enabled: !resolutionListModel.hasError
                            text: qsTr("Save")

                            onClicked:  {
                                page.log("Save clicked → attempting to add resolution " +
                                         id_width.value + " x " + id_height.value)

                                const success = resolutionListModel.addCustomResolution(
                                    id_width.value,
                                    id_height.value,
                                    60  // default refresh rate
                                )

                                if (success) {
                                    page.log("✔ Inserted successfully into model & DB.")
                                    page.created()
                                    pageStack.pop()
                                } else {
                                    page.log("✘ Insert failed → " +
                                             resolutionListModel.errorMessage)
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
