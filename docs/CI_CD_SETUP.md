# CI/CD Setup Guide

This document explains how to set up GitHub Actions CI/CD for building and releasing Pilvi RPM packages.

## Overview

The CI/CD pipeline automatically:
- ✅ Builds RPMs for all architectures (armv7hl, aarch64, i486)
- ✅ Runs validation checks on pull requests
- ✅ Creates GitHub releases with RPM attachments
- ✅ Generates changelogs automatically

## Workflows

### 1. PR Build Check (`pr-build.yml`)

**Triggers:**
- Pull requests to `main` branch
- Direct pushes to `main` branch

**What it does:**
- Builds RPM for armv7hl (fast arch check)
- Validates that build succeeds
- Provides quick feedback on PRs

**No release is created** - only build validation.

### 2. Build and Release (`build-release.yml`)

**Triggers:**
- Git tags matching `v*.*.*` (e.g., `v1.0.0`, `v0.2.1`)
- Manual workflow dispatch

**What it does:**
1. Builds RPMs for all architectures (armv7hl, aarch64, i486)
2. Generates changelog from git commits
3. Creates GitHub release
4. Uploads RPMs as release assets

## Required Secrets

### PILVI_CLIENT_ID

Your Google OAuth 2.0 Client ID for building the app.

**How to set up:**

