import QtQuick 2.0
import Sailfish.Silica 1.0
import "pages"

ApplicationWindow {
    id: appWindow

    Component.onCompleted: {
        if (credentialStore.hasCredentials) {
            pageStack.replace(Qt.resolvedUrl("pages/MainPage.qml"))
        } else {
            pageStack.replace(Qt.resolvedUrl("pages/AuthPage.qml"))
        }
    }

    cover: Qt.resolvedUrl("cover/CoverPage.qml")

    allowedOrientations: defaultAllowedOrientations
}
