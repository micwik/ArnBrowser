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

#ifndef CODEWINDOW_HPP
#define CODEWINDOW_HPP

#include <ArnInc/Arn.hpp>
#include <QDialog>

class QSettings;
class QCloseEvent;

namespace Ui {
    class CodeWindow;
}


class CodeWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CodeWindow( QSettings* appSettings, const QString& path, QWidget* parent = 0);
    ~CodeWindow();

    void  loadFile( const QString& fileName);
    void  setText( const QString& text);
    void  saveFile( const QString& fileName);
    QString  getText()  const;

private slots:
    void  on_loadFileButton_clicked();
    void  on_saveFileButton_clicked();
    void  on_reLoadButton_clicked();
    void  on_saveButton_clicked();
    void  on_findButton_clicked();
    void  on_findNextButton_clicked();
    void  readSettings();
    void  writeSettings();

protected:
    void closeEvent( QCloseEvent *event);

private:
    Ui::CodeWindow*  _ui;    
    ArnItem  _arnItem;
    QSettings*  _appSettings;
};

#endif // CODEWINDOW_HPP
