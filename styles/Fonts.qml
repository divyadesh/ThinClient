pragma Singleton
import QtQuick 2.9
import "."   // to access ScreenConfig

QtObject {
    // Global font family
    property string family: "Roboto"

    // Type scale (based on Material Design / HMI practice)
    property int display: 34 * ScreenConfig.scale   // Very large (screen titles, dashboards)
    property int h1:      28 * ScreenConfig.scale   // Main section headers
    property int h2:      22 * ScreenConfig.scale   // Subsection headers
    property int h3:      20 * ScreenConfig.scale   // Smaller headers
    property int body:    16 * ScreenConfig.scale   // Main body text
    property int small:   14 * ScreenConfig.scale   // Secondary body, hints
    property int caption: 12 * ScreenConfig.scale   // Captions, helper text
    property int tiny:    10 * ScreenConfig.scale   // Very small labels (avoid overuse!)
}
