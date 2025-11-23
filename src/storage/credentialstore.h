#ifndef CREDENTIALSTORE_H
#define CREDENTIALSTORE_H

#include <QObject>
#include <QSettings>
#include <QString>

class CredentialStore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool hasCredentials READ hasCredentials NOTIFY hasCredentialsChanged)
    Q_PROPERTY(QString accessToken READ accessToken NOTIFY accessTokenChanged)

public:
    explicit CredentialStore(QObject *parent = nullptr);
    ~CredentialStore();

    bool hasCredentials() const;
    QString accessToken() const { return m_accessToken; }
    QString refreshToken() const { return m_refreshToken; }

    Q_INVOKABLE void saveCredentials(const QString &accessToken, const QString &refreshToken);
    Q_INVOKABLE void clearCredentials();
    Q_INVOKABLE void loadCredentials();

signals:
    void hasCredentialsChanged();
    void accessTokenChanged();

private:
    void setAccessToken(const QString &token);

    QSettings *m_settings;
    QString m_accessToken;
    QString m_refreshToken;
};

#endif // CREDENTIALSTORE_H
