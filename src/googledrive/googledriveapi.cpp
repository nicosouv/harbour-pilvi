#include "googledriveapi.h"
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QHttpMultiPart>
#include <QUrlQuery>
#include <QMimeDatabase>
#include <QDebug>

const QString GoogleDriveApi::API_BASE_URL = "https://www.googleapis.com/drive/v3";
const QString GoogleDriveApi::UPLOAD_URL = "https://www.googleapis.com/upload/drive/v3/files";

GoogleDriveApi::GoogleDriveApi(CredentialStore *credStore, QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_credentialStore(credStore)
    , m_uploadProgress(0.0)
    , m_downloadProgress(0.0)
    , m_busy(false)
{
}

GoogleDriveApi::~GoogleDriveApi()
{
}

void GoogleDriveApi::listFiles(const QString &folderId, const QString &query)
{
    QUrlQuery urlQuery;
    QString q = QString("'%1' in parents and trashed=false").arg(folderId);
    if (!query.isEmpty()) {
        q += " and " + query;
    }
    urlQuery.addQueryItem("q", q);
    urlQuery.addQueryItem("fields", "files(id,name,mimeType,size,modifiedTime,starred,iconLink,thumbnailLink,webViewLink)");
    urlQuery.addQueryItem("pageSize", "100");
    urlQuery.addQueryItem("orderBy", "folder,name");

    QUrl url(API_BASE_URL + "/files");
    url.setQuery(urlQuery);

    makeRequest(url, ListFiles);
}

void GoogleDriveApi::getFileMetadata(const QString &fileId)
{
    QUrlQuery urlQuery;
    urlQuery.addQueryItem("fields", "*");

    QUrl url(API_BASE_URL + "/files/" + fileId);
    url.setQuery(urlQuery);

    makeRequest(url, GetMetadata);
}

void GoogleDriveApi::downloadFile(const QString &fileId, const QString &localPath)
{
    QUrl url(API_BASE_URL + "/files/" + fileId);
    QUrlQuery urlQuery;
    urlQuery.addQueryItem("alt", "media");
    url.setQuery(urlQuery);

    QNetworkRequest request(url);
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_credentialStore->accessToken()).toUtf8());

    QNetworkReply *reply = m_networkManager->get(request);
    m_pendingRequests[reply] = Download;
    m_downloadPaths[reply] = localPath;

    connect(reply, &QNetworkReply::finished, this, &GoogleDriveApi::handleNetworkReply);
    connect(reply, &QNetworkReply::downloadProgress, this, &GoogleDriveApi::handleDownloadProgress);

    setBusy(true);
}

void GoogleDriveApi::uploadFile(const QString &localPath, const QString &parentId)
{
    QFile file(localPath);
    if (!file.open(QIODevice::ReadOnly)) {
        setError("Cannot open file: " + localPath);
        return;
    }

    QFileInfo fileInfo(localPath);
    QMimeDatabase mimeDb;
    QString mimeType = mimeDb.mimeTypeForFile(fileInfo).name();

    // Metadata
    QJsonObject metadata;
    metadata["name"] = fileInfo.fileName();
    metadata["parents"] = QJsonArray() << parentId;

    QByteArray metadataBytes = QJsonDocument(metadata).toJson(QJsonDocument::Compact);

    // Multipart upload
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::RelatedType);

    QHttpPart metadataPart;
    metadataPart.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=UTF-8");
    metadataPart.setBody(metadataBytes);

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, mimeType);
    filePart.setBodyDevice(&file);
    file.setParent(multiPart);

    multiPart->append(metadataPart);
    multiPart->append(filePart);

    QUrl url(UPLOAD_URL);
    QUrlQuery urlQuery;
    urlQuery.addQueryItem("uploadType", "multipart");
    urlQuery.addQueryItem("fields", "id,name,mimeType,size,modifiedTime");
    url.setQuery(urlQuery);

    QNetworkRequest request(url);
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_credentialStore->accessToken()).toUtf8());

    QNetworkReply *reply = m_networkManager->post(request, multiPart);
    multiPart->setParent(reply);
    m_pendingRequests[reply] = Upload;

    connect(reply, &QNetworkReply::finished, this, &GoogleDriveApi::handleNetworkReply);
    connect(reply, &QNetworkReply::uploadProgress, this, &GoogleDriveApi::handleUploadProgress);

    setBusy(true);
}

