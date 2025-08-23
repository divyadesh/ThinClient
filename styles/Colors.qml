// qml/styles/Colors.qml
pragma Singleton
import QtQuick 2.15

QtObject {
    // 🌑 Backgrounds
    property color primaryBackground: "#1E1E1E"    // main app background
    property color secondaryBackground: "#2A2A2A"  // side menus, panels
    property color surfaceBackground: "#353935"    // cards, text fields

    // 🔠 Text
    property color textPrimary: "#FFFFFF"      // main titles & body
    property color textSecondary: "#B0B0B0"    // subtitles, hints
    property color textDisabled: "#6B6B6B"     // disabled states
    property color black: "#000000"

    // 🟩 Accent (Buttons)
    property color accentPrimary: "#27AE60"    // active button / selected tab
    property color accentHover: "#2ECC71"      // hover / pressed
    property color accentDisabled: "#144D2B"   // disabled accent

    // 🔲 Inputs / TextFields
    property color inputBackground: "#3A3A3A"    // text field background
    property color inputBorder: "#555555"        // neutral border
    property color inputFocus: "#27AE60"         // focus border highlight
    property color inputText: "#FFFFFF"          // typed text
    property color placeholderText: "#AAAAAA"    // placeholder / hints

    // ⚠️ Status colors
    property color statusSuccess: "#27AE60"  // success, OK
    property color statusWarning: "#F2C94C"  // warning, yellow
    property color statusError: "#E74C3C"    // error, red
    property color statusInfo: "#3498DB"     // info, blue

    readonly property color transparent: "transparent"
}
