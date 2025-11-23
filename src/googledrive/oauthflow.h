#ifndef OAUTHFLOW_H
#define OAUTHFLOW_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTcpServer>
#include <QString>
#include <QUrl>

class OAuthFlow : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isAuthenticating READ isAuthenticating NOTIFY isAuthenticatingChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)

public:
    explicit OAuthFlow(QObject *parent = nullptr);
    ~OAuthFlow();

    bool isAuthenticating() const { return m_isAuthenticating; }
    QString error() const { return m_error; }

    Q_INVOKABLE void startAuthentication();
    Q_INVOKABLE void cancelAuthentication();
    Q_INVOKABLE void refreshAccessToken(const QString &refreshToken);

signals:
    void isAuthenticatingChanged();
    void errorChanged();
    void authenticationSucceeded(const QString &accessToken, const QString &refreshToken);
    void authenticationFailed(const QString &error);

private slots:
    void handleIncomingConnection();
    void handleTokenResponse();

private:
    void setError(const QString &error);
    void setIsAuthenticating(bool authenticating);
    void openAuthorizationUrl();
    bool startLocalServer();
    void stopLocalServer();
    QString generateRandomState();
    QString generateCodeVerifier();
    QString generateCodeChallenge(const QString &verifier);
    void sendResponseToClient(QTcpSocket *socket, const QString &message);
    void exchangeCodeForToken(const QString &code);

    QNetworkAccessManager *m_networkManager;
    QTcpServer *m_localServer;
    QString m_error;
    QString m_state;
    QString m_codeVerifier;
    bool m_isAuthenticating;
    quint16 m_localPort;

    static const QString CLIENT_ID;
    static const QString CLIENT_SECRET;
    static const QString REDIRECT_URI;
    static const QString AUTHORIZATION_URL;
    static const QString TOKEN_URL;
    static const QString SCOPE;
};

#endif // OAUTHFLOW_H
