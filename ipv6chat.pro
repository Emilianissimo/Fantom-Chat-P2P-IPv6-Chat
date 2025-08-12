QT += core widgets network

TARGET = FantomChat
TEMPLATE = app

CONFIG += c++20

SOURCES += \
    src/encrypting/sodium/backends/SodiumCryptoBackend.cpp \
    src/encrypting/sodium/key_pairs/SodiumKeyPair.cpp \
    src/encrypting/sodium/sessions/SodiumSession.cpp \
    src/models/ContactListModel.cpp \
    src/network/IPv6ChatClient.cpp \
    src/network/IPv6ChatServer.cpp \
    src/models/MessageListModel.cpp \
    src/ui/contacts/delegates/ContactsDelegate.cpp \
    src/utils/Requests.cpp \
    main.cpp \
    src/ui/main_window/mainwindow.cpp \
    src/ui/chat/delegates/ChatMessageDelegate.cpp \
    src/ui/contacts/delegates/ContactsDelegate.cpp

HEADERS += \
    src/encrypting/interfaces/ICryptoError.h \
    src/encrypting/sodium/backends/SodiumCryptoBackend.h \
    src/encrypting/interfaces/ICryptoBackend.h \
    src/encrypting/interfaces/ICryptoKeyPair.h \
    src/encrypting/interfaces/ICryptoSession.h \
    src/encrypting/sodium/errors/SodiumCryptoError.h \
    src/encrypting/sodium/key_pairs/SodiumKeyPair.h \
    src/encrypting/sodium/sessions/SodiumSession.h \
    src/models/ContactListModel.h \
    src/network/IPv6ChatClient.h \
    src/network/IPv6ChatServer.h \
    src/models/MessageListModel.h \
    src/utils/ProtocolUtils.h \
    src/utils/Requests.h \
    src/utils/Structures.h \
    src/ui/main_window/mainwindow.h

FORMS += \
    assets/templates/mainwindow.ui

TRANSLATIONS = \
    translations/en.ts \
    translations/ru.ts \
    translations/sindarin.ts

win32 {
    OPENSSL_ROOT = C:/msys64/ucrt64
    ZLIB_ROOT = C:/msys64/ucrt64
    CURL_ROOT = C:/msys64/ucrt64
    SODIUM_ROOT = C:/msys64/ucrt64

    # Connect OpenSSL
    INCLUDEPATH += $$OPENSSL_ROOT/include
    LIBS += -L$$OPENSSL_ROOT/lib -lssl -lcrypto
    # Connect CURL
    INCLUDEPATH += $$CURL_ROOT/include
    LIBS += -L$$CURL_ROOT/lib -lcurl -lws2_32 -lwsock32 -lcrypt32
    # set needed DLLs
    BAT_PATH = $$PWD/win32/copy_dlls.bat
    QMAKE_POST_LINK += cmd /c \"$$BAT_PATH\" \"$$OUT_PWD\"

    # Connect Zlib
    INCLUDEPATH += $$ZLIB_ROOT/include
    LIBS += $$ZLIB_ROOT/lib/libz.a

    # Connect libsodium
    INCLUDEPATH += $$SODIUM_ROOT/include
    LIBS += -L$$SODIUM_ROOT/lib -lsodium
}

macx {
    SODIUM_ROOT = /opt/homebrew/opt/libsodium
    ZLIB_ROOT = /opt/homebrew/opt/zlib
    CONFIG += app_bundle
    LIBS += -lcurl
    # Connect Zlib
    INCLUDEPATH += $$ZLIB_ROOT/include
    LIBS += $$ZLIB_ROOT/lib/libz.a

    # Connect libsodium
    INCLUDEPATH += $$SODIUM_ROOT/include
    LIBS += -L$$SODIUM_ROOT/lib -lsodium
}

unix:!macx {
    LIBS += -lcurl -lsodium -lz
}

# Set path for installation
target.path = $$[QT_INSTALL_BINS]
INSTALLS += target

CONFIG+=fontAwesomeFree
include(dependencies/QtAwesome/QtAwesome.pri)

DISTFILES += \
    .gitignore \
    LICENSE.md \
    README.md \
    TESTING_NOTES.md \
    assets/styles/general.qss \
    copy_dlls.bat \
    assets/images/app_icon.rc \
    assets/styles/mainwindow.qss \
    assets/fonts/tngan.ttf \
    third_party_licenses/FontAwesome-LICENSE.md \
    third_party_licenses/Qt-LICENSE.md

RESOURCES += \
    resources.qrc

# Icon
RC_FILE = assets/images/app_icon.rc

