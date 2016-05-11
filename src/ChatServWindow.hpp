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

    void  reset();
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
    void  closeEvent( QCloseEvent *event);

private:
    void  addTxtLocal( const QString& text);

    Ui::ChatServWindow *_ui;
    QSettings*  _appSettings;
};

#endif // CHATSERV_HPP
