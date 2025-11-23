#include "filemodel.h"
#include <QDateTime>

FileModel::FileModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

FileModel::~FileModel()
{
    qDeleteAll(m_files);
}

int FileModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_files.count();
}

QVariant FileModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_files.count())
        return QVariant();

    FileItem *file = m_files.at(index.row());

    switch (role) {
    case IdRole:
        return file->id();
    case NameRole:
        return file->name();
    case MimeTypeRole:
        return file->mimeType();
    case SizeRole:
        return file->size();
    case ModifiedTimeRole:
        return file->modifiedTime();
    case IsFolderRole:
        return file->isFolder();
    case StarredRole:
        return file->starred();
    case IconUrlRole:
        return file->iconUrl();
    case ThumbnailUrlRole:
        return file->thumbnailUrl();
    case WebViewLinkRole:
        return file->webViewLink();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> FileModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "fileId";
    roles[NameRole] = "fileName";
    roles[MimeTypeRole] = "mimeType";
    roles[SizeRole] = "fileSize";
    roles[ModifiedTimeRole] = "modifiedTime";
    roles[IsFolderRole] = "isFolder";
    roles[StarredRole] = "starred";
    roles[IconUrlRole] = "iconUrl";
    roles[ThumbnailUrlRole] = "thumbnailUrl";
    roles[WebViewLinkRole] = "webViewLink";
    return roles;
}

void FileModel::clear()
{
    beginResetModel();
    qDeleteAll(m_files);
    m_files.clear();
    endResetModel();
    emit countChanged();
}

void FileModel::addFile(const QString &id, const QString &name, const QString &mimeType,
                       qint64 size, const QString &modifiedTime, bool starred,
                       const QString &iconUrl, const QString &thumbnailUrl,
                       const QString &webViewLink)
{
    QDateTime dateTime = QDateTime::fromString(modifiedTime, Qt::ISODate);

    beginInsertRows(QModelIndex(), m_files.count(), m_files.count());
    FileItem *file = new FileItem(id, name, mimeType, size, dateTime, starred,
                                  iconUrl, thumbnailUrl, webViewLink, this);
    m_files.append(file);
    endInsertRows();
    emit countChanged();
}

void FileModel::removeFile(int index)
{
    if (index < 0 || index >= m_files.count())
        return;

    beginRemoveRows(QModelIndex(), index, index);
    delete m_files.takeAt(index);
    endRemoveRows();
    emit countChanged();
}

FileItem* FileModel::getFile(int index) const
{
    if (index < 0 || index >= m_files.count())
        return nullptr;

    return m_files.at(index);
}

int FileModel::findFileById(const QString &id) const
{
    for (int i = 0; i < m_files.count(); ++i) {
        if (m_files.at(i)->id() == id)
            return i;
    }
    return -1;
}
