import QtQuick 2.0
import Sailfish.Silica 1.0

Dialog {
    id: dialog

    property alias email: emailField.text
    property string role: "reader"

    canAccept: emailField.text.length > 0 && emailField.acceptableInput

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column
            width: parent.width
            spacing: Theme.paddingMedium

            DialogHeader {
                title: qsTr("Share file")
            }

            TextField {
                id: emailField
                width: parent.width
                label: qsTr("Email address")
                placeholderText: qsTr("user@example.com")
                inputMethodHints: Qt.ImhEmailCharactersOnly
                validator: RegExpValidator {
                    regExp: /^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$/
                }
                EnterKey.enabled: text.length > 0
                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked: roleComboBox.focus = true
            }

            ComboBox {
                id: roleComboBox
                width: parent.width
                label: qsTr("Permission")

                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("Can view")
                        onClicked: dialog.role = "reader"
                    }
                    MenuItem {
                        text: qsTr("Can comment")
                        onClicked: dialog.role = "commenter"
                    }
                    MenuItem {
                        text: qsTr("Can edit")
                        onClicked: dialog.role = "writer"
                    }
                }
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.WordWrap
                text: qsTr("The person will receive an email notification about the shared file.")
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeExtraSmall
            }
        }
    }
}
