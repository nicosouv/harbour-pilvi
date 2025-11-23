import QtQuick 2.0
import Sailfish.Silica 1.0
import Nemo.FileManager 1.0

Page {
    id: page

    signal fileSelected(string filePath)

    property string currentPath: StandardPaths.home

    SilicaListView {
        id: listView
        anchors.fill: parent

        header: PageHeader {
            title: qsTr("Select file")
            description: currentPath
        }

        model: FolderListModel {
            id: folderModel
            path: currentPath
            showDirectories: true
            showFiles: true
        }

        delegate: ListItem {
            id: listItem
            contentHeight: Theme.itemSizeSmall

            Image {
                id: icon
                x: Theme.horizontalPageMargin
                anchors.verticalCenter: parent.verticalCenter
                width: Theme.iconSizeSmall
                height: Theme.iconSizeSmall
                source: model.isDirectory ? "image://theme/icon-m-folder" : "image://theme/icon-m-file-other"
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
                if (model.isDirectory) {
                    currentPath = model.filePath
                } else {
                    fileSelected(model.filePath)
                    pageStack.pop()
                }
            }
        }

        VerticalScrollDecorator {}
    }
}
