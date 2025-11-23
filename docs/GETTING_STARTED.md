# Getting Started with Pilvi

Complete guide to build and run Pilvi on your Sailfish OS device.

## Quick Start

```bash
# 1. Get your Google OAuth Client ID (see OAuth Setup below)

# 2. Clone the repository
cd /path/to/harbour-pilvi

# 3. Build with your client ID
export PILVI_CLIENT_ID="YOUR_CLIENT_ID.apps.googleusercontent.com"
./scripts/build.sh

# 4. Deploy to device
sfdk deploy --manual

# 5. Run and enjoy!
```

## Detailed Setup

### Step 1: Google OAuth Setup (5 minutes)

1. **Create Google Cloud Project**
   - Go to https://console.cloud.google.com/
   - Click "Create Project"
   - Name: "Pilvi" (or anything you like)

2. **Enable Google Drive API**
   - In your project: APIs & Services > Library
   - Search "Google Drive API"
   - Click Enable

3. **Create OAuth Credentials**
   - APIs & Services > Credentials
   - Create Credentials > OAuth 2.0 Client ID
   - Application type: **Desktop app**
   - Name: "Pilvi Sailfish"
   - Click Create
   - Copy the Client ID (e.g., `123456789.apps.googleusercontent.com`)
   - **Important:** You do NOT need the client secret!

4. **Configure OAuth Consent Screen** (if prompted)
   - User Type: External
   - App name: Pilvi
   - Add scope: `../auth/drive`
   - Add yourself as test user

**That's it!** No need to configure redirect URIs - `localhost` is automatically allowed.

**Detailed guide:** [OAUTH_SETUP.md](OAUTH_SETUP.md)

### Step 2: Build the App

#### Method A: Using the build script (easiest)

```bash
# Set your client ID
export PILVI_CLIENT_ID="YOUR_CLIENT_ID.apps.googleusercontent.com"

# Run build script
./scripts/build.sh
```

#### Method B: Direct with sfdk

```bash
sfdk build -- -DPILVI_CLIENT_ID="YOUR_CLIENT_ID.apps.googleusercontent.com"
```

#### Method C: Edit code directly

Open `src/googledrive/oauthflow.cpp` and edit line 14:

```cpp
#define PILVI_CLIENT_ID "YOUR_CLIENT_ID.apps.googleusercontent.com"
```

Then build:
```bash
sfdk build
```

### Step 3: Deploy to Device

```bash
# Deploy via SDK
sfdk deploy --manual

# Or build RPM and install manually
sfdk build
scp RPMS/*.rpm defaultuser@192.168.2.15:~/
ssh defaultuser@192.168.2.15
pkcon install-local harbour-pilvi-*.rpm
```

### Step 4: First Run

1. **Launch Pilvi** from app grid
2. **Tap "Sign in with Google"**
3. **Browser opens automatically** (Sailfish Browser)
4. **Sign in** with your Google account
5. **Authorize Pilvi** to access your Drive
6. **Browser shows success** - you can close it
7. **Return to Pilvi** - you're authenticated!
8. **Browse your files** and enjoy!

## Features Overview

### File Management
- Browse folders and files
- Upload files from device
- Download files to device
- Create new folders
- Rename files/folders
- Move files between folders
- Delete files/folders
- Star/favorite files

### Search
- Full-text search across all files
- Filter by file name
- Quick access to results

### File Sharing
- Share files with specific users
- Set permissions (view/comment/edit)
- Email notifications

### UI Features
- Pull-down menus for quick actions
- Context menus for file operations
- Remorse actions for undo
- Progress indicators for uploads/downloads
- Cover actions for background control
- Beautiful Silica native design

## Troubleshooting

### "OAuth not configured" error
**Problem:** Client ID not set during build
**Solution:** Rebuild with client ID (see Step 2 above)

### "Failed to start local server" error
**Problem:** Port 8080-8090 already in use
**Solution:** Close other apps using these ports, or restart device

### "Authentication failed" in browser
**Problem:** OAuth consent screen not configured, or app not verified
**Solution:**
- Add yourself as test user in Google Cloud Console
- If "App not verified" warning, click "Advanced" > "Go to Pilvi (unsafe)"
  (This is normal for apps in testing mode)

### Browser doesn't redirect back
**Problem:** Browser didn't handle the redirect
**Solution:**
- Manually close browser
- Return to Pilvi
- May need to retry authentication

### "Invalid client" error
**Problem:** Client ID incorrect or app type wrong
**Solution:**
- Verify client ID is copied correctly
- Ensure you created "Desktop app" not "Web application"

### Quota exceeded errors
**Problem:** Too many API requests
**Solution:**
- Free tier: 10,000 requests per 100 seconds
- Wait a bit and try again
- For heavy usage, consider paid tier

## Advanced Configuration

### Build for Different Architectures

```bash
# ARM (most devices)
sfdk build

# i486 (emulator)
sfdk config --task SailfishOS-latest-i486
sfdk build
```

### Debug Build

```bash
sfdk build --debug
sfdk deploy --debug
# Logs in journal
devel-su journalctl -f | grep pilvi
```

### Multiple Google Accounts

Currently Pilvi supports one account at a time. To switch accounts:
1. Settings > Sign out
2. Sign in with different account

Multi-account support planned for future release.

## Next Steps

- Read [OAUTH_COMPARISON.md](OAUTH_COMPARISON.md) to understand the security model
- Check out the code to see how it works
- Contribute improvements!
- Report bugs on GitHub

## Support

- **Documentation:** Check `docs/` folder
- **Issues:** Open on GitHub
- **Questions:** Discussion board

## Privacy & Security

- **Your data stays yours:** Pilvi only requests access to your Drive
- **Local storage:** Credentials stored locally on device
- **No tracking:** No analytics, no telemetry
- **Open source:** Audit the code yourself
- **PKCE security:** Industry-standard OAuth without secrets

Enjoy Pilvi! ☁️
