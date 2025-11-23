import QtQuick 2.0
import Sailfish.Silica 1.0

Dialog {
    id: dialog

    property alias title: header.title
    property alias placeholderText: textField.placeholderText
    property alias value: textField.text
    property alias label: textField.label

    canAccept: textField.text.length > 0

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column
            width: parent.width

            DialogHeader {
                id: header
            }

            TextField {
                id: textField
                width: parent.width
                focus: true
                EnterKey.enabled: text.length > 0
                EnterKey.iconSource: "image://theme/icon-m-enter-accept"
                EnterKey.onClicked: dialog.accept()
            }
        }
    }
}
