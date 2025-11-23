# Pilvi - Multi-cloud Storage Client for Sailfish OS

A beautiful, native multi-cloud storage client for Sailfish OS with full Google Drive support.

## Features

### Google Drive Integration
- **Full OAuth 2.0 authentication** - Secure login with Google account
- **File browsing** - Navigate folders with intuitive UI
- **File operations** - Upload, download, rename, move, delete files
- **Search** - Quick search across all your files
- **Starring** - Mark important files as favorites
- **Sharing** - Share files with specific users and permissions
- **Progress tracking** - Real-time upload/download progress
- **Metadata viewing** - File details, sizes, modification dates

### UI/UX Excellence
- **Full Silica components** - Native Sailfish OS design language
- **Pulley menus** - Quick access to common actions
- **Context menus** - File-specific operations
- **Cover actions** - Control from home screen
- **Search field** - Built-in search functionality
- **Progress indicators** - Visual feedback for operations
- **Remorse actions** - Undo destructive operations
- **View placeholders** - Helpful empty states
- **Responsive design** - Works in all orientations

### Performance
- **Async operations** - Non-blocking network requests
- **Progress tracking** - Real-time feedback
- **Efficient caching** - Smart local storage
- **Optimized models** - Fast list rendering

## Building

### Requirements
- Sailfish SDK
- Qt 5.6
- Google Drive API Client ID (free, see setup below)

### OAuth Setup

This app uses **OAuth 2.0 with PKCE** - no client secret needed!

1. Create Google Cloud project and enable Drive API
2. Create OAuth 2.0 Desktop app credentials
3. Copy your Client ID

**Detailed instructions:** See [docs/OAUTH_SETUP.md](docs/OAUTH_SETUP.md)

### Build

```bash
# Build with your client ID
sfdk build -- -DPILVI_CLIENT_ID="YOUR_CLIENT_ID.apps.googleusercontent.com"

# Or set environment variable
export PILVI_CLIENT_ID="YOUR_CLIENT_ID.apps.googleusercontent.com"
sfdk build

# Deploy to device
sfdk deploy --manual
```

## Usage

1. Launch Pilvi
2. Tap "Sign in with Google"
3. Browser opens automatically
4. Sign in and authorize
5. Return to Pilvi - done!
6. Browse, upload, download your files

## Architecture

### Backend (C++)
- `GoogleDriveApi` - Main API service layer
- `OAuthFlow` - Authentication handling
- `CredentialStore` - Secure credential storage
- `FileModel` - List model for files
- `FileItem` - Individual file representation

### Frontend (QML)
- `MainPage` - Root folder view
- `FileBrowserPage` - Folder navigation
- `FileDetailsPage` - File information and actions
- `AuthPage` - OAuth authentication
- `SearchPage` - File search
- `SettingsPage` - App settings

## Future Plans

- Dropbox support
- OneDrive support
- NextCloud/ownCloud support
- Offline sync
- Conflict resolution
- Multiple accounts
- Background sync

## License

MIT

## Contributing

Contributions welcome! Please open issues and pull requests.
