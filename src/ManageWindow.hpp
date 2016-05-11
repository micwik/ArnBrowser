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

#ifndef MANAGEWINDOW_HPP
#define MANAGEWINDOW_HPP

#include "Connector.hpp"
#include <ArnInc/ArnPersistSapi.hpp>
#include <ArnInc/MQFlags.hpp>
#include <ArnInc/ArnItem.hpp>
#include <QTimer>
#include <QStringList>
#include <QDialog>

class QSettings;
class QCloseEvent;

namespace Ui {
class ManageWindow;
}


class ArnItemUnidir : public ArnItem
{
public:
    explicit ArnItemUnidir( QObject *parent = 0) : ArnItem( parent)
    {
        setForceKeep();
    }
};


class ManageWindow : public QDialog
{
    Q_OBJECT    
public:
    explicit ManageWindow( QSettings* appSettings, const ConnectorPath& conPath,  QWidget* parent = 0);
    ~ManageWindow();
    
private slots:
    void  on_itemEdit_textChanged( const QString& txt);
    void  on_newButton_clicked();
    void  on_deleteButton_clicked();
    void  on_saveButton_clicked();
    void  on_cancelButton_clicked();
    void  on_resetButton_clicked();
    void  on_folderButton_clicked();

    void  readSettings();
    void  writeSettings();

    void  lsR( QStringList files);
    void  mandatoryLsR( QStringList files);
    void  doTypeUpdate();
    void  doUpdate();
    void  timeoutReset();
    void  timeoutSave();

protected:
    void closeEvent( QCloseEvent *event);

private:
    void  doPersistUpdate();

    ArnPersistSapi  _persistSapi;
    QByteArray  _storeValue;
    ArnItemUnidir  _arnPath;
    ConnectorPath  _conPath;
    bool  _isNewMode;
    bool  _isPersistFile;
    bool  _isMandatory;

    Ui::ManageWindow* _ui;
    QSettings*  _appSettings;
};

#endif // MANAGEWINDOW_HPP
