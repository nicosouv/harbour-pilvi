# OAuth 2.0 Setup for Pilvi

Pilvi uses **OAuth 2.0 with PKCE** (Proof Key for Code Exchange) for secure authentication with Google Drive. This approach does NOT require a client secret, making it safe for public distribution.

## Why PKCE?

- ✅ **No client secret needed** - Only client ID is required
- ✅ **Secure** - Uses cryptographic code challenge/verifier
- ✅ **Google approved** - Recommended for native/mobile apps
- ✅ **Public distribution safe** - Client ID can be in source code
- ✅ **Better UX** - Automatic redirect via local server

## Setup Instructions

### 1. Create Google Cloud Project

1. Go to [Google Cloud Console](https://console.cloud.google.com/)
2. Create a new project or select existing
3. Name it "Pilvi" (or your preference)

### 2. Enable Google Drive API

1. In your project, go to **APIs & Services > Library**
2. Search for "Google Drive API"
3. Click **Enable**

### 3. Create OAuth 2.0 Credentials

1. Go to **APIs & Services > Credentials**
2. Click **Create Credentials > OAuth 2.0 Client ID**
3. Configure OAuth consent screen if prompted:
   - User Type: **External**
   - App name: **Pilvi**
   - User support email: Your email
   - Scopes: Add `../auth/drive` (full Drive access)
   - Test users: Add your email for testing

4. Create OAuth Client ID:
   - Application type: **Desktop app** (NOT Web application!)
   - Name: **Pilvi Sailfish**
   - Click **Create**

5. Copy the **Client ID** (looks like `xxx.apps.googleusercontent.com`)
   - You do NOT need the client secret for PKCE!

### 4. Configure Authorized Redirect URIs

**IMPORTANT:** For desktop/native apps with PKCE, Google automatically allows:
- `http://localhost:*` (any port)
- `http://127.0.0.1:*`

Our app uses `http://localhost:8080/callback` - this works by default!

### 5. Build the App

#### Option A: Via command line define

```bash
sfdk build -- -DPILVI_CLIENT_ID="YOUR_CLIENT_ID.apps.googleusercontent.com"
```

#### Option B: Edit the code directly

Open `src/googledrive/oauthflow.cpp` and replace:

```cpp
#ifndef PILVI_CLIENT_ID
#define PILVI_CLIENT_ID ""  // <-- Put your client ID here
#endif
```

With:

```cpp
#ifndef PILVI_CLIENT_ID
#define PILVI_CLIENT_ID "YOUR_CLIENT_ID.apps.googleusercontent.com"
#endif
```

#### Option C: Use environment variable

```bash
export PILVI_CLIENT_ID="YOUR_CLIENT_ID.apps.googleusercontent.com"
sfdk build
```

### 6. Test

1. Build and deploy the app
2. Launch Pilvi
3. Tap "Sign in with Google"
4. Browser opens automatically
5. Sign in with your Google account
6. Authorize Pilvi
7. Browser shows success page
8. Return to Pilvi - you're authenticated!

## Security Notes

### Why is Client ID public?

- OAuth 2.0 with PKCE is **designed** for public clients
- Client ID is not a secret - it identifies your app
- Security comes from:
  - PKCE code verifier/challenge (random, per-session)
  - State parameter (CSRF protection)
  - User must authorize on their own Google account
  - Tokens are stored locally, never shared

### What about client secret?

- **NOT NEEDED** with PKCE!
- Client secret is only for server-side apps
- Desktop/mobile apps cannot keep secrets
- Google documentation recommends PKCE without secret for native apps

## Publishing Your App

### For Public Distribution

You have 2 options:

#### Option 1: Share Your Client ID (Recommended)

1. Publish your app with your client ID in the code
2. Users get your app from Harbour/OpenRepos
3. All users authenticate with **your** client ID
4. Each user authorizes their own Google account
5. Perfectly safe and standard practice

**Pros:**
- Simple for users
- One configuration
- Standard approach

**Cons:**
- All requests counted against your quota (usually fine for small apps)

#### Option 2: Let Users Use Their Own Client ID

1. Make Client ID configurable in settings
2. Users create their own Google Cloud project
3. Users enter their own client ID
4. More technical but gives full control

**Pros:**
- No shared quota
- Full user control

**Cons:**
- More complex for users
- Requires technical knowledge

## Quota and Limits

Google Drive API quotas (free tier):
- 10,000 requests per 100 seconds per project
- 1,000 requests per 100 seconds per user

For a personal app, this is usually plenty!

## References

- [RFC 7636 - PKCE](https://tools.ietf.org/html/rfc7636)
- [Google OAuth 2.0 for Mobile & Desktop](https://developers.google.com/identity/protocols/oauth2/native-app)
- [OAuth 2.0 Best Practices](https://tools.ietf.org/html/draft-ietf-oauth-security-topics)
