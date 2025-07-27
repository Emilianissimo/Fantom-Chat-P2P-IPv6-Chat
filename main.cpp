#include "mainwindow.h"
#include "src/font-awesome/AwesomeGlobal.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <curl/curl.h>

fa::QtAwesome* awesome = nullptr;


int main(int argc, char *argv[])
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    QApplication a(argc, argv);

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

    MainWindow w;
    w.show();

    return a.exec();
}
