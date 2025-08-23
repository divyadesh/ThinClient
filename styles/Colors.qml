// qml/styles/Colors.qml
pragma Singleton
import QtQuick 2.15

QtObject {
    // 🌑 Backgrounds
    property color primaryBackground: Qt.color("#1E1E1E")    // main app background
    property color secondaryBackground: Qt.color("#2A2A2A")  // side menus, panels
    property color surfaceBackground: Qt.color("#353935")    // cards, text fields

    // 🔠 Text
    property color textPrimary: Qt.color("#FFFFFF")      // main titles & body
    property color textSecondary: Qt.color("#B0B0B0")    // subtitles, hints
    property color textDisabled: Qt.color("#6B6B6B")     // disabled states

    // 🟩 Accent (Buttons)
    property color accentPrimary: Qt.color("#27AE60")    // active button / selected tab
    property color accentHover: Qt.color("#2ECC71")      // hover / pressed
    property color accentDisabled: Qt.color("#144D2B")   // disabled accent

    // 🔲 Inputs / TextFields
    property color inputBackground: Qt.color("#3A3A3A")    // text field background
    property color inputBorder: Qt.color("#555555")        // neutral border
    property color inputFocus: Qt.color("#27AE60")         // focus border highlight
    property color inputText: Qt.color("#FFFFFF")          // typed text
    property color placeholderText: Qt.color("#AAAAAA")    // placeholder / hints

    // ⚠️ Status colors
    property color statusSuccess: Qt.color("#27AE60")  // success, OK
    property color statusWarning: Qt.color("#F2C94C")  // warning, yellow
    property color statusError: Qt.color("#E74C3C")    // error, red
    property color statusInfo: Qt.color("#3498DB")     // info, blue
}
