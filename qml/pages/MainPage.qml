import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.pilvi.models 1.0

Page {
    id: page

    FileModel {
        id: fileModel
    }

    Component.onCompleted: {
        loadFiles()
    }

    function loadFiles() {
        driveApi.listFiles("root", "")
    }

    Connections {
        target: driveApi
        onFilesListed: {
            fileModel.clear()
            for (var i = 0; i < files.length; i++) {
                var file = files[i]
                fileModel.addFile(
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
        model: fileModel

        PullDownMenu {
            MenuItem {
                text: qsTr("Settings")
                onClicked: pageStack.push(Qt.resolvedUrl("SettingsPage.qml"))
            }
            MenuItem {
                text: qsTr("Refresh")
                onClicked: loadFiles()
            }
            MenuItem {
                text: qsTr("New folder")
                onClicked: {
                    var dialog = pageStack.push(Qt.resolvedUrl("../dialogs/InputDialog.qml"), {
                        title: qsTr("Create folder"),
                        placeholderText: qsTr("Folder name")
                    })
                    dialog.accepted.connect(function() {
                        driveApi.createFolder(dialog.value, "root")
                    })
                }
            }
            // TODO: Upload file (requires ContentHub API which is approved for Harbour)
            // MenuItem {
            //     text: qsTr("Upload file")
            //     onClicked: {
            //         // Will be implemented with ContentHub
            //     }
            // }
        }

        PushUpMenu {
            MenuItem {
                text: qsTr("Search")
                onClicked: {
                    var panel = pageStack.push(Qt.resolvedUrl("SearchPage.qml"))
                }
            }
        }

        header: PageHeader {
            title: qsTr("Pilvi")
            description: qsTr("Google Drive")
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
                    right: starIcon.left
                    leftMargin: Theme.paddingMedium
                    rightMargin: Theme.paddingMedium
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

            IconButton {
                id: starIcon
                anchors {
                    right: parent.right
                    rightMargin: Theme.horizontalPageMargin
                    verticalCenter: parent.verticalCenter
                }
                icon.source: starred ? "image://theme/icon-m-favorite-selected" : "image://theme/icon-m-favorite"
                onClicked: {
                    driveApi.starFile(fileId, !starred)
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

            menu: ContextMenu {
                MenuItem {
                    text: qsTr("Download")
                    visible: !isFolder
                    onClicked: {
                        var downloadPath = StandardPaths.download + "/" + fileName
                        driveApi.downloadFile(fileId, downloadPath)
                        remorse.execute(listItem, qsTr("Downloading"), function() {})
                    }
                }
                MenuItem {
                    text: qsTr("Rename")
                    onClicked: {
                        var dialog = pageStack.push(Qt.resolvedUrl("../dialogs/InputDialog.qml"), {
                            title: qsTr("Rename"),
                            placeholderText: qsTr("New name"),
                            value: fileName
                        })
                        dialog.accepted.connect(function() {
                            driveApi.renameFile(fileId, dialog.value)
                        })
                    }
                }
                MenuItem {
                    text: qsTr("Delete")
                    onClicked: {
                        remorse.execute(listItem, qsTr("Deleting"), function() {
                            driveApi.deleteFile(fileId)
                        })
                    }
                }
            }

            RemorseItem { id: remorse }
        }

        ViewPlaceholder {
            enabled: listView.count === 0 && !driveApi.busy
            text: qsTr("No files")
            hintText: qsTr("Pull down to upload files or create folders")
        }

        BusyIndicator {
            size: BusyIndicatorSize.Large
            anchors.centerIn: parent
            running: driveApi.busy
        }

        VerticalScrollDecorator {}
    }
}
