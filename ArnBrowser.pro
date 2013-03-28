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

SOURCES += src/main.cpp \
    src/MainWindow.cpp \
    src/TermWindow.cpp \
    src/MultiDelegate.cpp \
    src/Math.cpp \
    src/CodeWindow.cpp \
    src/ArnModel.cpp

HEADERS += src/MainWindow.hpp \
    src/TermWindow.hpp \
    src/MultiDelegate.hpp \
    src/ItemDataRole.hpp \
    src/Math.hpp \
    src/CodeWindow.hpp \
    src/ArnModel.hpp \
    src/MTextEdit.hpp

FORMS += src/MainWindow.ui \
    src/TermWindow.ui \
    src/CodeWindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ArnLib/release/ -lArn
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ArnLib/debug/ -lArn
else:unix: LIBS += -L$$OUT_PWD/../ArnLib/ -lArn

INCLUDEPATH += src $$PWD/.. $$PWD/../include


qscintilla {
  DEFINES += QSCINTILLA

  win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QScintilla/Qt4/release/ -lqscintilla2
  else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QScintilla/Qt4/debug/ -lqscintilla2
  else:unix: LIBS += -L$$OUT_PWD/../QScintilla/Qt4/ -lqscintilla2

  INCLUDEPATH += $$PWD/../QScintilla/Qt4
}
