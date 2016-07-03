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

#include "MTextEdit.hpp"


MTextEdit::MTextEdit( QWidget* parent)
    : QStackedWidget( parent)
{
    setupEditor();

    _textView = new QTextEdit;
    _textView->setReadOnly( true);

    addWidget( _textEdit);
    addWidget( _textView);
}


QString  MTextEdit::selectedText()
{
#ifdef QSCINTILLA
    return QString();  // TODO: fixme
#else
    QTextCursor  textCursor = _textEdit->textCursor();
    return textCursor.selectedText();
#endif
}


bool  MTextEdit::findFirst( QString text)
{
    setCurrentIndex(0);
    _lastFind = text;
#ifdef QSCINTILLA
    return _textEdit->findFirst( text, false, false, false, true);
#else
    if (!_textEdit->find( _lastFind)) {
        _textEdit->moveCursor( QTextCursor::Start);
        return _textEdit->find( _lastFind);
    }
    return true;
#endif
}


bool  MTextEdit::findNext()
{
    setCurrentIndex(0);
#ifdef QSCINTILLA
    return _textEdit->findNext();
#else
    if (!_textEdit->find( _lastFind)) {
        _textEdit->moveCursor( QTextCursor::Start);
        return _textEdit->find( _lastFind);
    }
    return true;
#endif
}


bool  MTextEdit::findPrevious()
{
    setCurrentIndex(0);
#ifdef QSCINTILLA
    return false;  // TODO: fixme
#else
    if (!_textEdit->find( _lastFind, QTextDocument::FindBackward)) {
        _textEdit->moveCursor( QTextCursor::End);
        return _textEdit->find( _lastFind, QTextDocument::FindBackward);
    }
    return true;
#endif
}


void  MTextEdit::setPlainText( const QString& text)
{
    setCurrentIndex(0);
#ifdef QSCINTILLA
    _textEdit->setText( text);
#else
    _textEdit->setPlainText( text);
#endif
}


void  MTextEdit::setHtml( const QString& text)
{
    setCurrentIndex(1);
    _textView->setHtml( text);
#ifdef QSCINTILLA
    _textEdit->setText( text);
#else
    _textEdit->setPlainText( text);
#endif
}


QString  MTextEdit::text()  const
{
#ifdef QSCINTILLA
    return _textEdit->text();
#else
    return _textEdit->toPlainText();
#endif
}


MTextEditor* MTextEdit::editor() const
{
    return _textEdit;
}


void  MTextEdit::setupEditor()
{
#ifdef QSCINTILLA
    _textEdit = new QsciScintilla;
#else
    _textEdit = new QTextEdit;
#endif
}
