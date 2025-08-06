#include "src/ui/main_window/mainwindow.h"
#include "src/font-awesome/AwesomeGlobal.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QFile>
#include <curl/curl.h>
#include <QCommandLineParser>

fa::QtAwesome* awesome = nullptr;


int main(int argc, char *argv[])
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    QApplication a(argc, argv);

    // Setting up --local directive to provide local network mode
    QCommandLineParser parser;
    parser.setApplicationDescription("Fantom Chat - P2P IPv6 direct connect");
    parser.addHelpOption();
    parser.addOption({{"l", "local"}, "Use local IPv6 mode."});
    parser.process(a);
    a.setProperty("local_network", parser.isSet("local"));

    a.setStyle("Fusion");

    // Inititalizing style resources
    QFile f(":/assets/styles/mainwindow.qss");
    if (!f.open(QFile::ReadOnly)) {
        qDebug() << "Failed to load stylesheet";
    } else {
        qDebug() << "Stylesheet loaded";
        a.setStyleSheet(f.readAll());
    }

    // Inititalizing font awesome resource
    awesome = new fa::QtAwesome(qApp);
    awesome->initFontAwesome();

    // Registering meta tupes for delegates
    qRegisterMetaType<Message>("Message");
    qRegisterMetaType<Contact>("Contact");

    MainWindow w;

    // Initializing translator
    QSettings settings("config.ini", QSettings::IniFormat);
    QString langCode = settings.value("language", "en").toString();
    w.switchLanguage(langCode);

    w.show();

    return a.exec();
}
