import QtQuick 2.0
import Sailfish.Silica 1.0
import "pages"

ApplicationWindow {
    id: appWindow

    initialPage: Component {
        id: initialPageComponent

        Loader {
            sourceComponent: credentialStore.hasCredentials ? mainPageComponent : authPageComponent

            Component {
                id: mainPageComponent
                MainPage { }
            }

            Component {
                id: authPageComponent
                AuthPage { }
            }
        }
    }

    cover: Qt.resolvedUrl("cover/CoverPage.qml")

    allowedOrientations: defaultAllowedOrientations
}