void GoogleDriveApi::createFolder(const QString &name, const QString &parentId)
{
    QJsonObject metadata;
    metadata["name"] = name;
    metadata["mimeType"] = "application/vnd.google-apps.folder";
    metadata["parents"] = QJsonArray() << parentId;

    QByteArray data = QJsonDocument(metadata).toJson();
    QUrl url(API_BASE_URL + "/files");

    makeRequest(url, CreateFolder, data, "POST");
}

void GoogleDriveApi::deleteFile(const QString &fileId)
{
    QUrl url(API_BASE_URL + "/files/" + fileId);
    makeRequest(url, Delete, QByteArray(), "DELETE");
}

void GoogleDriveApi::renameFile(const QString &fileId, const QString &newName)
{
    QJsonObject metadata;
    metadata["name"] = newName;

    QByteArray data = QJsonDocument(metadata).toJson();
    QUrl url(API_BASE_URL + "/files/" + fileId);

    makeRequest(url, Rename, data, "PATCH");
}

void GoogleDriveApi::moveFile(const QString &fileId, const QString &newParentId)
{
    QUrlQuery urlQuery;
    urlQuery.addQueryItem("addParents", newParentId);
    urlQuery.addQueryItem("fields", "id,name,parents");

    QUrl url(API_BASE_URL + "/files/" + fileId);
    url.setQuery(urlQuery);

    makeRequest(url, Move, QByteArray(), "PATCH");
}

void GoogleDriveApi::copyFile(const QString &fileId, const QString &newParentId)
{
    QJsonObject metadata;
    if (!newParentId.isEmpty()) {
        metadata["parents"] = QJsonArray() << newParentId;
    }

    QByteArray data = QJsonDocument(metadata).toJson();
    QUrl url(API_BASE_URL + "/files/" + fileId + "/copy");

    makeRequest(url, Copy, data, "POST");
}

void GoogleDriveApi::starFile(const QString &fileId, bool starred)
{
    QJsonObject metadata;
    metadata["starred"] = starred;

    QByteArray data = QJsonDocument(metadata).toJson();
    QUrl url(API_BASE_URL + "/files/" + fileId);

    makeRequest(url, Star, data, "PATCH");
}

void GoogleDriveApi::shareFile(const QString &fileId, const QString &email, const QString &role)
{
    QJsonObject permission;
    permission["type"] = "user";
    permission["role"] = role; // reader, writer, commenter
    permission["emailAddress"] = email;

    QByteArray data = QJsonDocument(permission).toJson();
    QUrl url(API_BASE_URL + "/files/" + fileId + "/permissions");

    makeRequest(url, Share, data, "POST");
}

void GoogleDriveApi::searchFiles(const QString &query)
{
    QUrlQuery urlQuery;
    QString q = QString("name contains '%1' and trashed=false").arg(query);
    urlQuery.addQueryItem("q", q);
    urlQuery.addQueryItem("fields", "files(id,name,mimeType,size,modifiedTime,starred,iconLink,thumbnailLink)");
    urlQuery.addQueryItem("pageSize", "50");

    QUrl url(API_BASE_URL + "/files");
    url.setQuery(urlQuery);

    makeRequest(url, Search);
}

void GoogleDriveApi::getChanges(const QString &pageToken)
{
    QUrlQuery urlQuery;
    if (!pageToken.isEmpty()) {
        urlQuery.addQueryItem("pageToken", pageToken);
    } else {
        urlQuery.addQueryItem("pageToken", "1");
    }
    urlQuery.addQueryItem("fields", "newStartPageToken,changes(fileId,file(id,name,mimeType,trashed))");

    QUrl url(API_BASE_URL + "/changes");
    url.setQuery(urlQuery);

    makeRequest(url, Changes);
}

