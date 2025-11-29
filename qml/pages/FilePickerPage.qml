import QtQuick 2.0
import Sailfish.Silica 1.0
import Qt.labs.folderlistmodel 2.1

Page {
    id: page

    property string currentPath: StandardPaths.home
    property string parentFolderId: "root"

    signal fileSelected(string filePath)

    FolderListModel {
        id: folderModel
        folder: "file://" + currentPath
        showDirs: true
        showDotAndDotDot: false
        showHidden: false
    }

    SilicaListView {
        id: listView
        anchors.fill: parent
        model: folderModel

        header: PageHeader {
            title: currentPath === StandardPaths.home ? qsTr("Select File") : qsTr("...")
            description: currentPath
        }

        delegate: ListItem {
            id: listItem
            contentHeight: Theme.itemSizeMedium

            Image {
                id: icon
                x: Theme.horizontalPageMargin
                anchors.verticalCenter: parent.verticalCenter
                width: Theme.iconSizeMedium
                height: Theme.iconSizeMedium
                source: model.fileIsDir ? "image://theme/icon-m-folder" : "image://theme/icon-m-file-other"
            }

            Label {
                anchors {
                    left: icon.right
                    right: parent.right
                    leftMargin: Theme.paddingMedium
                    rightMargin: Theme.horizontalPageMargin
                    verticalCenter: parent.verticalCenter
                }
                text: model.fileName
                color: listItem.highlighted ? Theme.highlightColor : Theme.primaryColor
                truncationMode: TruncationMode.Fade
            }

            onClicked: {
                if (model.fileIsDir) {
                    currentPath = model.filePath
                } else {
                    // Upload file
                    driveApi.uploadFile(model.filePath, parentFolderId)
                    pageStack.pop()
                }
            }
        }

        PullDownMenu {
            MenuItem {
                text: qsTr("Parent folder")
                enabled: currentPath !== "/"
                onClicked: {
                    var parts = currentPath.split("/")
                    parts.pop()
                    currentPath = parts.join("/") || "/"
                }
            }
        }

        ViewPlaceholder {
            enabled: listView.count === 0
            text: qsTr("Empty folder")
        }

        VerticalScrollDecorator {}
    }
}

