// qml/styles/Theme.qml
// Centralized Theme configuration for the entire application
// Import this singleton anywhere in QML to maintain consistent styling

pragma Singleton
import QtQuick 2.9
import "."   // Imports Colors.qml and Fonts.qml from the same folder

QtObject {
    // Colors and Typography

    // Spacing system (used for margins, paddings, gaps)
    // These values scale consistently across the UI
    property int paddingSmall: 8     // e.g., tight spacing inside buttons
    property int paddingMedium: 16   // e.g., default padding for panels/cards
    property int paddingLarge: 24    // e.g., spacing between major sections

    // 📐 Spacing (margins, paddings, gaps)
    property int spacingXS: 4
    property int spacingS: 8
    property int spacingM: 16
    property int spacingL: 24
    property int spacingXL: 32

    // 🔲 Radius (applied across UI elements)
    property int radiusSmall: 6
    property int radiusMedium: 12
    property int radiusLarge: 20

    // ⏺ Icon Sizes
    property int iconSmall: 16 * ScreenConfig.scale
    property int iconMedium: 24 * ScreenConfig.scale
    property int iconLarge: 32 * ScreenConfig.scale
    property int iconExtraLarge: 64 * ScreenConfig.scale

    // 🔘 Buttons (based on Material Design + HMI usability)
    property int buttonHeight: 40   // min height
    property int buttonMinWidth: 100
    property int buttonRadius: radiusMedium

    // 🔤 Inputs / Text Fields
    property int inputHeight: 36
    property int inputMinWidth: 200
    property int inputRadius: radiusSmall

    // 🖥️ General Layout (good for dashboards)
    property int panelPadding: spacingM
    property int sectionSpacing: spacingL
}

