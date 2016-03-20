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

#ifndef CHATSERV_HPP
#define CHATSERV_HPP

#include <ArnInc/ArnItem.hpp>
#include <QDialog>

class QSettings;
class QCloseEvent;

namespace Ui {
    class ChatServWindow;
}


class ChatServWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ChatServWindow( QSettings* appSettings, QWidget* parent = 0);
    ~ChatServWindow();

//public slots:
    void  addTxtNormal( const QString& text);
    void  addTxtPrio( const QString& text);

signals:
    void  txtOutput( const QString& text);
    void  abortKillRequest();

private slots:
    void  on_closeButton_clicked();
    void  on_clearButton_clicked();
    void  on_abortKillButton_clicked();
    void  doLineInput();
    void  readSettings();
    void  writeSettings();

protected:
    void closeEvent( QCloseEvent *event);

private:
    void  addTxtLocal( const QString& text);

    Ui::ChatServWindow *_ui;
    QSettings*  _appSettings;
};

#endif // CHATSERV_HPP
