# ARN += core       # Level 1: Basic Arn functionality without any tcp and syncing
ARN += client     # Level 2: Client TCP functionality with sync etc
# ARN += server     # Level 3: Server TCP functionality with persistence etc
# ARN += scriptjs   # Java script support QT >= 5 (new QJSEngine)
# ARN += script     # Java script support QT <= 5 (legacy QtScriptEngine)
# ARN += scriptauto # Java script support depend on Qt version & ArnLib: scriptjs (prefered) or script
ARN += qml        # QML support
# ARN += zeroconf   # Using part of Bonjour (R), Apple's (R) implementation of zero-configuration networking.
ARN += discover   # High level service discovery using <zeroconf> and optionally <server> for remote config.
#
# CONFIG += ArnLibCompile  # Compile ArnLib source into application. Be ware, breaks LGPL.
# CONFIG += ArnRealFloat   # Use float as real type, default is double. Must be same in application & lib pro-file.
# CONFIG += mDnsIntern     # Use internal mDNS code for zero-config (no external dependency)
#
QMFEATURES=$$[QMAKEFEATURES]
isEmpty(QMFEATURES) {
    win32: QMAKEFEATURES *= $$PWD/../qtfeatures   # '*=' because of 3 passes under Windows
    else:  QMAKEFEATURES *= /usr/include/qtfeatures
    cache(QMAKEFEATURES, set transient super)
}
# load(arnlib)      # Alt. 1) Load arnlib.prf here
CONFIG += arnlib  # Alt. 2) Load arnlib.prf after parsing project file

# Usage of js aware code editor
# CONFIG += qscintilla

win32 {
    CONFIG += mDnsIntern
    CONFIG += ArnLibCompile
}

# CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS_WARN_ON = -Wall -Wno-deprecated-declarations

QT += gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ArnBrowser
TEMPLATE = app
OBJECTS_DIR = build
MOC_DIR = build
RCC_DIR = build
UI_DIR = build

SOURCES += src/main.cpp \
    src/MainWindow.cpp \
    src/TermWindow.cpp \
    src/MultiDelegate.cpp \
    src/Math.cpp \
    src/CodeWindow.cpp \
    src/ArnModel.cpp \
    src/ManageWindow.cpp \
    src/VcsWindow.cpp \
    src/DiscoverWindow.cpp \
    src/MTextEdit.cpp \
    src/Connector.cpp \
    src/LogWindow.cpp \
    src/LoginDialog.cpp \
    src/SettingsWindow.cpp \
    src/ChatServWindow.cpp \
    src/SettingsHandler.cpp

HEADERS += src/MainWindow.hpp \
    src/TermWindow.hpp \
    src/MultiDelegate.hpp \
    src/ItemDataRole.hpp \
    src/Math.hpp \
    src/CodeWindow.hpp \
    src/ArnModel.hpp \
    src/MTextEdit.hpp \
    src/ManageWindow.hpp \
    src/VcsWindow.hpp \
    src/DiscoverWindow.hpp \
    src/Connector.hpp \
    src/LogWindow.hpp \
    src/LoginDialog.hpp \
    src/SettingsWindow.hpp \
    src/ChatServWindow.hpp \
    src/SettingsHandler.hpp

FORMS += src/MainWindow.ui \
    src/TermWindow.ui \
    src/CodeWindow.ui \
    src/ManageWindow.ui \
    src/VcsWindow.ui \
    src/DiscoverWindow.ui \
    src/LogWindow.ui \
    src/LoginDialog.ui \
    src/SettingsWindow.ui \
    src/ChatServWindow.ui

RESOURCES += \
    src/files.qrc

RC_FILE = src/arnbrowser_ico.rc

OTHER_FILES += \
    README.md \
    doc/Install.md \
    doc/Todo.md


contains(ARN, qml) {
    DEFINES += QMLRUN
    SOURCES += src/QmlRunWindow.cpp
    HEADERS += src/QmlRunWindow.hpp
}


INCLUDEPATH += src
#INCLUDEPATH += src $$PWD/.. $$PWD/../include


qscintilla {
    DEFINES += QSCINTILLA

    win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QScintilla/Qt4/release/ -lqscintilla2
    else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QScintilla/Qt4/debug/ -lqscintilla2
    else:unix: LIBS += -L$$OUT_PWD/../QScintilla/Qt4/ -lqscintilla2

    INCLUDEPATH += $$PWD/../QScintilla/Qt4
}
