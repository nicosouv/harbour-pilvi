#ifndef FILEMODEL_H
#define FILEMODEL_H

#include <QAbstractListModel>
#include <QList>
#include "fileitem.h"

class FileModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum FileRoles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        MimeTypeRole,
        SizeRole,
        ModifiedTimeRole,
        IsFolderRole,
        StarredRole,
        IconUrlRole,
        ThumbnailUrlRole,
        WebViewLinkRole
    };

    explicit FileModel(QObject *parent = nullptr);
    ~FileModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const { return m_files.count(); }

    Q_INVOKABLE void clear();
    Q_INVOKABLE void addFile(const QString &id, const QString &name, const QString &mimeType,
                            qint64 size, const QString &modifiedTime, bool starred,
                            const QString &iconUrl, const QString &thumbnailUrl,
                            const QString &webViewLink);
    Q_INVOKABLE void removeFile(int index);
    Q_INVOKABLE FileItem* getFile(int index) const;
    Q_INVOKABLE int findFileById(const QString &id) const;

signals:
    void countChanged();

private:
    QList<FileItem*> m_files;
};

#endif // FILEMODEL_H
