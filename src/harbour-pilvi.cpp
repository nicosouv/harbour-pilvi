#include <QtQuick>
#include <sailfishapp.h>
#include "googledrive/googledriveapi.h"
#include "googledrive/oauthflow.h"
#include "models/filemodel.h"
#include "storage/credentialstore.h"

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    app->setOrganizationName("harbour-pilvi");
    app->setApplicationName("harbour-pilvi");

    QScopedPointer<QQuickView> view(SailfishApp::createView());

    // Register types
    qmlRegisterType<GoogleDriveApi>("harbour.pilvi.googledrive", 1, 0, "GoogleDriveApi");
    qmlRegisterType<OAuthFlow>("harbour.pilvi.googledrive", 1, 0, "OAuthFlow");
    qmlRegisterType<FileModel>("harbour.pilvi.models", 1, 0, "FileModel");

    // Create and expose singletons
    CredentialStore *credentialStore = new CredentialStore(app.data());
    GoogleDriveApi *driveApi = new GoogleDriveApi(credentialStore, app.data());

    view->rootContext()->setContextProperty("driveApi", driveApi);
    view->rootContext()->setContextProperty("credentialStore", credentialStore);

    view->setSource(SailfishApp::pathTo("qml/harbour-pilvi.qml"));
    view->show();

    return app->exec();
}
