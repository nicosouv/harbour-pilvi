# OAuth Methods Comparison for Sailfish OS

## Summary: We use PKCE (Best for Pilvi)

| Method | Client Secret? | UX | Security | Distribution |
|--------|---------------|-----|----------|--------------|
| **PKCE** ✅ | ❌ No | ⭐⭐⭐⭐⭐ Auto redirect | ⭐⭐⭐⭐⭐ Very secure | ✅ Public safe |
| Device Flow | ❌ No | ⭐⭐⭐ Manual code entry | ⭐⭐⭐⭐⭐ Very secure | ✅ Public safe |
| Standard OAuth | ✅ Yes | ⭐⭐⭐⭐⭐ Auto redirect | ⭐⭐⭐ Secret exposure risk | ❌ Risky |

## Detailed Comparison

### 1. OAuth 2.0 with PKCE (Current Implementation)

**Flow:**
1. App generates random code_verifier
2. Creates SHA256 hash → code_challenge
3. Opens browser with authorization URL + code_challenge
4. User signs in on Google
5. Google redirects to `http://localhost:8080/callback?code=xxx`
6. App's local TCP server receives the code
7. App exchanges code + code_verifier for tokens (no secret needed!)

**Pros:**
- ✅ No client secret needed
- ✅ Seamless UX (automatic redirect)
- ✅ Industry standard for native apps
- ✅ Recommended by Google for mobile/desktop
- ✅ Each session uses unique cryptographic verifier
- ✅ Safe to distribute publicly
- ✅ Same approach as major apps (VS Code, etc.)

**Cons:**
- Requires opening browser
- Local server must be able to bind port (usually no problem)

**Security:**
- PKCE prevents authorization code interception
- State parameter prevents CSRF
- Code verifier is never transmitted (only SHA256 hash)
- Each authentication session is unique

**References:**
- [RFC 7636 - PKCE](https://tools.ietf.org/html/rfc7636)
- [Google OAuth for Native Apps](https://developers.google.com/identity/protocols/oauth2/native-app)

---

### 2. Device Flow (Alternative)

**Flow:**
1. App requests device code from Google
2. Google returns: device_code, user_code, verification_url
3. App displays: "Go to google.com/device and enter: ABC-DEF"
4. User opens browser manually, enters code
5. App polls Google every 5 seconds
6. Once user authorizes, app gets tokens

**Pros:**
- ✅ No client secret needed
- ✅ Works without local server
- ✅ Good for devices with limited input
- ✅ Safe to distribute publicly

**Cons:**
- ❌ Manual code entry (worse UX)
- ❌ User must switch apps manually
- ❌ Polling creates unnecessary requests
- ❌ User could enter code incorrectly

**When to use:**
- Devices without browser integration
- Smart TVs, IoT devices
- When you can't bind local server port

---

### 3. Standard OAuth (Not Recommended)

**Flow:**
1. App opens browser with client_id + client_secret
2. User authorizes
3. Redirect to local server
4. Exchange code for token using client_secret

**Pros:**
- ✅ Simple implementation
- ✅ Automatic redirect

**Cons:**
- ❌ Client secret in code = security risk
- ❌ Anyone can decompile and steal secret
- ❌ Violates OAuth best practices for native apps
- ❌ Google recommends against this

**Why we don't use it:**
- Client secret cannot be kept secret in a native app
- Anyone with the app binary can extract it
- Compromised secret allows impersonation
- Not suitable for public distribution

---

## Why GitDeck Uses Client Secret

GitDeck uses client_id + client_secret because:

1. **GitHub's API design** - GitHub OAuth was designed before PKCE
2. **Optional PKCE** - GitHub supports PKCE but doesn't require it
3. **Compile-time config** - Uses `#define` to allow users to add their own credentials
4. **Each user can use their own** - Users can compile with their own GitHub OAuth app

**For Pilvi (Google Drive), PKCE is better because:**
- Google fully supports and recommends PKCE
- Simpler for users (no secret to manage)
- More secure
- Standard approach for Google APIs

---

## Security Analysis

### PKCE Security Model

**Code Verifier:**
- Random 128-character string
- Generated fresh for each auth session
- Never leaves the device until token exchange
- Only the SHA256 hash is sent in authorization

**Protection against:**
- ✅ Authorization code interception
- ✅ Man-in-the-middle attacks
- ✅ Code replay attacks
- ✅ Malicious apps trying to steal codes

**Attack scenarios prevented:**

1. **Attacker intercepts authorization code:**
   - Without code_verifier, code is useless
   - Verifier never transmitted over network during auth
   - Only sent during token exchange (HTTPS)

2. **Malicious app registers same redirect URI:**
   - Even if they get the code, no verifier = no token
   - Google validates code_challenge matches code_verifier

3. **Source code inspection:**
   - Client ID is not secret (by design)
   - No client secret to steal
   - Code verifier is random per session

---

## Distribution Models

### For Public App Store (Harbour/OpenRepos)

**Option 1: Shared Client ID (Recommended)**
```cpp
#define PILVI_CLIENT_ID "123456.apps.googleusercontent.com"
```

- All users use your client ID
- You manage the Google Cloud project
- Users just install and sign in
- Quotas shared across all users (usually fine)

**Option 2: User-Provided Client ID**
```cpp
// Add UI for users to enter their client ID
Settings {
    TextField {
        label: "Google Client ID"
        text: settings.clientId
    }
}
```

- Each user creates their own Google project
- More technical
- Full control, separate quotas
- Better for power users

---

## Conclusion

**PKCE is the clear winner for Pilvi:**

1. ✅ Best security without client secret
2. ✅ Best user experience (auto redirect)
3. ✅ Google's recommended approach
4. ✅ Safe for public distribution
5. ✅ Industry standard for native apps

The implementation is based on GitDeck's proven OAuth approach, adapted to use PKCE instead of client secret, giving us the best of both worlds: GitDeck's seamless local server UX + PKCE's superior security model.
