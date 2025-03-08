#include "mainwindow.h"
#include "src/headers/AwesomeGlobal.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>


fa::QtAwesome* awesome = nullptr;


int main(int argc, char *argv[])
{
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
