pragma Singleton
import QtQuick 2.15

QtObject {
    // Supported HMI resolutions
    // A dictionary (JavaScript object) that stores
    // common HMI screen resolutions by name.
    // You only need to refer to them by key (e.g. "WVGA", "FHD")
    // instead of writing width/height everywhere.
    property var resolutions: ({

        // QVGA = Quarter VGA (small screens, older HMIs)
        "QVGA": { width: 320, height: 240 },

        // WQVGA = Wide QVGA (wider than QVGA, small HMIs ~4.3")
        "WQVGA": { width: 480, height: 272 },

        // WVGA = Wide VGA (most common for mid-size HMIs ~7")
        "WVGA": { width: 800, height: 480 },

        // WSVGA = Wide Super VGA (~9–10" HMIs)
        "WSVGA": { width: 1024, height: 600 },

        // XGA = Extended Graphics Array (~12–15" HMIs, square aspect)
        "XGA": { width: 1024, height: 768 },

        // WXGA = Wide XGA (larger widescreen HMIs ~15.6")
        "WXGA": { width: 1280, height: 800 },

        // FHD = Full HD (modern high-end HMIs, 1080p widescreen)
        "FHD": { width: 1920, height: 1080 }
    })

    // Single property to switch resolution
    property string currentResolution: "WXGA"   // default

    // Derived width/height
    property int screenWidth: resolutions[currentResolution].width
    property int screenHeight: resolutions[currentResolution].height

    // Scale factors (base reference = WVGA 800x480)
    property real scaleX: screenWidth / 800.0
    property real scaleY: screenHeight / 480.0
    property real scale: Math.min(scaleX, scaleY)
}
