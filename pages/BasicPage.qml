import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import App.Styles 1.0

Page {
    id: page
    property string pageTitle: ""
    property var pageStack: StackView.view

    signal backToHome()

    background: null
}
