#include <QApplication>
#include <QCommandLineParser>
#include <QDataStream>
#include <QDebug>
#include <QFileInfo>
#include <QLocalServer>
#include <QLocalSocket>
#include <QSettings>
#include <QThread>
#include <QFile>

#ifdef Q_OS_WIN
#include "RegWin.h"
#endif

#include "UIManager.h"

namespace {
    constexpr const char* kServerName = "OurPaintSingleInstance";

    bool trySendToRunningInstance(const QStringList& paths, int timeoutMs = 500) {
        if (paths.isEmpty())
            return false;

        QLocalSocket socket;
        socket.connectToServer(kServerName);
        if (!socket.waitForConnected(timeoutMs))
            return false;

        QDataStream stream(&socket);
        stream << paths;
        socket.flush();
        socket.waitForBytesWritten(timeoutMs);
        return true;
    }
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    Q_INIT_RESOURCE(resources);
    Q_INIT_RESOURCE(translations);

    QCoreApplication::setOrganizationName("OurPaint");
    QCoreApplication::setApplicationName("OurPaint");

#ifdef Q_OS_WIN
    QSettings appSettings;
    if (!appSettings.value("fileAssociationRegistered", false).toBool()) {
        AppUI::registerFileAssociation();
        appSettings.setValue("fileAssociationRegistered", true);
    }
#endif

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument(
        "files",
        QCoreApplication::translate("main", "Paths to .ourp project files."),
        "[files...]");
    parser.process(app);

    QStringList filePaths;
    for (const QString& f : parser.positionalArguments()) {
        if (QFileInfo(f).suffix().compare("ourp", Qt::CaseInsensitive) == 0)
            filePaths.append(f);
        else
            qWarning() << "[main] Not a .ourp file:" << f;
    }

    if (trySendToRunningInstance(filePaths))
        return 0;

    QLocalServer server;
    bool isServer = server.listen(kServerName);

    if (!isServer) {
        for (int i = 0; i < 10 && !isServer; ++i) {
            if (trySendToRunningInstance(filePaths, 200))
                return 0;
            QThread::msleep(50);
            isServer = server.listen(kServerName);
        }
    }

    if (!isServer) {
        QLocalServer::removeServer(kServerName);
        isServer = server.listen(kServerName);
        if (!isServer) {
            qWarning() << "[main] Cannot listen:" << server.errorString();
        }
    }

    AppUI::UIManager uiManager;

    if (isServer) {
        QObject::connect(&server, &QLocalServer::newConnection, [&server, &uiManager]() {
            while (QLocalSocket* client = server.nextPendingConnection()) {
                QObject::connect(client, &QLocalSocket::readyRead, [client, &uiManager]() {
                    QDataStream stream(client);
                    QStringList received;
                    stream >> received;
                    if (!received.isEmpty())
                        uiManager.openFilesFromCommandLine(received);
                    client->deleteLater();
                });
            }
        });
    }

    if (!filePaths.isEmpty())
        uiManager.openFilesFromCommandLine(filePaths);

    return QApplication::exec();
}