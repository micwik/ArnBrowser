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

#ifndef TERMWINDOW_HPP
#define TERMWINDOW_HPP

#include "Connector.hpp"
#include <ArnInc/ArnItem.hpp>
#include <QDialog>
#include <QStringList>

class QSettings;
class QCloseEvent;
class QKeyEvent;

namespace Ui {
    class TermWindow;
}


class TermWindow : public QDialog
{
    Q_OBJECT

public:
    explicit TermWindow( QSettings* appSettings, const ConnectorPath& conPath, QWidget* parent = 0);
    ~TermWindow();

private slots:
    void  doPipeInputRq( QString text);
    void  doPipeInputPv( QString text);
    void  doLineInputRq();
    void  doLineInputPv();
    void  on_clearButton_clicked();
    void  readSettings();
    void  writeSettings();
    void  appendText( QString text);

protected:
    virtual void  keyPressEvent( QKeyEvent* ev);
    virtual void  closeEvent( QCloseEvent* event);

private:
    struct History {
        QStringList  list;
        int  cur;

        History();
        void  addEntry( const QString& text);
    };

    History*  selHistory( const QWidget* widget);

    Ui::TermWindow*  _ui;
    ArnItem  _pipeRq;
    ArnItem  _pipePv;
    History  _historyRq;
    History  _historyPv;
    QString  _lastFind;
    QSettings*  _appSettings;
    int _nrRows { 0 };
    int _maxRows { 0 };
};

#endif // TERMWINDOW_HPP
