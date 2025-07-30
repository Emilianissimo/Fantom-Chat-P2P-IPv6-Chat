QT += core widgets network linguist

TARGET = FantomChat
TEMPLATE = app

SOURCES += \
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
    src/models/ContactListModel.h \
    src/network/IPv6ChatClient.h \
    src/network/IPv6ChatServer.h \
    src/models/MessageListModel.h \
    src/utils/ProtocolUtils.h \
    src/utils/Requests.h \
    src/utils/Structures.h \
    src/ui/main_window/mainwindow.h

FORMS += \
    src/templates/mainwindow.ui

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
    QMAKE_POST_LINK += cmd /c \"$$BAT_PATH\" \"$$OUT_PWD\"
}

macx {
    LIBS += -L$$CURL_ROOT/lib -lcurl
    QMAKE_POST_LINK +=
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
    copy_dlls.bat \
    src/styles/mainwindow.qss \
    third_party_licenses/FontAwesome-LICENSE.md \
    third_party_licenses/Qt-LICENSE.md

# Set on to run on local IPv6 network (for Debugging)
# local_network = false

# !isEmpty(local_network) {
#     message("🔧 Local IPv6 network mode ENABLED")
#     DEFINES += USE_LOCAL_IPV6
# } else {
#     message("🌐 Remote network mode")
# }

RESOURCES += \
    resources.qrc
