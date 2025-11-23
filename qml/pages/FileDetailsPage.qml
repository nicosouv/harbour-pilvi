import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page

    property string fileId
    property string fileName
    property int fileSize
    property string mimeType
    property var modifiedTime
    property bool starred
    property string webViewLink

    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        PullDownMenu {
            MenuItem {
                text: qsTr("Open in browser")
                visible: webViewLink !== ""
                onClicked: Qt.openUrlExternally(webViewLink)
            }
            MenuItem {
                text: qsTr("Share")
                onClicked: {
                    var dialog = pageStack.push("../dialogs/ShareDialog.qml")
                    dialog.accepted.connect(function() {
                        driveApi.shareFile(fileId, dialog.email, dialog.role)
                    })
                }
            }
            MenuItem {
                text: qsTr("Download")
                onClicked: {
                    var downloadPath = StandardPaths.download + "/" + fileName
                    driveApi.downloadFile(fileId, downloadPath)
                }
            }
        }

        Column {
            id: column
            width: page.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: fileName
            }

            Image {
                id: thumbnail
                width: parent.width
                height: width * 0.75
                fillMode: Image.PreserveAspectFit
                source: mimeType.startsWith("image/") ? thumbnailUrl : ""
                visible: source !== ""
            }

            DetailItem {
                label: qsTr("Size")
                value: Format.formatFileSize(fileSize)
            }

            DetailItem {
                label: qsTr("Type")
                value: mimeType
            }

            DetailItem {
                label: qsTr("Modified")
                value: Format.formatDate(modifiedTime, Formatter.DateMedium)
            }

            DetailItem {
                label: qsTr("Starred")
                value: starred ? qsTr("Yes") : qsTr("No")
            }

            SectionHeader {
                text: qsTr("Actions")
            }

            ButtonLayout {
                Button {
                    text: starred ? qsTr("Unstar") : qsTr("Star")
                    onClicked: {
                        driveApi.starFile(fileId, !starred)
                        starred = !starred
                    }
                }

                Button {
                    text: qsTr("Download")
                    onClicked: {
                        var downloadPath = StandardPaths.download + "/" + fileName
                        driveApi.downloadFile(fileId, downloadPath)
                        pageStack.pop()
                    }
                }
            }

            ButtonLayout {
                Button {
                    text: qsTr("Rename")
                    onClicked: {
                        var dialog = pageStack.push("../dialogs/InputDialog.qml", {
                            title: qsTr("Rename"),
                            placeholderText: qsTr("New name"),
                            value: fileName
                        })
                        dialog.accepted.connect(function() {
                            driveApi.renameFile(fileId, dialog.value)
                            fileName = dialog.value
                        })
                    }
                }

                Button {
                    text: qsTr("Delete")
                    onClicked: {
                        var remorse = Remorse.popupAction(page, qsTr("Deleting"), function() {
                            driveApi.deleteFile(fileId)
                            pageStack.pop()
                        })
                    }
                }
            }

            ProgressBar {
                width: parent.width
                minimumValue: 0
                maximumValue: 1
                value: driveApi.downloadProgress
                visible: driveApi.downloadProgress > 0 && driveApi.downloadProgress < 1
                label: qsTr("Downloading")
            }
        }

        VerticalScrollDecorator {}
    }
}
