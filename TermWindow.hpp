// Copyright (C) 2010-2013 Michael Wiklund.
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

#ifndef TERMWINDOW_HPP
#define TERMWINDOW_HPP

#include "ArnLib/Arn.hpp"
#include <QDialog>

class QSettings;
class QCloseEvent;

namespace Ui {
    class TermWindow;
}


class TermWindow : public QDialog
{
    Q_OBJECT

public:
    explicit TermWindow( QSettings* appSettings, const QString& path, QWidget* parent = 0);
    ~TermWindow();

private slots:
    void  doPipeInput( QString text);
    void  doPipe2Input( QString text);
    void  doLineInput();
    void  readSettings();
    void  writeSettings();

protected:
    void closeEvent( QCloseEvent *event);

private:
    Ui::TermWindow *_ui;
    ArnItem  _pipe;
    ArnItem  _pipe2;
    QSettings*  _appSettings;
};

#endif // TERMWINDOW_HPP
