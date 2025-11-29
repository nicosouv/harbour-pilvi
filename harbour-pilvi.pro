TARGET = harbour-pilvi

CONFIG += sailfishapp

# OAuth Configuration - loaded from .qmake.conf
CLIENT_ID = $$pilvi_client_id
CLIENT_SECRET = $$pilvi_client_secret

message("=== OAuth Configuration Debug ===")
message("pilvi_client_id from .qmake.conf: $$pilvi_client_id")
message("pilvi_client_secret from .qmake.conf: $$pilvi_client_secret")
message("CLIENT_ID variable: $$CLIENT_ID")
message("CLIENT_SECRET variable: $$CLIENT_SECRET")

isEmpty(CLIENT_ID) {
    warning("✗ No PILVI_CLIENT_ID defined - OAuth will not work!")
    warning("✗ Set PILVI_CLIENT_ID environment variable before building")
    DEFINES += PILVI_CLIENT_ID=\\\"\\\"
} else {
    message("✓ Building with OAuth Client ID: $$CLIENT_ID")
    DEFINES += PILVI_CLIENT_ID=\\\"$$CLIENT_ID\\\"
}

isEmpty(CLIENT_SECRET) {
    warning("✗ No PILVI_CLIENT_SECRET defined - OAuth will not work!")
    warning("✗ Set PILVI_CLIENT_SECRET environment variable before building")
    DEFINES += PILVI_CLIENT_SECRET=\\\"\\\"
} else {
    message("✓ Building with OAuth Client Secret (hidden)")
    DEFINES += PILVI_CLIENT_SECRET=\\\"$$CLIENT_SECRET\\\"
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
    qml/pages/FilePickerPage.qml \
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
