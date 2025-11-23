#include "credentialstore.h"
#include <QDebug>

CredentialStore::CredentialStore(QObject *parent)
    : QObject(parent)
    , m_settings(new QSettings(this))
{
    loadCredentials();
}

CredentialStore::~CredentialStore()
{
}

bool CredentialStore::hasCredentials() const
{
    return !m_accessToken.isEmpty() && !m_refreshToken.isEmpty();
}

void CredentialStore::saveCredentials(const QString &accessToken, const QString &refreshToken)
{
    m_settings->setValue("oauth/accessToken", accessToken);
    m_settings->setValue("oauth/refreshToken", refreshToken);
    m_settings->sync();

    setAccessToken(accessToken);
    m_refreshToken = refreshToken;

    emit hasCredentialsChanged();
}

void CredentialStore::clearCredentials()
{
    m_settings->remove("oauth/accessToken");
    m_settings->remove("oauth/refreshToken");
    m_settings->sync();

    setAccessToken(QString());
    m_refreshToken.clear();

    emit hasCredentialsChanged();
}

void CredentialStore::loadCredentials()
{
    QString accessToken = m_settings->value("oauth/accessToken").toString();
    QString refreshToken = m_settings->value("oauth/refreshToken").toString();

    setAccessToken(accessToken);
    m_refreshToken = refreshToken;

    if (hasCredentials()) {
        emit hasCredentialsChanged();
    }
}

void CredentialStore::setAccessToken(const QString &token)
{
    if (m_accessToken != token) {
        m_accessToken = token;
        emit accessTokenChanged();
    }
}
