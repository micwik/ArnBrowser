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
