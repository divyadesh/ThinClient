// global app state (signals, context properties)
import QtQuick 2.15

QtObject {
    signal alarmRaised(string code)
    signal userLoggedIn(string username)
    property bool isConnected: false
}
