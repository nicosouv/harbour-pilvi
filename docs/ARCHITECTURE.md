# Pilvi Architecture

## Overview

Pilvi is a native Sailfish OS application built with Qt 5.6/QML that provides secure access to Google Drive using OAuth 2.0 with PKCE.

## Technology Stack

- **Language:** C++ (backend) + QML (UI)
- **Framework:** Qt 5.6, Sailfish Silica
- **Build System:** qmake
- **Network:** QNetworkAccessManager
- **Auth:** OAuth 2.0 + PKCE (RFC 7636)
- **Local Server:** QTcpServer (for OAuth callback)
- **Storage:** QSettings (encrypted credentials)

## Architecture Layers

```
┌─────────────────────────────────────┐
│         QML UI Layer                │
│  (Silica Components, Pages)         │
├─────────────────────────────────────┤
│      Business Logic (C++)           │
│  - GoogleDriveApi                   │
│  - OAuthFlow (PKCE)                 │
│  - FileModel                        │
├─────────────────────────────────────┤
│       Data Layer                    │
│  - CredentialStore                  │
│  - FileCache                        │
├─────────────────────────────────────┤
│      Network Layer                  │
│  - QNetworkAccessManager            │
│  - QTcpServer (OAuth callback)      │
└─────────────────────────────────────┘
```

## Key Components

### 1. OAuth Flow (PKCE Implementation)

**File:** `src/googledrive/oauthflow.{h,cpp}`

**Responsibility:** Secure authentication with Google using PKCE

**Flow:**
```
┌──────────┐                    ┌──────────┐                   ┌──────────┐
│  Pilvi   │                    │ Browser  │                   │  Google  │
└────┬─────┘                    └────┬─────┘                   └────┬─────┘
     │                               │                               │
     │ 1. Generate code_verifier     │                               │
     │    (128 random chars)          │                               │
     │                               │                               │
     │ 2. SHA256(verifier)           │                               │
     │    = code_challenge            │                               │
     │                               │                               │
     │ 3. Start TCP server           │                               │
     │    localhost:8080              │                               │
     │                               │                               │
     │ 4. Open authorization URL ───>│                               │
     │    + code_challenge            │                               │
     │    + state (CSRF token)        │                               │
     │                               │                               │
     │                               │ 5. User signs in ───────────>│
     │                               │                               │
     │                               │<─ 6. Authorization granted ──│
     │                               │                               │
     │<─ 7. Redirect to ─────────────│                               │
     │    localhost:8080/callback    │                               │
     │    ?code=xxx&state=yyy         │                               │
     │                               │                               │
     │ 8. Verify state               │                               │
     │                               │                               │
     │ 9. Exchange code + verifier ─────────────────────────────────>│
     │                               │                               │
     │<────────────────────────────── 10. Access + Refresh tokens ──│
     │                               │                               │
     │ 11. Store tokens securely     │                               │
     │                               │                               │
```

**Security Features:**
- PKCE prevents authorization code interception
- State parameter prevents CSRF attacks
- Code verifier never transmitted (only SHA256 hash)
- Each session uses unique cryptographic values
- No client secret needed

**Key Methods:**
- `startAuthentication()` - Initiates OAuth flow
- `generateCodeVerifier()` - Creates random verifier
- `generateCodeChallenge()` - SHA256 + base64url
- `handleIncomingConnection()` - Receives OAuth callback
- `exchangeCodeForToken()` - Gets access token
- `refreshAccessToken()` - Refreshes expired token

### 2. Google Drive API

**File:** `src/googledrive/googledriveapi.{h,cpp}`

**Responsibility:** All Google Drive operations

**Supported Operations:**
- `listFiles()` - Browse folders
- `getFileMetadata()` - File details
- `uploadFile()` - Upload with progress
- `downloadFile()` - Download with progress
- `createFolder()` - New folder
- `deleteFile()` - Delete
- `renameFile()` - Rename
- `moveFile()` - Move to folder
- `copyFile()` - Duplicate
- `starFile()` - Star/unstar
- `shareFile()` - Share with users
- `searchFiles()` - Full-text search
- `getChanges()` - Sync changes
- `getAbout()` - Quota info

**API Design:**
```cpp
class GoogleDriveApi : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(qreal uploadProgress READ uploadProgress ...)
    Q_PROPERTY(qreal downloadProgress READ downloadProgress ...)

signals:
    void filesListed(const QJsonArray &files);
    void fileUploaded(const QJsonObject &metadata);
    void fileDownloaded(const QString &localPath);
    // ... more signals
};
```

**Usage from QML:**
```qml
driveApi.listFiles("root", "")

Connections {
    target: driveApi
    onFilesListed: {
        // Process files array
    }
}
```

### 3. File Model

**File:** `src/models/filemodel.{h,cpp}`

**Responsibility:** QML ListView data model

**Design:**
```cpp
class FileModel : public QAbstractListModel {
    enum FileRoles {
        IdRole, NameRole, MimeTypeRole, SizeRole,
        ModifiedTimeRole, IsFolderRole, StarredRole, ...
    };

    Q_INVOKABLE void addFile(...);
    Q_INVOKABLE void removeFile(int index);
    Q_INVOKABLE FileItem* getFile(int index);
};
```

**Usage:**
```qml
SilicaListView {
    model: fileModel
    delegate: ListItem {
        Label { text: fileName }  // Role auto-mapped
        Label { text: Format.formatFileSize(fileSize) }
    }
}
```

### 4. Credential Store

**File:** `src/storage/credentialstore.{h,cpp}`

**Responsibility:** Secure local storage of OAuth tokens

