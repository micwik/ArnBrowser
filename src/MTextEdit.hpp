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

#ifndef MTEXTEDIT_HPP
#define MTEXTEDIT_HPP

#include <QString>


/// Wrapper for textediting using QScintilla or QTextEdit

#ifdef QSCINTILLA
#  include <Qsci/qsciscintilla.h>

class MTextEdit : public QsciScintilla
{
public:
    MTextEdit( QWidget *parent = 0) : QsciScintilla( parent) {}
    bool findFirst( QString text) {
        return QsciScintilla::findFirst( text, false, false, false, true);
    }
};

#else
#  include <QTextEdit>

class MTextEdit : public QTextEdit
{
    QString  _lastFind;

public:
    MTextEdit( QWidget *parent = 0) : QTextEdit( parent) {}
    bool findFirst( QString text) {
        _lastFind = text;
        return find( text);
    }
    bool findNext() {
        return find( _lastFind);
    }
    QString  text() {
        return toPlainText();
    }
};

#endif

#endif // MTEXTEDIT_HPP