void GoogleDriveApi::getAbout()
{
    QUrlQuery urlQuery;
    urlQuery.addQueryItem("fields", "user,storageQuota");

    QUrl url(API_BASE_URL + "/about");
    url.setQuery(urlQuery);

    makeRequest(url, About);
}

void GoogleDriveApi::makeRequest(const QUrl &url, RequestType type, const QByteArray &data, const QString &method)
{
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_credentialStore->accessToken()).toUtf8());

    if (!data.isEmpty()) {
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    }

    QNetworkReply *reply = nullptr;
    if (method == "GET") {
        reply = m_networkManager->get(request);
    } else if (method == "POST") {
        reply = m_networkManager->post(request, data);
    } else if (method == "PATCH") {
        reply = m_networkManager->sendCustomRequest(request, "PATCH", data);
    } else if (method == "DELETE") {
        reply = m_networkManager->deleteResource(request);
    }

    if (reply) {
        m_pendingRequests[reply] = type;
        connect(reply, &QNetworkReply::finished, this, &GoogleDriveApi::handleNetworkReply);
        setBusy(true);
    }
}

void GoogleDriveApi::handleNetworkReply()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply)
        return;

    reply->deleteLater();

    RequestType type = m_pendingRequests.take(reply);
    setBusy(!m_pendingRequests.isEmpty());

    if (reply->error() != QNetworkReply::NoError) {
        setError(reply->errorString());
        return;
    }

    QByteArray responseData = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(responseData);

    switch (type) {
    case ListFiles: {
        QJsonArray files = doc.object()["files"].toArray();
        emit filesListed(files);
        break;
    }
    case GetMetadata:
        emit fileMetadataReceived(doc.object());
        break;
    case Download: {
        QString localPath = m_downloadPaths.take(reply);
        QFile file(localPath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(responseData);
            file.close();
            emit fileDownloaded(localPath);
        } else {
            setError("Failed to save file: " + localPath);
        }
        setDownloadProgress(0.0);
        break;
    }
    case Upload:
        emit fileUploaded(doc.object());
        setUploadProgress(0.0);
        break;
    case CreateFolder:
        emit folderCreated(doc.object());
        break;
    case Delete:
        emit fileDeleted(reply->url().path().split("/").last());
        break;
    case Rename:
    case Move:
        emit fileMoved(doc.object());
        break;
    case Copy:
        emit fileCopied(doc.object());
        break;
    case Star:
        // Handle star response
        break;
    case Share:
        emit fileShared(reply->url().path().split("/").first());
        break;
    case Search: {
        QJsonArray files = doc.object()["files"].toArray();
        emit searchCompleted(files);
        break;
    }
    case Changes: {
        QJsonArray changes = doc.object()["changes"].toArray();
        QString newPageToken = doc.object()["newStartPageToken"].toString();
        emit changesReceived(changes, newPageToken);
        break;
    }
    case About:
        emit aboutReceived(doc.object());
        break;
    }
}

void GoogleDriveApi::handleUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    if (bytesTotal > 0) {
        setUploadProgress(static_cast<qreal>(bytesSent) / bytesTotal);
    }
}

void GoogleDriveApi::handleDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesTotal > 0) {
        setDownloadProgress(static_cast<qreal>(bytesReceived) / bytesTotal);
    }
}

void GoogleDriveApi::setBusy(bool busy)
{
    if (m_busy != busy) {
        m_busy = busy;
        emit busyChanged();
    }
}

void GoogleDriveApi::setError(const QString &error)
{
    if (m_error != error) {
        m_error = error;
        emit errorChanged();
    }
}

void GoogleDriveApi::setUploadProgress(qreal progress)
{
    if (m_uploadProgress != progress) {
        m_uploadProgress = progress;
        emit uploadProgressChanged();
    }
}

void GoogleDriveApi::setDownloadProgress(qreal progress)
{
    if (m_downloadProgress != progress) {
        m_downloadProgress = progress;
        emit downloadProgressChanged();
    }
}
