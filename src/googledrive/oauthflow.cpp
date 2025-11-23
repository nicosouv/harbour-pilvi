#include "oauthflow.h"
#include <QDesktopServices>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>
#include <QDateTime>
#include <QCryptographicHash>
#include <QDebug>

// Google OAuth 2.0 configuration
#ifndef PILVI_CLIENT_ID
#define PILVI_CLIENT_ID ""
#warning "PILVI_CLIENT_ID macro is not defined at compile time"
#endif

#ifndef PILVI_CLIENT_SECRET
#define PILVI_CLIENT_SECRET ""
#warning "PILVI_CLIENT_SECRET macro is not defined at compile time"
#endif

// Stringify macro value for debugging
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

const QString OAuthFlow::CLIENT_ID = QString(PILVI_CLIENT_ID);
const QString OAuthFlow::CLIENT_SECRET = QString(PILVI_CLIENT_SECRET);
const QString OAuthFlow::REDIRECT_URI = "http://localhost";
const QString OAuthFlow::AUTHORIZATION_URL = "https://accounts.google.com/o/oauth2/v2/auth";
const QString OAuthFlow::TOKEN_URL = "https://oauth2.googleapis.com/token";
const QString OAuthFlow::SCOPE = "https://www.googleapis.com/auth/drive";

