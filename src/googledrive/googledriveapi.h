#ifndef GOOGLEDRIVEAPI_H
#define GOOGLEDRIVEAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonObject>
#include "../storage/credentialstore.h"

class FileModel;

class GoogleDriveApi : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_PROPERTY(qreal uploadProgress READ uploadProgress NOTIFY uploadProgressChanged)
    Q_PROPERTY(qreal downloadProgress READ downloadProgress NOTIFY downloadProgressChanged)

public:
    explicit GoogleDriveApi(CredentialStore *credStore, QObject *parent = nullptr);
    ~GoogleDriveApi();

    bool busy() const { return m_busy; }
    QString error() const { return m_error; }
    qreal uploadProgress() const { return m_uploadProgress; }
    qreal downloadProgress() const { return m_downloadProgress; }

    // File operations
    Q_INVOKABLE void listFiles(const QString &folderId = "root", const QString &query = "");
    Q_INVOKABLE void getFileMetadata(const QString &fileId);
    Q_INVOKABLE void downloadFile(const QString &fileId, const QString &localPath);
    Q_INVOKABLE void uploadFile(const QString &localPath, const QString &parentId = "root");
    Q_INVOKABLE void createFolder(const QString &name, const QString &parentId = "root");
    Q_INVOKABLE void deleteFile(const QString &fileId);
    Q_INVOKABLE void renameFile(const QString &fileId, const QString &newName);
    Q_INVOKABLE void moveFile(const QString &fileId, const QString &newParentId);
    Q_INVOKABLE void copyFile(const QString &fileId, const QString &newParentId = "");
    Q_INVOKABLE void starFile(const QString &fileId, bool starred);
    Q_INVOKABLE void shareFile(const QString &fileId, const QString &email, const QString &role);
    Q_INVOKABLE void searchFiles(const QString &query);
    Q_INVOKABLE void getChanges(const QString &pageToken = "");
    Q_INVOKABLE void getAbout();

signals:
    void busyChanged();
    void errorChanged();
    void uploadProgressChanged();
    void downloadProgressChanged();

    void filesListed(const QJsonArray &files);
    void fileMetadataReceived(const QJsonObject &metadata);
    void fileDownloaded(const QString &localPath);
    void fileUploaded(const QJsonObject &metadata);
    void folderCreated(const QJsonObject &metadata);
    void fileDeleted(const QString &fileId);
    void fileRenamed(const QJsonObject &metadata);
    void fileMoved(const QJsonObject &metadata);
    void fileCopied(const QJsonObject &metadata);
    void fileStarred(const QString &fileId, bool starred);
    void fileShared(const QString &fileId);
    void searchCompleted(const QJsonArray &results);
    void changesReceived(const QJsonArray &changes, const QString &newPageToken);
    void aboutReceived(const QJsonObject &about);

private slots:
    void handleNetworkReply();
    void handleUploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void handleDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    enum RequestType {
        ListFiles,
        GetMetadata,
        Download,
        Upload,
        CreateFolder,
        Delete,
        Rename,
        Move,
        Copy,
        Star,
        Share,
        Search,
        Changes,
        About
    };

    void makeRequest(const QUrl &url, RequestType type, const QByteArray &data = QByteArray(), const QString &method = "GET");
    void setBusy(bool busy);
    void setError(const QString &error);
    void setUploadProgress(qreal progress);
    void setDownloadProgress(qreal progress);
    QString buildQuery(const QString &query);

    QNetworkAccessManager *m_networkManager;
    CredentialStore *m_credentialStore;
    QString m_error;
    qreal m_uploadProgress;
    qreal m_downloadProgress;
    bool m_busy;
    QHash<QNetworkReply*, RequestType> m_pendingRequests;
    QHash<QNetworkReply*, QString> m_downloadPaths;

    static const QString API_BASE_URL;
    static const QString UPLOAD_URL;
};

#endif // GOOGLEDRIVEAPI_H
