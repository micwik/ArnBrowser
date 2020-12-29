# -------------------------------------------------
# Project created by QtCreator 2010-08-12T21:08:40
# -------------------------------------------------

CONFIG += ArnLibCompile

# Usage of running qml script in Arn
CONFIG += QmlRun

# Usage of js aware code editor
# CONFIG += qscintilla

# Usage of internal mDNS code (no external dependency)
win32 {
    CONFIG += mDnsIntern
}

QMAKE_CXXFLAGS_WARN_ON = -Wall -Wno-deprecated-declarations

QT += core gui
QT += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET = ArnBrowser
TEMPLATE = app
OBJECTS_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp
UI_DIR = tmp

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


QmlRun {
    ARN += qml
    DEFINES += QMLRUN
    greaterThan(QT_MAJOR_VERSION, 4) {
        QT += qml quick
    } else {
        QT += declarative
    }
    SOURCES += src/QmlRunWindow.cpp
    HEADERS += src/QmlRunWindow.hpp
}

INCLUDEPATH += src $$PWD/.. $$PWD/../include

greaterThan(QT_MAJOR_VERSION, 5) {
    ARNLIB = Arn6
} else {
    greaterThan(QT_MAJOR_VERSION, 4) {
        ARNLIB = Arn5
    } else {
        ARNLIB = Arn4
    }
}

ArnLibCompile {
    ARN += client
    ARN += discover
    include(../ArnLib/src/ArnLib.pri)
    INCLUDEPATH += $$PWD/../ArnLib/src
} else {
    win32: INCLUDEPATH += $$PWD/../ArnLib/src

    win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ArnLib/release/ -l$${ARNLIB}
    else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ArnLib/debug/ -l$${ARNLIB}
    else:unix: LIBS += -L$$OUT_PWD/../ArnLib/ -l$${ARNLIB}
}

!mDnsIntern {
    win32:CONFIG(release, debug|release): LIBS +=  -ldns_sd
    else:win32:CONFIG(debug, debug|release): LIBS +=  -ldns_sd
    else:unix: LIBS += -ldns_sd
}

qscintilla {
    DEFINES += QSCINTILLA

    win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QScintilla/Qt4/release/ -lqscintilla2
    else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QScintilla/Qt4/debug/ -lqscintilla2
    else:unix: LIBS += -L$$OUT_PWD/../QScintilla/Qt4/ -lqscintilla2

    INCLUDEPATH += $$PWD/../QScintilla/Qt4
}
