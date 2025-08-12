#include "src/ui/main_window/mainwindow.h"
#include "src/font-awesome/AwesomeGlobal.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QFile>
#include <curl/curl.h>
#include <QCommandLineParser>
#include <QDir>
#include <QLockFile>
#include <QStandardPaths>

fa::QtAwesome* awesome = nullptr;


int main(int argc, char *argv[])
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("EmilErofeevskiy");
    QCoreApplication::setApplicationName("FantomChat");
    // Prevent from initializing copy of instance of application
    // Single instance lock
    const QString lockDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(lockDir);
    QLockFile lockFile(lockDir + "/fantomchat.instance.lock");
    lockFile.setStaleLockTime(30000);
    if (!lockFile.tryLock()) {
        if (!lockFile.removeStaleLockFile() || !lockFile.tryLock())
            return 0;
    }

    // Setting up --local directive to provide local network mode
    QCommandLineParser parser;
    parser.setApplicationDescription("Fantom Chat - P2P IPv6 direct connect");
    parser.addHelpOption();
    parser.addOption({{"l", "local"}, "Use local IPv6 mode."});
    parser.process(a);
    a.setProperty("local_network", parser.isSet("local"));

    a.setStyle("Fusion");

    // Inititalizing font awesome resource
    awesome = new fa::QtAwesome(qApp);
    awesome->initFontAwesome();

    // Registering meta tupes for delegates
    qRegisterMetaType<Message>("Message");
    qRegisterMetaType<Contact>("Contact");

    MainWindow w;

    w.show();
    int rc = a.exec();
    curl_global_cleanup();
    return rc;
}
