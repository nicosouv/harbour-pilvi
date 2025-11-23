import QtQuick 2.0
import Sailfish.Silica 1.0

CoverBackground {
    id: cover

    Image {
        anchors.centerIn: parent
        width: parent.width * 0.6
        height: width
        source: "image://theme/icon-l-cloud"
        opacity: 0.3
    }

    Label {
        id: titleLabel
        anchors {
            top: parent.top
            topMargin: Theme.paddingLarge
            horizontalCenter: parent.horizontalCenter
        }
        text: "Pilvi"
        font.pixelSize: Theme.fontSizeLarge
        color: Theme.primaryColor
    }

    Label {
        id: statusLabel
        anchors {
            bottom: progressBar.visible ? progressBar.top : parent.bottom
            bottomMargin: Theme.paddingLarge
            horizontalCenter: parent.horizontalCenter
        }
        text: driveApi.busy ? qsTr("Syncing...") : qsTr("Ready")
        font.pixelSize: Theme.fontSizeSmall
        color: Theme.secondaryColor
    }

    ProgressBar {
        id: progressBar
        anchors {
            bottom: parent.bottom
            bottomMargin: Theme.paddingLarge
        }
        width: parent.width - 2 * Theme.paddingLarge
        x: Theme.paddingLarge
        minimumValue: 0
        maximumValue: 1
        value: Math.max(driveApi.uploadProgress, driveApi.downloadProgress)
        visible: driveApi.uploadProgress > 0 || driveApi.downloadProgress > 0
    }

    CoverActionList {
        id: coverAction

        CoverAction {
            iconSource: "image://theme/icon-cover-refresh"
            onTriggered: {
                driveApi.listFiles("root", "")
            }
        }

        CoverAction {
            iconSource: "image://theme/icon-cover-search"
            onTriggered: {
                // Activate app and open search page
                appWindow.activate()
            }
        }
    }
}
