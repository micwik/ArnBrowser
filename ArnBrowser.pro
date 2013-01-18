# -------------------------------------------------
# Project created by QtCreator 2010-08-12T21:08:40
# -------------------------------------------------

# Usage of js aware code editor
# CONFIG += qscintilla

QT += core gui
QT += network

TARGET = ArnBrowser
TEMPLATE = app
OBJECTS_DIR = tmp
MOC_DIR = tmp
UI_DIR = tmp

SOURCES += main.cpp \
    MainWindow.cpp \
    TermWindow.cpp \
    MultiDelegate.cpp \
    Math.cpp \
    CodeWindow.cpp \
    ArnModel.cpp

HEADERS += MainWindow.hpp \
    TermWindow.hpp \
    MultiDelegate.hpp \
    ItemDataRole.hpp \
    Math.hpp \
    CodeWindow.hpp \
    ArnModel.hpp \
    MTextEdit.hpp

FORMS += MainWindow.ui \
    TermWindow.ui \
    CodeWindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ArnLib/release/ -lArn
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ArnLib/debug/ -lArn
else:unix: LIBS += -L$$OUT_PWD/../ArnLib/ -lArn

INCLUDEPATH += $$PWD/..


qscintilla {
  DEFINES += QSCINTILLA

  win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QScintilla/Qt4/release/ -lqscintilla2
  else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QScintilla/Qt4/debug/ -lqscintilla2
  else:unix: LIBS += -L$$OUT_PWD/../QScintilla/Qt4/ -lqscintilla2

  INCLUDEPATH += $$PWD/../QScintilla/Qt4
}
