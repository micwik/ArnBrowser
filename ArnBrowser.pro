# -------------------------------------------------
# Project created by QtCreator 2010-08-12T21:08:40
# -------------------------------------------------

CONFIG += ArnLibCompile

# Usage of running qml script in Arn (must be Qt5)
greaterThan(QT_MAJOR_VERSION, 4): CONFIG += QmlRun

# Usage of js aware code editor
# CONFIG += qscintilla

# Usage of internal mDNS code (no external dependency)
CONFIG += mDnsIntern

QT += core gui
QT += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ArnBrowser
TEMPLATE = app
OBJECTS_DIR = tmp
MOC_DIR = tmp
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
    src/QmlRunWindow.cpp

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
    src/QmlRunWindow.hpp

FORMS += src/MainWindow.ui \
    src/TermWindow.ui \
    src/CodeWindow.ui \
    src/ManageWindow.ui \
    src/VcsWindow.ui \
    src/DiscoverWindow.ui

RESOURCES += \
    src/files.qrc

OTHER_FILES += \
    README.md \
    Install.md


QmlRun {
    ARN += qml
    DEFINES += QMLRUN
    SOURCES +=
    HEADERS +=
}

INCLUDEPATH += src $$PWD/.. $$PWD/../include

greaterThan(QT_MAJOR_VERSION, 4) {
    ARNLIB = Arn5
} else {
    ARNLIB = Arn4
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
