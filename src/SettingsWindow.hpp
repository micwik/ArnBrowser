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

#ifndef SETTINGSWINDOW_HPP
#define SETTINGSWINDOW_HPP

#include <QDialog>

class SettingsHandler;
class QSettings;
class QCloseEvent;

namespace Ui {
class SettingsWindow;
}


class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow( SettingsHandler* settings, QWidget *parent = 0);
    ~SettingsWindow();

    virtual void  accept();
    virtual void  reject();

protected:
    void  closeEvent( QCloseEvent *event);

private slots:
    void  on_generateButton_clicked();

private:
    void  readSettings();
    void  writeSettings();

    Ui::SettingsWindow *_ui;
    SettingsHandler*  _settings;
    QSettings*  _appSettings;
};

#endif // SETTINGSWINDOW_HPP
