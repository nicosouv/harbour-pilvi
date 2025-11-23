#!/bin/bash
# Pilvi build script with OAuth client ID configuration

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "======================================"
echo "    Pilvi - Sailfish OS Build"
echo "======================================"
echo ""

# Check if client ID is set
if [ -z "$PILVI_CLIENT_ID" ]; then
    echo -e "${YELLOW}Warning: PILVI_CLIENT_ID environment variable not set${NC}"
    echo ""
    echo "You have several options to set your Google OAuth Client ID:"
    echo ""
    echo "1. Set environment variable (recommended):"
    echo "   export PILVI_CLIENT_ID=\"YOUR_CLIENT_ID.apps.googleusercontent.com\""
    echo "   ./scripts/build.sh"
    echo ""
    echo "2. Pass as argument to this script:"
    echo "   ./scripts/build.sh YOUR_CLIENT_ID.apps.googleusercontent.com"
    echo ""
    echo "3. Edit src/googledrive/oauthflow.cpp directly"
    echo ""
    echo "See docs/OAUTH_SETUP.md for detailed setup instructions."
    echo ""

    # Check if passed as argument
    if [ ! -z "$1" ]; then
        export PILVI_CLIENT_ID="$1"
        echo -e "${GREEN}Using client ID from argument: $PILVI_CLIENT_ID${NC}"
        echo ""
    else
        echo -e "${RED}Error: No client ID provided${NC}"
        echo "Aborting build."
        exit 1
    fi
fi

echo -e "${GREEN}Building with Client ID:${NC} $PILVI_CLIENT_ID"
echo ""

# Build with sfdk
echo "Running sfdk build..."
sfdk build -- -DPILVI_CLIENT_ID="\"$PILVI_CLIENT_ID\""

if [ $? -eq 0 ]; then
    echo ""
    echo -e "${GREEN}✓ Build successful!${NC}"
    echo ""
    echo "To deploy to device:"
    echo "  sfdk deploy --manual"
    echo ""
else
    echo ""
    echo -e "${RED}✗ Build failed${NC}"
    exit 1
fi
