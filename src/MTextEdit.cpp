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


bool  MTextEdit::findFirst( QString text)
{
    setCurrentIndex(0);
    _lastFind = text;
#ifdef QSCINTILLA
    return _textEdit->findFirst( text, false, false, false, true);
#else
    return _textEdit->find( text);
#endif
}


bool  MTextEdit::findNext()
{
    setCurrentIndex(0);
#ifdef QSCINTILLA
    return _textEdit->findNext();
#else
    return _textEdit->find( _lastFind);
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
