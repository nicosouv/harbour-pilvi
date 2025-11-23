#ifndef NETWORKREQUEST_H
#define NETWORKREQUEST_H

#include <QObject>

class NetworkRequest : public QObject
{
    Q_OBJECT
public:
    explicit NetworkRequest(QObject *parent = nullptr);
};

#endif // NETWORKREQUEST_H
