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

#ifndef DISCOVERWINDOW_HPP
#define DISCOVERWINDOW_HPP

#include <ArnInc/MQFlags.hpp>
#include <ArnInc/Arn.hpp>
#include <ArnInc/ArnDiscover.hpp>
#include <QHostInfo>
#include <QStringList>
#include <QDialog>

class QSettings;
class QCloseEvent;

namespace Ui {
class DiscoverWindow;
}


class DiscoverWindow : public QDialog
{
    Q_OBJECT    
public:
    explicit DiscoverWindow( QSettings* appSettings, QWidget* parent = 0);
    ~DiscoverWindow();

    void  getResult( QString& hostName, quint16& hostPort, QString& hostAddr);
    
protected:
    void  closeEvent( QCloseEvent *event);

private slots:
    void  on_connectButton_clicked();
    void  onFilterChanged();
    void  onFilterGroupChanged();
    void  onFilterGroupLeave();
    void  onServiceSelectChanged();

    void  onBrowseError( int code);
    void  onServiceAdded( int index, QString name);
    void  onServiceRemoved( int index);
    void  onInfoUpdated( int index, ArnDiscoverInfo::State state);

    void  readSettings();
    void  writeSettings();

private:
    void  updateBrowse();
    void  updateCurService();
    void  updateInfoView( int index);

    Ui::DiscoverWindow*  _ui;
    QSettings*  _appSettings;
    ArnDiscoverBrowser*  _serviceBrowser;
    QString  _filterGroup;
};

#endif // DISCOVERWINDOW_HPP
