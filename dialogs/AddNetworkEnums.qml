pragma Singleton
import QtQuick 2.15

QtObject {

    // ============================
    // SECURITY MODES
    // ============================
    readonly property int securityOpen: 0
    readonly property int securityWEP: 1
    readonly property int securityWPA: 2
    readonly property int securityWPA2: 3
    readonly property int securityWPAMixed: 4
    readonly property int securityWPA3: 5
    readonly property int securityWPA2WPA3: 6
    readonly property int securityEAP: 7
    readonly property int securityEAP3: 8

    // ============================
    // PROXY SETTINGS
    // ============================
    readonly property int proxyNone: 0
    readonly property int proxyManual: 1
    readonly property int proxyAuto: 2

    // ============================
    // IP SETTINGS
    // ============================
    readonly property int ipDHCP: 0
    readonly property int ipStatic: 1

    // ============================
    // MAC MODE
    // ============================
    readonly property int macDevice: 0
    readonly property int macRandom: 1

    // ============================
    // METERED MODE
    // ============================
    readonly property int meteredAuto: 0
    readonly property int meteredYes: 1
    readonly property int meteredNo: 2

    // ============================
    // HIDDEN NETWORK
    // ============================
    readonly property int hiddenNo: 0
    readonly property int hiddenYes: 1
}
