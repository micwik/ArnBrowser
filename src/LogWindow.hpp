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

#ifndef LOGWINDOW_HPP
#define LOGWINDOW_HPP

#include "Connector.hpp"
#include <ArnInc/ArnItem.hpp>
#include <QDialog>
#include <QFile>

class QSettings;
class QCloseEvent;

namespace Ui {
    class LogWindow;
}


class LogWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LogWindow( QSettings* appSettings, const ConnectorPath& conPath, QWidget *parent = 0);
    ~LogWindow();

private slots:
    void  doPipeInputRq( QString text);
    void  doPipeInputPv( QString text);
    void  on_startLogButton_clicked();
    void  on_browseButton_clicked();

private:
    void  doWriteToLog( const QString& text);
    void  closeEvent(QCloseEvent *event);
    void  readSettings();
    void  writeSettings();

    ArnItem  _pipeRq;
    ArnItem  _pipePv;
    QString  _fileDir;
    QFile  _logFile;
    int  _logLines;
    QSettings*  _appSettings;
    Ui::LogWindow *_ui;
};

#endif // LOGWINDOW_HPP
