#ifndef FILECACHE_H
#define FILECACHE_H

#include <QObject>

class FileCache : public QObject
{
    Q_OBJECT
public:
    explicit FileCache(QObject *parent = nullptr);
};

#endif // FILECACHE_H
