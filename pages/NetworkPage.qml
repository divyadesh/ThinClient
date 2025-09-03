import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import App.Styles 1.0

import "../controls"
import "../components"

BasicPage {
    id: page
    padding: 20
    property bool addNewServer: false

    header: PageHeader {
        pageTitle: page.pageTitle
        onBackPressed: {
            if(pageStack.depth == 1) {
                backToHome()
                return
            }
            pageStack.pop()
        }
    }

    contentItem: Item {}
}
