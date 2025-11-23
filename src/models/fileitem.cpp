#include "fileitem.h"

FileItem::FileItem(QObject *parent)
    : QObject(parent)
    , m_size(0)
    , m_starred(false)
{
}

FileItem::FileItem(const QString &id, const QString &name, const QString &mimeType,
                   qint64 size, const QDateTime &modifiedTime, bool starred,
                   const QString &iconUrl, const QString &thumbnailUrl,
                   const QString &webViewLink, QObject *parent)
    : QObject(parent)
    , m_id(id)
    , m_name(name)
    , m_mimeType(mimeType)
    , m_size(size)
    , m_modifiedTime(modifiedTime)
    , m_starred(starred)
    , m_iconUrl(iconUrl)
    , m_thumbnailUrl(thumbnailUrl)
    , m_webViewLink(webViewLink)
{
}

void FileItem::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged();
    }
}

void FileItem::setStarred(bool starred)
{
    if (m_starred != starred) {
        m_starred = starred;
        emit starredChanged();
    }
}
