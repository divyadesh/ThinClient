import QtQuick 2.15
import QtQuick.Controls 2.0
import App.Styles 1.0

Label {
    color: Colors.textPrimary
    font.pixelSize: 14
    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    maximumLineCount: 2
    elide: Text.ElideRight
}