**Storage:**
```cpp
QSettings settings; // Stored in ~/.config/harbour-pilvi/
settings.setValue("oauth/accessToken", token);
settings.setValue("oauth/refreshToken", refreshToken);
```

**Security:**
- Tokens stored in user's home directory
- File permissions restrict to user only
- No plaintext secrets in code
- Refresh token allows long-term access

### 5. UI Pages (QML)

**Silica Components Used:**

| Component | Usage |
|-----------|-------|
| `SilicaListView` | File lists |
| `PullDownMenu` | Quick actions (upload, refresh, settings) |
| `PushUpMenu` | Search access |
| `ContextMenu` | File operations (rename, delete, etc.) |
| `RemorseItem` | Undo destructive actions |
| `BusyIndicator` | Loading states |
| `ProgressBar` | Upload/download progress |
| `ViewPlaceholder` | Empty states |
| `PageHeader` | Page titles + descriptions |
| `DetailItem` | Key-value displays |
| `SearchField` | File search |
| `Dialog` | Input dialogs |
| `Cover` | Active cover with actions |

**Page Flow:**
```
AuthPage (if not authenticated)
    ↓
MainPage (root folder)
    ↓
FileBrowserPage (subfolder) ←──┐
    ↓                          │
FileDetailsPage (file info)    │
                               │
SearchPage ────────────────────┘
```

## Data Flow Examples

### Upload File Flow

```
User taps "Upload" in PullDownMenu
    ↓
FilePickerPage opens (native file picker)
    ↓
User selects file
    ↓
Signal: fileSelected(filePath)
    ↓
driveApi.uploadFile(filePath, folderId)
    ↓
GoogleDriveApi creates multipart request
    ↓
QNetworkAccessManager sends request
    ↓
Progress signals: uploadProgressChanged
    ↓
QML ProgressBar updates
    ↓
Signal: fileUploaded(metadata)
    ↓
Refresh file list
```

### Authentication Flow

```
App starts
    ↓
Check credentialStore.hasCredentials
    ↓
No credentials → AuthPage
    ↓
User taps "Sign in with Google"
    ↓
oauthFlow.startAuthentication()
    ↓
QTcpServer starts on localhost:8080
    ↓
Browser opens with authorization URL
    ↓
User signs in on Google
    ↓
Google redirects to localhost:8080/callback
    ↓
QTcpServer receives HTTP request
    ↓
Parse code from URL
    ↓
Exchange code for tokens
    ↓
credentialStore.saveCredentials()
    ↓
Signal: authenticationSucceeded
    ↓
Navigate to MainPage
```

## Security Considerations

### OAuth PKCE
- ✅ No client secret in code
- ✅ Each session unique
- ✅ MITM attack prevention
- ✅ Code interception prevention

### Network Security
- ✅ All Google API calls over HTTPS
- ✅ Certificate validation
- ✅ Token expiration handling
- ✅ Automatic refresh token flow

### Local Storage
- ✅ Tokens in QSettings (user-only access)
- ✅ No plaintext secrets
- ✅ Sandboxed app directory

### Input Validation
- ✅ State parameter verification (CSRF)
- ✅ HTTP request parsing validation
- ✅ JSON response validation

## Performance Optimizations

### Network
- Async requests (non-blocking UI)
- Connection reuse (QNetworkAccessManager)
- Efficient JSON parsing
- Chunked uploads for large files

### UI
- QML ListView delegate caching
- Lazy loading of file lists
- Smooth animations (60fps target)
- Efficient property bindings

### Memory
- Proper object ownership (Qt parent-child)
- QNetworkReply cleanup (deleteLater)
- Model reset for large lists

## Future Improvements

### Planned Features
- [ ] Offline sync
- [ ] Conflict resolution
- [ ] Multiple accounts
- [ ] Dropbox support
- [ ] OneDrive support
- [ ] Background sync
- [ ] File thumbnails
- [ ] Document editing
- [ ] Quota visualization

### Performance
- [ ] Incremental file loading (pagination)
- [ ] Better caching strategy
- [ ] Delta sync (changes API)
- [ ] Image thumbnail caching

### UX
- [ ] Drag & drop upload
- [ ] Batch operations
- [ ] Recent files view
- [ ] Starred files view
- [ ] Shared with me view

## Build System

**qmake Configuration:**
```qmake
CONFIG += sailfishapp

# OAuth client ID injection
!isEmpty(PILVI_CLIENT_ID) {
    DEFINES += PILVI_CLIENT_ID=\\\"$${PILVI_CLIENT_ID}\\\"
}

SOURCES += src/**/*.cpp
HEADERS += src/**/*.h
```

**RPM Packaging:**
- YAML spec file in `rpm/`
- Desktop entry for launcher
- Icons (86x86 to 172x172)
- Translations support

## Testing

### Manual Testing Checklist
- [ ] OAuth flow (fresh auth)
- [ ] Token refresh (expired token)
- [ ] File browsing
- [ ] Upload various file types
- [ ] Download files
- [ ] Folder creation
- [ ] Rename/move/delete
- [ ] Search functionality
- [ ] Star/unstar
- [ ] Share with users
- [ ] Error handling (network errors)
- [ ] Cover actions

### Unit Tests (TODO)
- OAuth PKCE generation
- File model operations
- Network request handling

## References

- [RFC 7636 - PKCE](https://tools.ietf.org/html/rfc7636)
- [Google Drive API v3](https://developers.google.com/drive/api/v3/reference)
- [Sailfish Silica Components](https://sailfishos.org/develop/docs/silica/)
- [Qt 5.6 Documentation](https://doc.qt.io/qt-5.6/)
