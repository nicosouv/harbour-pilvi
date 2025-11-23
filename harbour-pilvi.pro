TARGET = harbour-pilvi

CONFIG += sailfishapp

# OAuth Configuration
# Pass client ID via: sfdk build -- -DPILVI_CLIENT_ID="xxx.apps.googleusercontent.com"
# Or set environment variable: PILVI_CLIENT_ID

message("=== OAuth Configuration Debug ===")
message("PILVI_CLIENT_ID (qmake var before env check): $${PILVI_CLIENT_ID}")

# Try to get from environment variable first
isEmpty(PILVI_CLIENT_ID) {
    message("PILVI_CLIENT_ID qmake var is empty, checking environment...")
    PILVI_CLIENT_ID = $$(PILVI_CLIENT_ID)
    message("PILVI_CLIENT_ID from environment: $${PILVI_CLIENT_ID}")
} else {
    message("PILVI_CLIENT_ID already set as qmake var: $${PILVI_CLIENT_ID}")
}

!isEmpty(PILVI_CLIENT_ID) {
    DEFINES += PILVI_CLIENT_ID=\\\"$${PILVI_CLIENT_ID}\\\"
    message("✓ Building with OAuth Client ID: $${PILVI_CLIENT_ID}")
    message("✓ DEFINES contains: PILVI_CLIENT_ID=\\\"$${PILVI_CLIENT_ID}\\\"")
} else {
    warning("✗ PILVI_CLIENT_ID not set - OAuth will not work!")
    warning("✗ See docs/OAUTH_SETUP.md for setup instructions")
}
message("=================================")

SOURCES += \
    src/harbour-pilvi.cpp \
    src/googledrive/googledriveapi.cpp \
    src/googledrive/oauthflow.cpp \
    src/models/filemodel.cpp \
    src/models/fileitem.cpp \
    src/network/networkrequest.cpp \
    src/storage/credentialstore.cpp \
    src/storage/filecache.cpp

HEADERS += \
    src/googledrive/googledriveapi.h \
    src/googledrive/oauthflow.h \
    src/models/filemodel.h \
    src/models/fileitem.h \
    src/network/networkrequest.h \
    src/storage/credentialstore.h \
    src/storage/filecache.h

DISTFILES += \
    qml/harbour-pilvi.qml \
    qml/pages/MainPage.qml \
    qml/pages/FileBrowserPage.qml \
    qml/pages/FileDetailsPage.qml \
    qml/pages/SettingsPage.qml \
    qml/pages/AuthPage.qml \
    qml/pages/SearchPage.qml \
    qml/cover/CoverPage.qml \
    qml/dialogs/InputDialog.qml \
    qml/dialogs/ShareDialog.qml \
    rpm/harbour-pilvi.spec \
    harbour-pilvi.desktop \
    harbour-pilvi.png \
    translations/*.ts

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172

CONFIG += sailfishapp_i18n

TRANSLATIONS += translations/harbour-pilvi-en.ts
