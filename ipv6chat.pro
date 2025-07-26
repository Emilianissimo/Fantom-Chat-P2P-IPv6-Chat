QT += core widgets network linguist

TARGET = ipv6chat
TEMPLATE = app

SOURCES += \
    IPv6ChatClient.cpp \
    IPv6ChatServer.cpp \
    MessageListModel.cpp \
    Requests.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    IPv6ChatClient.h \
    IPv6ChatServer.h \
    MessageListModel.h \
    ProtocolUtils.h \
    Requests.h \
    Structures.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

TRANSLATIONS = ipv6chat_en_US.ts

win32 {
    OPENSSL_ROOT = C:/msys64/ucrt64
    ZLIB_ROOT = C:/msys64/ucrt64
    CURL_ROOT = C:/msys64/ucrt64
}

macx {
    OPENSSL_ROOT = /opt/homebrew/opt/openssl@3
    ZLIB_ROOT = /opt/homebrew/opt/zlib
    CURL_ROOT =
}

# Connect OpenSSL
INCLUDEPATH += $$OPENSSL_ROOT/include
LIBS += -L$$OPENSSL_ROOT/lib -lssl -lcrypto

# Connect Zlib
INCLUDEPATH += $$ZLIB_ROOT/include
LIBS += $$ZLIB_ROOT/lib/libz.a

# Connect CURL
INCLUDEPATH += $$CURL_ROOT/include

win32 {
    LIBS += -L$$CURL_ROOT/lib -lcurl -lws2_32 -lwsock32 -lcrypt32
    # set needed DLLs
    BAT_PATH = $$PWD/win32/copy_dlls.bat
    QMAKE_POST_LINK += call \"$$BAT_PATH\" \"$$OUT_PWD\"
}

macx {
    LIBS += -L$$CURL_ROOT/lib -lcurl
    QMAKE_POST_LINK +=
}



# Set path for installation
target.path = $$[QT_INSTALL_BINS]
INSTALLS += target

CONFIG+=fontAwesomeFree
include(QtAwesome/QtAwesome.pri)

DISTFILES += \
    .gitignore \
    LICENSE.md \
    README.md \
    TESTING_NOTES.md \
    copy_dlls.bat \
    third_party_licenses/FontAwesome-LICENSE.md \
    third_party_licenses/Qt-LICENSE.md

# Set on to run on local IPv6 network
local_network = true

!isEmpty(local_network) {
    message("🔧 Local IPv6 network mode ENABLED")
    DEFINES += USE_LOCAL_IPV6
} else {
    message("🌐 Remote network mode")
}
