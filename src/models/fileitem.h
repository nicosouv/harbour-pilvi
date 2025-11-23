#ifndef FILEITEM_H
#define FILEITEM_H

#include <QObject>
#include <QString>
#include <QDateTime>

class FileItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString mimeType READ mimeType CONSTANT)
    Q_PROPERTY(qint64 size READ size CONSTANT)
    Q_PROPERTY(QDateTime modifiedTime READ modifiedTime CONSTANT)
    Q_PROPERTY(bool isFolder READ isFolder CONSTANT)
    Q_PROPERTY(bool starred READ starred NOTIFY starredChanged)
    Q_PROPERTY(QString iconUrl READ iconUrl CONSTANT)
    Q_PROPERTY(QString thumbnailUrl READ thumbnailUrl CONSTANT)
    Q_PROPERTY(QString webViewLink READ webViewLink CONSTANT)

public:
    explicit FileItem(QObject *parent = nullptr);
    FileItem(const QString &id, const QString &name, const QString &mimeType,
             qint64 size, const QDateTime &modifiedTime, bool starred,
             const QString &iconUrl, const QString &thumbnailUrl,
             const QString &webViewLink, QObject *parent = nullptr);

    QString id() const { return m_id; }
    QString name() const { return m_name; }
    QString mimeType() const { return m_mimeType; }
    qint64 size() const { return m_size; }
    QDateTime modifiedTime() const { return m_modifiedTime; }
    bool isFolder() const { return m_mimeType == "application/vnd.google-apps.folder"; }
    bool starred() const { return m_starred; }
    QString iconUrl() const { return m_iconUrl; }
    QString thumbnailUrl() const { return m_thumbnailUrl; }
    QString webViewLink() const { return m_webViewLink; }

    void setName(const QString &name);
    void setStarred(bool starred);

signals:
    void nameChanged();
    void starredChanged();

private:
    QString m_id;
    QString m_name;
    QString m_mimeType;
    qint64 m_size;
    QDateTime m_modifiedTime;
    bool m_starred;
    QString m_iconUrl;
    QString m_thumbnailUrl;
    QString m_webViewLink;
};

#endif // FILEITEM_H