1. **Get your Client ID** (if you don't have one):
   - Follow [OAUTH_SETUP.md](OAUTH_SETUP.md)
   - Copy the Client ID (e.g., `123456.apps.googleusercontent.com`)

2. **Add to GitHub Secrets**:
   - Go to your repo: Settings > Secrets and variables > Actions
   - Click "New repository secret"
   - Name: `PILVI_CLIENT_ID`
   - Value: Your full client ID
   - Click "Add secret"

## Creating a Release

### Method 1: Git Tag (Recommended)

```bash
# Make sure you're on main and up to date
git checkout main
git pull

# Create and push a version tag
git tag v1.0.0
git push origin v1.0.0

# GitHub Actions will automatically:
# - Build RPMs for all architectures
# - Generate changelog
# - Create GitHub release
```

### Method 2: Manual Trigger

1. Go to: Actions > Build and Release
2. Click "Run workflow"
3. Select branch (usually `main`)
4. Click "Run workflow"

This builds a dev version (`0.1.0-dev`) without creating a release.

## Version Numbering

Follow [Semantic Versioning](https://semver.org/):

- `v1.0.0` - Major release (breaking changes)
- `v1.1.0` - Minor release (new features, backward compatible)
- `v1.1.1` - Patch release (bug fixes)

**Examples:**
```bash
git tag v0.1.0  # First beta
git tag v0.2.0  # Second beta with new features
git tag v1.0.0  # First stable release
git tag v1.0.1  # Bug fix
git tag v1.1.0  # New feature
```

## Architecture Support

| Architecture | Devices | Priority |
|--------------|---------|----------|
| **armv7hl** | Jolla 1, Jolla C, Xperia X, XA2 | High (most common) |
| **aarch64** | Xperia 10 II, III, IV, V | High (newer devices) |
| **i486** | Emulator only | Low (testing only) |

## Build Process

### What happens during build:

1. **Checkout code** - Gets your latest code
2. **Extract version** - From git tag or default to `0.1.0-dev`
3. **Update spec** - Injects version into RPM spec
4. **Inject Client ID** - Sets `PILVI_CLIENT_ID` environment variable
5. **Build RPM** - Uses official Sailfish SDK Docker image
6. **Upload artifacts** - Saves RPMs for release

### Build environment:

- **OS:** Ubuntu (latest)
- **SDK:** Sailfish OS SDK via Docker
- **Release:** 4.5.0.18 (configurable in workflow)
- **Builder:** R1tschY/sailfish-build-rpm action

## Changelog Generation

Changelogs are auto-generated from git commit messages.

**Best practices for commit messages:**

```bash
# Good commits (will look nice in changelog)
git commit -m "Add file sharing functionality"
git commit -m "Fix download progress indicator"
git commit -m "Improve search performance"

# Less good (but still works)
git commit -m "fix stuff"
git commit -m "wip"
```

**Generated changelog example:**
```markdown
## What's New in v1.0.0

- Add file sharing functionality (a1b2c3d)
- Fix download progress indicator (e4f5g6h)
- Improve search performance (i7j8k9l)
```

## Testing the CI/CD

### Test PR Build

1. Create a branch
2. Make changes
3. Open PR to `main`
4. CI runs automatically
5. Check build status in PR

### Test Release Build

```bash
# Create a test tag
git tag v0.0.1-test
git push origin v0.0.1-test

# Check Actions tab
# Delete tag after testing:
git tag -d v0.0.1-test
git push origin :refs/tags/v0.0.1-test
```

## Troubleshooting

### Build fails with "PILVI_CLIENT_ID not set"

**Cause:** Secret not configured in GitHub

**Fix:**
1. Go to repo Settings > Secrets
2. Add `PILVI_CLIENT_ID` secret
3. Re-run workflow

### Release not created

**Cause:** Tag doesn't match `v*.*.*` pattern

**Fix:**
```bash
# Wrong
git tag 1.0.0        # Missing 'v'
git tag version-1.0  # Wrong format

# Correct
git tag v1.0.0
```

### No RPMs in release

**Cause:** Build failed or no tag pushed

**Fix:**
1. Check Actions tab for build errors
2. Ensure tag is pushed: `git push origin v1.0.0`
3. Check workflow logs

### Wrong version in RPM

**Cause:** Tag format incorrect or not on tagged commit

**Fix:**
```bash
# Make sure tag is on latest commit
git log --oneline -1
git tag v1.0.0
git push origin v1.0.0
```

## Advanced Configuration

### Change Sailfish Release Version

Edit `.github/workflows/*.yml`:

```yaml
env:
  SAILFISH_RELEASE: '4.6.0.11'  # Update this
```

### Build Only Specific Architectures

Edit `.github/workflows/build-release.yml`:

```yaml
strategy:
  matrix:
    arch:
      - armv7hl  # Remove others if needed
      # - i486
      # - aarch64
```

### Add Build Steps

Add steps in workflow file:

```yaml
- name: Run tests
  run: |
    # Add test commands

- name: Validate spec
  run: |
    # Add validation
```

## Security Considerations

### Secrets Safety

- ✅ Client ID is safe to be in GitHub Secrets
- ✅ Secrets are never exposed in logs
- ✅ PRs from forks cannot access secrets (by default)
- ✅ Each workflow run is isolated

### Client ID in Builds

The Client ID is compiled into the RPM during CI build. This is:
- ✅ **Safe** - Client ID is not a secret (by design)
- ✅ **Standard** - OAuth 2.0 + PKCE allows public client IDs
- ✅ **Secure** - Security comes from PKCE, not client ID

See [OAUTH_COMPARISON.md](OAUTH_COMPARISON.md) for security details.

## Monitoring

### Check Build Status

- **Actions tab** - See all workflow runs
- **Commits** - Green ✅ or red ❌ next to each commit
- **PRs** - Build status at bottom of PR
- **Email** - GitHub sends notifications on failures

### Build Badges

Add to README.md:

```markdown
![Build](https://github.com/USERNAME/harbour-pilvi/workflows/Build%20and%20Release/badge.svg)
```

## Resources

- [GitHub Actions Docs](https://docs.github.com/en/actions)
- [Sailfish Build Action](https://github.com/R1tschY/sailfish-build-rpm)
- [Semantic Versioning](https://semver.org/)

## Support

Questions about CI/CD setup?
- Open an issue
- Check GitHub Actions logs
- Review this documentation
