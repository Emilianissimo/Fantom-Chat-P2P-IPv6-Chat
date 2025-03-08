QT += core widgets network linguist

TARGET = ipv6chat
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

TRANSLATIONS = ipv6chat_en_US.ts

# Connect OpenSSL
# Win
OPENSSL_ROOT = C:/msys64/ucrt64
INCLUDEPATH += $$OPENSSL_ROOT/include
LIBS += -L$$OPENSSL_ROOT/lib -lssl -lcrypto

# Connect Zlib
# Win
ZLIB_ROOT = C:/msys64/ucrt64
INCLUDEPATH += $$ZLIB_ROOT/include
LIBS += $$ZLIB_ROOT/lib/libz.a

# Подключение CURL
LIBS += -lcurl

# Установка путей при инсталляции
target.path = $$[QT_INSTALL_BINS]
INSTALLS += target

CONFIG+=fontAwesomeFree
include(QtAwesome/QtAwesome.pri)

DISTFILES += \
    .gitignore
