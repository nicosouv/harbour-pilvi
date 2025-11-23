import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.pilvi.models 1.0

Page {
    id: page

    allowedOrientations: Orientation.All

    FileModel {
        id: searchResultsModel
    }

    Connections {
        target: driveApi
        onSearchCompleted: {
            searchResultsModel.clear()
            for (var i = 0; i < results.length; i++) {
                var file = results[i]
                searchResultsModel.addFile(
                    file.id,
                    file.name,
                    file.mimeType,
                    file.size || 0,
                    file.modifiedTime || "",
                    file.starred || false,
                    file.iconLink || "",
                    file.thumbnailLink || "",
                    file.webViewLink || ""
                )
            }
        }
    }

    SilicaListView {
        id: listView
        anchors.fill: parent
        model: searchResultsModel

        header: Column {
            width: parent.width

            PageHeader {
                title: qsTr("Search")
            }

            SearchField {
                id: searchField
                width: parent.width
                placeholderText: qsTr("Search files")
                EnterKey.iconSource: "image://theme/icon-m-enter-accept"
                EnterKey.onClicked: {
                    if (text.length > 0) {
                        driveApi.searchFiles(text)
                        focus = false
                    }
                }

                onTextChanged: {
                    if (text.length === 0) {
                        searchResultsModel.clear()
                    }
                }
            }
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
                source: isFolder ? "image://theme/icon-m-folder" : "image://theme/icon-m-file-other"
            }

            Column {
                anchors {
                    left: icon.right
                    right: parent.right
                    leftMargin: Theme.paddingMedium
                    rightMargin: Theme.horizontalPageMargin
                    verticalCenter: parent.verticalCenter
                }

                Label {
                    width: parent.width
                    text: fileName
                    color: listItem.highlighted ? Theme.highlightColor : Theme.primaryColor
                    truncationMode: TruncationMode.Fade
                }

                Label {
                    width: parent.width
                    text: isFolder ? qsTr("Folder") : Format.formatFileSize(fileSize)
                    color: listItem.highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                    font.pixelSize: Theme.fontSizeExtraSmall
                }
            }

            onClicked: {
                if (isFolder) {
                    pageStack.push(Qt.resolvedUrl("FileBrowserPage.qml"), {
                        folderId: fileId,
                        folderName: fileName
                    })
                } else {
                    pageStack.push(Qt.resolvedUrl("FileDetailsPage.qml"), {
                        fileId: fileId,
                        fileName: fileName,
                        fileSize: fileSize,
                        mimeType: mimeType,
                        modifiedTime: modifiedTime,
                        starred: starred,
                        webViewLink: webViewLink
                    })
                }
            }
        }

        ViewPlaceholder {
            enabled: listView.count === 0 && searchField.text.length > 0 && !driveApi.busy
            text: qsTr("No results")
            hintText: qsTr("Try a different search term")
        }

        ViewPlaceholder {
            enabled: listView.count === 0 && searchField.text.length === 0
            text: qsTr("Search your files")
            hintText: qsTr("Enter a search term above")
        }

        BusyIndicator {
            size: BusyIndicatorSize.Large
            anchors.centerIn: parent
            running: driveApi.busy
        }

        VerticalScrollDecorator {}
    }
}
