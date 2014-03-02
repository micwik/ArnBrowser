// Copyright (C) 2010-2014 Michael Wiklund.
// All rights reserved.
// Contact: arnlib@wiklunden.se
//
// This file is part of the ArnBrowser - Active Registry Network Browser.
// Parts of ArnBrowser depend on Qt 4 and/or other libraries that have their own
// licenses. ArnBrowser is independent of these licenses; however, use of these other
// libraries is subject to their respective license agreements.
//
// GNU Lesser General Public License Usage
// This file may be used under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation and
// appearing in the file LICENSE.LGPL included in the packaging of this file.
// In addition, as a special exception, you may use the rights described
// in the Nokia Qt LGPL Exception version 1.1, included in the file
// LGPL_EXCEPTION.txt in this package.
//
// GNU General Public License Usage
// Alternatively, this file may be used under the terms of the GNU General
// Public License version 3.0 as published by the Free Software Foundation
// and appearing in the file LICENSE.GPL included in the packaging of this file.
//
// Other Usage
// Alternatively, this file may be used in accordance with the terms and
// conditions contained in a signed written agreement between you and Michael Wiklund.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//

#include "CodeWindow.hpp"
#include "ui_CodeWindow.h"
#include "MTextEdit.hpp"
#include <QFont>
#include <QFontMetrics>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QSettings>
#include <QCloseEvent>
#include <QDebug>

#ifdef QSCINTILLA
#  include <Qsci/qscilexerjavascript.h>
#endif

CodeWindow::CodeWindow( QSettings* appSettings, const QString& path, QWidget* parent) :
    QDialog( parent),
    _ui( new Ui::CodeWindow)
{
    _ui->setupUi( this);

    this->setWindowTitle( QString("CodeEdit ") + path);

    _appSettings = appSettings;
    readSettings();

    QFont  font("MonoSpace", 12);
    MTextEdit*  textEdit = _ui->textEdit;

#ifdef QSCINTILLA
    QsciLexerJavaScript*  lexer = 0;
    if (path.endsWith(".js"))
        lexer = new QsciLexerJavaScript;

    if (lexer) {
        lexer->setFont( font);
        textEdit->setLexer( lexer);
    }
    else {
        textEdit->setFont( font);
    }

    textEdit->setTabWidth(4);
    textEdit->setMarginWidth(0, 35);
    textEdit->setMarginLineNumbers(0, true);
    textEdit->setUtf8(true);
    textEdit->setAutoIndent(true);
    textEdit->setBraceMatching( QsciScintilla::StrictBraceMatch);
    textEdit->setMatchedBraceForegroundColor( Qt::red);
#else
    QFontMetrics fm( font);
    int  tabWidthInPixels = fm.width("    ");
    textEdit->setTabStopWidth( tabWidthInPixels);
    textEdit->setCurrentFont( font);
#endif

    _arnItem.open( path);
    setText( _arnItem.toString());
}


CodeWindow::~CodeWindow()
{
    delete _ui;
}


void  CodeWindow::on_loadFileButton_clicked()
{
    QString  fileName = QFileDialog::getOpenFileName( this);
    if (!fileName.isEmpty())
        loadFile( fileName);
}


void  CodeWindow::on_saveFileButton_clicked()
{
    QString  fileName = QFileDialog::getSaveFileName( this);
    if (!fileName.isEmpty())
        saveFile( fileName);
}


void  CodeWindow::on_reLoadButton_clicked()
{
    setText( _arnItem.toString());
}


void  CodeWindow::on_saveButton_clicked()
{
    _arnItem = getText();
}


void  CodeWindow::on_findButton_clicked()
{
    bool isOk;
    QString  text = QInputDialog::getText(this, tr("Find"),
                                          tr("Text:"), QLineEdit::Normal,
                                          QString(), &isOk);
    if (isOk && !text.isEmpty()) {
        _ui->textEdit->findFirst( text);
    }
}


void  CodeWindow::on_findNextButton_clicked()
{
    _ui->textEdit->findNext();
}


void  CodeWindow::loadFile( const QString& fileName)
{
    QFile  file( fileName);
    if (!file.open( QFile::ReadOnly)) {
        QMessageBox::warning( this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg( fileName)
                             .arg( file.errorString()));
        return;
    }

    QTextStream in( &file);
    setText( in.readAll());

    //setCurrentFile(fileName);
    //statusBar()->showMessage(tr("File loaded"), 2000);
}


void  CodeWindow::setText( const QString& text)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    _ui->textEdit->setText( text);
    QApplication::restoreOverrideCursor();
}


void  CodeWindow::saveFile( const QString& fileName)
{
    QFile file( fileName);
    if (!file.open( QFile::WriteOnly)) {
        QMessageBox::warning( this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg( fileName)
                             .arg( file.errorString()));
        return;
    }

    QTextStream  out( &file);
    out << getText();

    //setCurrentFile( fileName);
    //statusBar()->showMessage(tr("File saved"), 2000);
    return;
}


QString  CodeWindow::getText()  const
{
    QApplication::setOverrideCursor( Qt::WaitCursor);
    QString  text = _ui->textEdit->text();
    QApplication::restoreOverrideCursor();

    return text;
}


void  CodeWindow::closeEvent( QCloseEvent* event)
{
    qDebug() << "Close event";
    writeSettings();
    event->accept();
    deleteLater();
}


void  CodeWindow::readSettings()
{
    QPoint  pos = _appSettings->value("codeEdit/pos", QPoint(200, 200)).toPoint();
    QSize  size = _appSettings->value("codeEdit/size", QSize(400, 400)).toSize();
    resize( size);
    move( pos);
}


void  CodeWindow::writeSettings()
{
    qDebug() << "Write code settings";
    _appSettings->setValue("codeEdit/pos", pos());
    _appSettings->setValue("codeEdit/size", size());
}

