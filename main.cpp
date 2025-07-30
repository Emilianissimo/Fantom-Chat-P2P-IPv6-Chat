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
    QCommandLineParser parser;
    parser.setApplicationDescription("IPv6 Chat Client");
    parser.addHelpOption();
    parser.addOption({{"l", "local"}, "Use local IPv6 mode."});
    parser.process(a);
    a.setProperty("local_network", parser.isSet("local"));

    a.setStyle("Fusion");

    QFile f(":/src/styles/mainwindow.qss");
    if (!f.open(QFile::ReadOnly)) {
        qDebug() << "Failed to load stylesheet";
    } else {
        qDebug() << "Stylesheet loaded";
        a.setStyleSheet(f.readAll());
    }

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "ipv6chat_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    awesome = new fa::QtAwesome(qApp);
    awesome->initFontAwesome();

    qRegisterMetaType<Message>("Message");
    qRegisterMetaType<Contact>("Contact");

    MainWindow w;

    w.show();

    return a.exec();
}
