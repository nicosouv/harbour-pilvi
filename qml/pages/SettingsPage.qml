import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page

    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column
            width: page.width

            PageHeader {
                title: qsTr("Settings")
            }

            SectionHeader {
                text: qsTr("Account")
            }

            TextSwitch {
                text: qsTr("Signed in")
                description: credentialStore.hasCredentials ? qsTr("Tap to sign out") : qsTr("Not signed in")
                checked: credentialStore.hasCredentials
                automaticCheck: false
                onClicked: {
                    if (credentialStore.hasCredentials) {
                        var remorse = Remorse.popupAction(page, qsTr("Signing out"), function() {
                            credentialStore.clearCredentials()
                            pageStack.replace(Qt.resolvedUrl("AuthPage.qml"))
                        })
                    }
                }
            }

            SectionHeader {
                text: qsTr("Storage")
            }

            DetailItem {
                label: qsTr("Cache size")
                value: "0 KB"
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Clear cache")
                onClicked: {
                    // TODO: Implement cache clearing
                }
            }

            SectionHeader {
                text: qsTr("About")
            }

            DetailItem {
                label: qsTr("Version")
                value: "0.1.0"
            }

            DetailItem {
                label: qsTr("Author")
                value: "Pilvi Team"
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.WordWrap
                text: qsTr("Multi-cloud storage client for Sailfish OS")
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeSmall
            }
        }

        VerticalScrollDecorator {}
    }
}
