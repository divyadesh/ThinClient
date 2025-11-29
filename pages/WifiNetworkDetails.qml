import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import App.Styles 1.0
import App.Enums 1.0
import App.Backend 1.0

import "../controls"
import "../components"
import "../dialogs"

BasicPage {
    id: page
    StackView.visible: true
    pageTitle: qsTr("Network Details")
    padding: 20

    background: Rectangle {
        color: "#0F0E13"
    }

    header: PageHeader {
        pageTitle: page.pageTitle
        onBackPressed: pageStack.pop()
    }

    contentItem: PrefsFlickable {
        id: formFlickable
        contentHeight: layout.implicitHeight
        contentWidth: layout.width

        ColumnLayout {
            id: layout
            width: page.width - 40
            spacing: 20
            clip: true

            PrefsDelegate {
                Layout.fillWidth: true
                title: qsTr("Status")
                subTitle: qsTr("Connected")
            }

            PrefsDelegate {
                Layout.fillWidth: true
                title: qsTr("Signal Strength")
                subTitle: qsTr("Connected")
            }

            PrefsDelegate {
                Layout.fillWidth: true
                title: qsTr("Connection speed")
                subTitle: qsTr("Connected")
            }

            PrefsDelegate {
                Layout.fillWidth: true
                title: qsTr("Security")
                subTitle: qsTr("Connected")
            }

            PrefsDelegate {
                Layout.fillWidth: true
                title: qsTr("Mac address")
                subTitle: qsTr("Connected")
            }

            PrefsDelegate {
                Layout.fillWidth: true
                title: qsTr("IPv4 address")
                subTitle: qsTr("Connected")
            }

            PrefsOptionSelector {
                Layout.fillWidth: true
                title: qsTr("IPv4 settings")
            }

            PrefsItemDelegate {
                id: staticIpAddress
                Layout.fillWidth: true
                text: qsTr("IPv4 address")

                indicator: PrefsTextField {
                    id: staticIpAddressField
                    x: staticIpAddress.width - width - staticIpAddress.rightPadding
                    y: staticIpAddress.topPadding + (staticIpAddress.availableHeight - height) / 2

                    text: ""
                    validator: RegularExpressionValidator {
                        regularExpression:  /^((?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\.){0,3}(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])$/
                    }

                    onActiveFocusChanged: {
                        if (activeFocus) {
                            // Ensures the field is visible above the keyboard
                            formFlickable.ensureVisible(staticIpAddressField)
                        }
                    }
                }
            }

            PrefsItemDelegate {
                id: staticIpSubNetMask
                Layout.fillWidth: true
                text: qsTr("IPv4 Subnet")

                indicator: PrefsTextField {
                    id: staticIpSubNetMaskField
                    x: staticIpSubNetMask.width - width - staticIpSubNetMask.rightPadding
                    y: staticIpSubNetMask.topPadding + (staticIpSubNetMask.availableHeight - height) / 2

                    text: ""
                    inputMask: "000.000.000.000"
                    onActiveFocusChanged: {
                        if (activeFocus) {
                            // Ensures the field is visible above the keyboard
                            formFlickable.ensureVisible(staticIpSubNetMaskField)
                        }
                    }
                }
            }

            PrefsItemDelegate {
                id: staticIpGateWay
                Layout.fillWidth: true
                text: qsTr("IPv4 Gatway")

                indicator: PrefsTextField {
                    id: staticIpGateWayField
                    x: staticIpGateWay.width - width - staticIpGateWay.rightPadding
                    y: staticIpGateWay.topPadding + (staticIpGateWay.availableHeight - height) / 2

                    text: ""
                    inputMask: "000.000.000.000"
                    onActiveFocusChanged: {
                        if (activeFocus) {
                            // Ensures the field is visible above the keyboard
                            formFlickable.ensureVisible(staticIpGateWay)
                        }
                    }
                }
            }

            PrefsItemDelegate {
                id: staticIpDNS
                Layout.fillWidth: true
                text: qsTr("IPv4 DNS")

                indicator: PrefsTextField {
                    id: staticIpDNSField
                    x: staticIpDNS.width - width - staticIpDNS.rightPadding
                    y: staticIpDNS.topPadding + (staticIpDNS.availableHeight - height) / 2

                    inputMask: "0.0.0.0"
                    text: ""
                    onActiveFocusChanged: {
                        if (activeFocus) {
                            // Ensures the field is visible above the keyboard
                            formFlickable.ensureVisible(staticIpDNS)
                        }
                    }
                }
            }
        }
    }

    component PrefsDelegate: PrefsItemDelegate {
        id: control
        property string title: ""
        property string subTitle: ""
        padding: 8
        radius: 3

        contentItem: ColumnLayout {
            PrefsLabel {
                id: titleText
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                text: control.title
                font.pixelSize: control.font.pixelSize
                color: Colors.textPrimary
            }

            PrefsLabel {
                id: subTitleText
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                text: control.subTitle
                font.pixelSize: control.font.pixelSize - 2
                color: Colors.textSecondary
            }
        }
    }

    component PrefsOptionSelector: PrefsItemDelegate {
        id: selector
        property string title: ""
        padding: 8
        radius: 3

        contentItem: ColumnLayout {
            PrefsLabel {
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                text: selector.title
                font.pixelSize: control.font.pixelSize
                color: Colors.textPrimary
            }

            PrefsComboBox {
                id: comboxControl
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                Layout.fillWidth: true
                model: ["DHCP", "Static"]
                background: null
                indicator: null

                contentItem: PrefsLabel {
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    text: comboxControl.displayText
                    font.pixelSize: selector.font.pixelSize - 2
                    color: Colors.accentPrimary
                    font.underline: true
                }
            }
        }
    }
}
