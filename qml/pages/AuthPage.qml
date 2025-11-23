import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.pilvi.googledrive 1.0

Page {
    id: page

    allowedOrientations: Orientation.All

    OAuthFlow {
        id: oauthFlow

        onAuthenticationSucceeded: {
            credentialStore.saveCredentials(accessToken, refreshToken)
            pageStack.replace(Qt.resolvedUrl("MainPage.qml"))
        }

        onAuthenticationFailed: {
            errorLabel.text = qsTr("Authentication failed: %1").arg(error)
        }
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column
            width: page.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("Sign in")
            }

            Image {
                anchors.horizontalCenter: parent.horizontalCenter
                width: Theme.itemSizeHuge
                height: Theme.itemSizeHuge
                source: "/usr/share/icons/hicolor/172x172/apps/harbour-pilvi.png"
            }

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 2 * Theme.horizontalPageMargin
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                text: qsTr("Sign in to Google Drive to access your files")
                color: Theme.highlightColor
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: oauthFlow.isAuthenticating ? qsTr("Authenticating...") : qsTr("Sign in with Google")
                enabled: !oauthFlow.isAuthenticating
                onClicked: {
                    oauthFlow.startAuthentication()
                }
            }

            BusyIndicator {
                anchors.horizontalCenter: parent.horizontalCenter
                size: BusyIndicatorSize.Large
                running: oauthFlow.isAuthenticating
                visible: running
            }

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 2 * Theme.horizontalPageMargin
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                text: qsTr("Your browser will open. Sign in with Google and authorize Pilvi.")
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeSmall
                visible: oauthFlow.isAuthenticating
            }

            Label {
                id: errorLabel
                width: parent.width - 2 * Theme.horizontalPageMargin
                anchors.horizontalCenter: parent.horizontalCenter
                wrapMode: Text.WordWrap
                color: Theme.errorColor
                visible: text !== ""
            }

            SectionHeader {
                text: qsTr("How it works")
                visible: !oauthFlow.isAuthenticating
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.WordWrap
                text: "1. " + qsTr("Tap the button above")
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeSmall
                visible: !oauthFlow.isAuthenticating
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.WordWrap
                text: "2. " + qsTr("Browser will open automatically")
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeSmall
                visible: !oauthFlow.isAuthenticating
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.WordWrap
                text: "3. " + qsTr("Sign in with your Google account")
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeSmall
                visible: !oauthFlow.isAuthenticating
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.WordWrap
                text: "4. " + qsTr("Return to Pilvi automatically")
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeSmall
                visible: !oauthFlow.isAuthenticating
            }
        }

        VerticalScrollDecorator {}
    }
}
