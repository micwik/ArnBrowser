// Copyright (C) 2010-2016 Michael Wiklund.
// All rights reserved.
// Contact: arnlib@wiklunden.se
//
// This file is part of the ArnLib - Active Registry Network.
// Parts of ArnLib depend on Qt and/or other libraries that have their own
// licenses. Usage of these other libraries is subject to their respective
// license agreements.
//
// GNU General Public License Usage
// This file may be used under the terms of the GNU General Public
// License version 3.0 as published by the Free Software Foundation and appearing
// in the file LICENSE_GPL.txt included in the packaging of this file.
//
// Other Usage
// Alternatively, this file may be used in accordance with the terms and conditions
// contained in a signed written agreement between you and Michael Wiklund.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
//

#include "CodeWindow.hpp"
#include "ui_CodeWindow.h"
#include "MTextEdit.hpp"
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
# include <Qsci/qscilexerjavascript.h>
# include <Qsci/qscilexerhtml.h>
#endif

CodeWindow::CodeWindow( QSettings* appSettings, const ConnectorPath& conPath, QWidget* parent)
    : QDialog( parent),
    _ui( new Ui::CodeWindow)
{
    _ui->setupUi( this);

    this->setWindowTitle( QString("CodeEdit ") + conPath.normPath());

    _appSettings = appSettings;
    readSettings();

    _font = QFont("MonoSpace", 10);
    _font.setStyleHint( QFont::TypeWriter);
    MTextEdit*  textEdit = _ui->textEdit;

#ifdef QSCINTILLA
    QsciLexer*  lexer = 0;
    if (path.endsWith(".js"))
        lexer = new QsciLexerJavaScript;
    if (path.endsWith(".html") || path.endsWith(".xhtml"))
        lexer = new QsciLexerHTML;

    if (lexer) {
        lexer->setFont( _font);
        textEdit->editor()->setLexer( lexer);
    }
    else {
        textEdit->setFont( _font);
    }

    textEdit->editor()->setTabWidth(4);
    textEdit->editor()->setMarginWidth(0, 35);
    textEdit->editor()->setMarginLineNumbers(0, true);
    textEdit->editor()->setUtf8(true);
    textEdit->editor()->setAutoIndent(true);
    textEdit->editor()->setBraceMatching( QsciScintilla::StrictBraceMatch);
    textEdit->editor()->setMatchedBraceForegroundColor( Qt::red);
#else
    QFontMetrics fm( _font);
#if QT_VERSION >= 0x060000
    int  tabWidthInPixels = fm.size( Qt::TextSingleLine, "    ").width();
    textEdit->editor()->setTabStopDistance( qreal( tabWidthInPixels));
#else
    int  tabWidthInPixels = fm.width("    ");
    textEdit->editor()->setTabStopWidth( tabWidthInPixels);
#endif

    textEdit->editor()->setCurrentFont( _font);
#endif

    QString  path = conPath.localPath();
    _isHtml = path.endsWith(".html") || path.endsWith(".xhtml");
    _isSetPlainText = true;  // Default
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


void  CodeWindow::on_forceText_clicked()
{
    setText( getText());
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
    _lastSetText = text;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (!_isHtml || _ui->forceText->isChecked()) {
        _ui->textEdit->setPlainText( text);
        _isSetPlainText = true;
    }
    else {
        _ui->textEdit->setHtml( text);
        _isSetPlainText = false;
    }
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
    QString  text = _isSetPlainText ? _ui->textEdit->text() : _lastSetText;
    QApplication::restoreOverrideCursor();

    return text;
}


void  CodeWindow::keyPressEvent( QKeyEvent* ev)
{
    // qDebug() << "KeyPressEvent: keyCode=" << ev->key();
    QWidget*  widget = QApplication::focusWidget();
    Q_UNUSED(widget);

    switch (ev->key() | ev->modifiers()) {
    case Qt::ControlModifier + Qt::Key_F:
    {   //// Find
        // qDebug() << "KeyPressEvent: key=Ctrl-F";
        ev->accept();

        QString  text = _ui->textEdit->selectedText();
        bool  isOk;
        text = QInputDialog::getText(this, "Find", "Text:", QLineEdit::Normal, text, &isOk);
        if (isOk && !text.isEmpty()) {
            _ui->textEdit->findFirst( text);
        }
        break;
    }
    case Qt::Key_F3:
    {   //// Find next
        // qDebug() << "KeyPressEvent: key=F3";
        ev->accept();

        _ui->textEdit->findNext();
        break;
    }
    case Qt::ShiftModifier + Qt::Key_F3:
    {   //// Find previous
        // qDebug() << "KeyPressEvent: key=Shift-F3";
        ev->accept();

        _ui->textEdit->findPrevious();
        break;
    }
    default:
        QDialog::keyPressEvent( ev);
        break;
    }
}


void  CodeWindow::closeEvent( QCloseEvent* event)
{
    // qDebug() << "Close event";
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
    // qDebug() << "Write code settings";
    _appSettings->setValue("codeEdit/pos", pos());
    _appSettings->setValue("codeEdit/size", size());
}

