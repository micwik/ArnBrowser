# -------------------------------------------------
# Project created by QtCreator 2010-08-12T21:08:40
# -------------------------------------------------

CONFIG += ArnLibCompile

# Usage of js aware code editor
# CONFIG += qscintilla

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
    src/DiscoverWindow.cpp

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
    src/DiscoverWindow.hpp

FORMS += src/MainWindow.ui \
    src/TermWindow.ui \
    src/CodeWindow.ui \
    src/ManageWindow.ui \
    src/VcsWindow.ui \
    src/DiscoverWindow.ui

greaterThan(QT_MAJOR_VERSION, 4) {
    ARNLIB = Arn5
} else {
    ARNLIB = Arn
}

INCLUDEPATH += src $$PWD/.. $$PWD/../include

ArnLibCompile {
    ARN += server
    ARN += discover
    CONFIG += mDnsIntern
    include(../ArnLib/src/ArnLib.pri)
    INCLUDEPATH += $$PWD/../ArnLib/src
} else {
    win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ArnLib/release/ -l$${ARNLIB}
    else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ArnLib/debug/ -l$${ARNLIB}
    else:unix: LIBS += -L$$OUT_PWD/../ArnLib/ -l$${ARNLIB}
}


qscintilla {
  DEFINES += QSCINTILLA

  win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QScintilla/Qt4/release/ -lqscintilla2
  else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QScintilla/Qt4/debug/ -lqscintilla2
  else:unix: LIBS += -L$$OUT_PWD/../QScintilla/Qt4/ -lqscintilla2

  INCLUDEPATH += $$PWD/../QScintilla/Qt4
}

RESOURCES += \
    src/files.qrc