OAuthFlow::OAuthFlow(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_localServer(new QTcpServer(this))
    , m_isAuthenticating(false)
    , m_localPort(8080)
{
    connect(m_localServer, &QTcpServer::newConnection,
            this, &OAuthFlow::handleIncomingConnection);

    // Debug: Log CLIENT_ID configuration status
    if (CLIENT_ID.isEmpty()) {
        qWarning() << "OAuth client ID is not configured!";
    } else {
        qDebug() << "OAuth configured successfully";
    }
}

OAuthFlow::~OAuthFlow()
{
    stopLocalServer();
}

void OAuthFlow::setIsAuthenticating(bool authenticating)
{
    if (m_isAuthenticating != authenticating) {
        m_isAuthenticating = authenticating;
        emit isAuthenticatingChanged();
    }
}

void OAuthFlow::setError(const QString &error)
{
    if (m_error != error) {
        m_error = error;
        emit errorChanged();
    }
}

QString OAuthFlow::generateRandomState()
{
    QString state;
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    const int length = 32;
    const int charsetSize = static_cast<int>(sizeof(charset) - 1);

    static bool seeded = false;
    if (!seeded) {
        qsrand(static_cast<uint>(QDateTime::currentMSecsSinceEpoch()));
        seeded = true;
    }

    for (int i = 0; i < length; i++) {
        int index = qrand() % charsetSize;
        state.append(charset[index]);
    }

    return state;
}

QString OAuthFlow::generateCodeVerifier()
{
    // Generate a random code verifier (43-128 chars)
    QString verifier;
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
    const int length = 128;
    const int charsetSize = static_cast<int>(sizeof(charset) - 1);

    for (int i = 0; i < length; i++) {
        int index = qrand() % charsetSize;
        verifier.append(charset[index]);
    }

    return verifier;
}

QString OAuthFlow::generateCodeChallenge(const QString &verifier)
{
    // SHA256 hash of the verifier, then base64url encode
    QByteArray hash = QCryptographicHash::hash(verifier.toUtf8(), QCryptographicHash::Sha256);
    QString base64 = hash.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
    return base64;
}

void OAuthFlow::startAuthentication()
{
    if (m_isAuthenticating) {
        qDebug() << "Authentication already in progress";
        return;
    }

    if (CLIENT_ID.isEmpty()) {
        qWarning() << "OAuth client ID not configured";
        setError("OAuth not configured");
        emit authenticationFailed("OAuth not configured");
        return;
    }

    qDebug() << "Starting OAuth 2.0 + PKCE authentication flow...";

    setIsAuthenticating(true);

    // Generate random state for CSRF protection
    m_state = generateRandomState();

    // Generate PKCE code verifier and challenge
    m_codeVerifier = generateCodeVerifier();
    QString codeChallenge = generateCodeChallenge(m_codeVerifier);

    // Start local HTTP server to receive callback
    if (!startLocalServer()) {
        setError("Failed to start local callback server");
        emit authenticationFailed("Failed to start local callback server");
        setIsAuthenticating(false);
        return;
    }

    // Open browser with authorization URL
    openAuthorizationUrl();
}

void OAuthFlow::cancelAuthentication()
{
    qDebug() << "Canceling authentication";
    stopLocalServer();
    setIsAuthenticating(false);
}

void OAuthFlow::openAuthorizationUrl()
{
    QUrl url(AUTHORIZATION_URL);
    QUrlQuery query;

    // Build redirect URI with actual port
    QString redirectUri = QString("%1:%2").arg(REDIRECT_URI).arg(m_localPort);

    query.addQueryItem("client_id", CLIENT_ID);
    query.addQueryItem("redirect_uri", redirectUri);
    query.addQueryItem("response_type", "code");
    query.addQueryItem("scope", SCOPE);
    query.addQueryItem("state", m_state);
    query.addQueryItem("access_type", "offline");
    query.addQueryItem("prompt", "consent");

    // PKCE parameters
    query.addQueryItem("code_challenge", generateCodeChallenge(m_codeVerifier));
    query.addQueryItem("code_challenge_method", "S256");

    url.setQuery(query);

    qDebug() << "Opening browser for Google authentication...";

    if (!QDesktopServices::openUrl(url)) {
        qWarning() << "Failed to open browser";
        setError("Failed to open browser");
        emit authenticationFailed("Failed to open browser");
        setIsAuthenticating(false);
    }
}

bool OAuthFlow::startLocalServer()
{
    // Try to bind to port 8080, if busy try 8081-8090
    for (quint16 port = 8080; port < 8090; ++port) {
        if (m_localServer->listen(QHostAddress::LocalHost, port)) {
            m_localPort = port;
            qDebug() << "Local callback server started on port:" << port;
            return true;
        }
    }

    qWarning() << "Failed to start local server on any port";
    return false;
}

void OAuthFlow::stopLocalServer()
{
    if (m_localServer->isListening()) {
        m_localServer->close();
        qDebug() << "Local callback server stopped";
    }
}

void OAuthFlow::handleIncomingConnection()
{
    qDebug() << "Received OAuth callback connection";

    QTcpSocket *socket = m_localServer->nextPendingConnection();

    if (!socket) {
        return;
    }

    connect(socket, &QTcpSocket::readyRead, this, [this, socket]() {
        QString request = socket->readAll();
        qDebug() << "OAuth callback request received";

        // Parse HTTP request
        QStringList lines = request.split("\r\n");
        if (lines.isEmpty()) {
            socket->deleteLater();
            return;
        }

        // Extract URL from first line (GET /callback?code=xxx&state=yyy HTTP/1.1)
        QStringList requestLine = lines[0].split(" ");
        if (requestLine.size() < 2) {
            sendResponseToClient(socket, "Invalid request");
            socket->deleteLater();
            return;
        }

        QString path = requestLine[1];
        QUrl url("http://localhost" + path);
        QUrlQuery query(url);

        // Check for error
        if (query.hasQueryItem("error")) {
            QString error = query.queryItemValue("error");
            qWarning() << "OAuth error:" << error;

            sendResponseToClient(socket, "Authentication failed: " + error);

            setError(error);
            emit authenticationFailed(error);
            setIsAuthenticating(false);

            socket->deleteLater();
            stopLocalServer();
            return;
        }

        // Verify state (CSRF protection)
        QString state = query.queryItemValue("state");
        if (state != m_state) {
            qWarning() << "State mismatch! Possible CSRF attack.";
            sendResponseToClient(socket, "Security error: state mismatch");

            setError("State mismatch");
            emit authenticationFailed("State mismatch");
            setIsAuthenticating(false);

            socket->deleteLater();
            stopLocalServer();
            return;
        }

        // Get authorization code
        QString code = query.queryItemValue("code");
        if (code.isEmpty()) {
            sendResponseToClient(socket, "No authorization code received");

            setError("No authorization code");
            emit authenticationFailed("No authorization code");
            setIsAuthenticating(false);

            socket->deleteLater();
            stopLocalServer();
            return;
        }

        // Send success response to browser
        sendResponseToClient(socket, "Authentication successful! You can close this window.");

        // Exchange code for token
        exchangeCodeForToken(code);

        socket->deleteLater();
        stopLocalServer();
    });
}

void OAuthFlow::exchangeCodeForToken(const QString &code)
{
    qDebug() << "Exchanging authorization code for access token...";

    // Build redirect URI with actual port (must match authorization request)
    QString redirectUri = QString("%1:%2").arg(REDIRECT_URI).arg(m_localPort);

    QUrlQuery postData;
    postData.addQueryItem("client_id", CLIENT_ID);
    postData.addQueryItem("client_secret", CLIENT_SECRET);
    postData.addQueryItem("code", code);
    postData.addQueryItem("code_verifier", m_codeVerifier); // PKCE verifier
    postData.addQueryItem("redirect_uri", redirectUri);
    postData.addQueryItem("grant_type", "authorization_code");

    QUrl url(TOKEN_URL);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkReply *reply = m_networkManager->post(request, postData.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply, &QNetworkReply::finished, this, &OAuthFlow::handleTokenResponse);
}

void OAuthFlow::refreshAccessToken(const QString &refreshToken)
{
    if (refreshToken.isEmpty()) {
        setError("No refresh token available");
        emit authenticationFailed("No refresh token available");
        return;
    }

    qDebug() << "Refreshing access token...";

    QUrlQuery postData;
    postData.addQueryItem("client_id", CLIENT_ID);
    postData.addQueryItem("client_secret", CLIENT_SECRET);
    postData.addQueryItem("refresh_token", refreshToken);
    postData.addQueryItem("grant_type", "refresh_token");

    QUrl url(TOKEN_URL);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkReply *reply = m_networkManager->post(request, postData.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply, &QNetworkReply::finished, this, &OAuthFlow::handleTokenResponse);
}

void OAuthFlow::handleTokenResponse()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        return;
    }

    reply->deleteLater();

    QByteArray responseData = reply->readAll();

    if (reply->error() != QNetworkReply::NoError) {
        QString error = reply->errorString();
        qWarning() << "Token request failed:" << error;

        setError(error);
        emit authenticationFailed(error);
        setIsAuthenticating(false);
        return;
    }

    // Use responseData instead of reading again (buffer would be empty)
    QJsonDocument doc = QJsonDocument::fromJson(responseData);

    if (!doc.isObject()) {
        setError("Invalid response from Google");
        emit authenticationFailed("Invalid response from Google");
        setIsAuthenticating(false);
        return;
    }

    QJsonObject response = doc.object();

    if (response.contains("error")) {
        QString error = response["error_description"].toString();
        if (error.isEmpty()) {
            error = response["error"].toString();
        }
        qWarning() << "Token exchange failed:" << error;

        setError(error);
        emit authenticationFailed(error);
        setIsAuthenticating(false);
        return;
    }

    QString accessToken = response["access_token"].toString();
    QString refreshToken = response["refresh_token"].toString();

    if (accessToken.isEmpty()) {
        setError("No access token received");
        emit authenticationFailed("No access token received");
        setIsAuthenticating(false);
        return;
    }

    qDebug() << "OAuth authentication succeeded!";

    setIsAuthenticating(false);
    emit authenticationSucceeded(accessToken, refreshToken);
}

void OAuthFlow::sendResponseToClient(QTcpSocket *socket, const QString &message)
{
    QString html = QString(
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<title>Pilvi Authentication</title>"
        "<meta charset='utf-8'>"
        "<style>"
        "body { font-family: sans-serif; text-align: center; padding: 50px; background: #f5f5f5; color: #333; }"
        "h1 { color: #4285f4; }"
        ".container { max-width: 600px; margin: 0 auto; background: white; padding: 40px; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }"
        ".icon { font-size: 48px; margin-bottom: 20px; }"
        "</style>"
        "</head>"
        "<body>"
        "<div class='container'>"
        "<div class='icon'>☁️</div>"
        "<h1>Pilvi</h1>"
        "<p>%1</p>"
        "<p><small>You can close this window now.</small></p>"
        "</div>"
        "</body>"
        "</html>"
    ).arg(message);

    QString response = QString(
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Content-Length: %1\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%2"
    ).arg(html.toUtf8().length()).arg(html);

    socket->write(response.toUtf8());
    socket->flush();
}
