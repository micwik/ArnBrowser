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

#include "DiscoverWindow.hpp"
#include "ui_DiscoverWindow.h"
#include <QInputDialog>
#include <QCloseEvent>
#include <QSettings>
#include <QDebug>

using Arn::XStringMap;


DiscoverWindow::DiscoverWindow( QSettings* appSettings, QWidget* parent) :
    QDialog( parent),
    _ui( new Ui::DiscoverWindow)
{
    _ui->setupUi( this);
    _ui->connectButton->setEnabled( false);

    this->setWindowTitle( QString("Discover ARN"));

    _appSettings = appSettings;
    readSettings();
    _filterGroup = _ui->groupEdit->text();

    _serviceBrowser = new ArnDiscoverBrowser( this);
    _serviceBrowser->setDefaultStopState( ArnDiscoverInfo::State::HostInfo);
    //connect(_serviceBrowser, SIGNAL(browseError(int)),
    //        this, SLOT(onBrowseError(int)));
    connect(_serviceBrowser, SIGNAL(serviceAdded(int,QString)),
            this, SLOT(onServiceAdded(int,QString)));
    connect(_serviceBrowser, SIGNAL(serviceRemoved(int)), this, SLOT(onServiceRemoved(int)));
    connect(_serviceBrowser, SIGNAL(infoUpdated(int,ArnDiscoverInfo::State)),
            this, SLOT(onInfoUpdated(int,ArnDiscoverInfo::State)));

    //// Logics

    connect( _ui->groupEdit, SIGNAL(returnPressed()), this, SLOT(onFilterGroupChanged()));
    connect( _ui->groupEdit, SIGNAL(editingFinished()), this, SLOT(onFilterGroupLeave()));
    connect( _ui->filterGroupButton, SIGNAL(clicked()), this, SLOT(onFilterChanged()));
    connect( _ui->filterServerButton, SIGNAL(clicked()), this, SLOT(onFilterChanged()));
    connect( _ui->filterClientButton, SIGNAL(clicked()), this, SLOT(onFilterChanged()));
    connect( _ui->filterAllButton, SIGNAL(clicked()), this, SLOT(onFilterChanged()));
    connect( _ui->serviceTabView, SIGNAL(itemSelectionChanged()), this, SLOT(onServiceSelectChanged()));
    connect( _ui->serviceTabView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_connectButton_clicked()));
    connect( _ui->cancelButton, SIGNAL(clicked()), this, SLOT(close()));

    updateBrowse();
}


DiscoverWindow::~DiscoverWindow()
{
    delete _ui;
}


void  DiscoverWindow::getResult( QString& hostName, quint16& hostPort, QString &hostAddr)
{
    hostName = _ui->hostNameValue->text();
    hostPort = _ui->hostPortValue->text().toUInt();
    hostAddr = _ui->hostIpValue->text();
}


void  DiscoverWindow::updateBrowse()
{
    if (_serviceBrowser->isBrowsing()) {
        _serviceBrowser->stopBrowse();
        _ui->serviceTabView->clear();
        updateInfoView(-1);
    }

    bool  isFilterGroup = _ui->filterGroupButton->isChecked();
    _ui->groupEdit->setEnabled( isFilterGroup);
    _ui->groupText->setEnabled( isFilterGroup);

    if (isFilterGroup) {
        if (_ui->groupEdit->text().isEmpty())
            return;  // Don't browse using empty group filter
        _serviceBrowser->setFilter( _ui->groupEdit->text());
    }
    else {
        ArnDiscover::Type  typeFilter;  // Default no filter
        if (_ui->filterServerButton->isChecked())
            typeFilter = typeFilter.Server;
        else if (_ui->filterClientButton->isChecked())
            typeFilter = typeFilter.Client;
        _serviceBrowser->setFilter( typeFilter);
    }

    _serviceBrowser->browse();
}


void  DiscoverWindow::onBrowseError( int code)
{
    qWarning() << "Browse Error code=" << code;
}


void  DiscoverWindow::onServiceAdded( int index, QString name)
{
    _ui->serviceTabView->insertItem( index, name);
}


void  DiscoverWindow::onServiceRemoved( int index)
{
    QListWidgetItem*  item =_ui->serviceTabView->takeItem( index);
    if (item)
        delete item;
    updateInfoView(-1);
}


void  DiscoverWindow::onInfoUpdated( int index, ArnDiscoverInfo::State state)
{
    Q_UNUSED(state);

    updateInfoView( index);
}


void  DiscoverWindow::updateInfoView( int index)
{
    int  curIndex = _ui->serviceTabView->currentRow();
    // qDebug() << "updateInfoView: curRow=" << curIndex << " index=" << index;
    if (index != curIndex)  return;

    bool  isOk = (curIndex >= 0);
    ArnDiscoverInfo  info = isOk ? _serviceBrowser->infoByIndex( curIndex) : ArnDiscoverInfo();
    XStringMap  xsmTxt = info.properties();

    _ui->connectButton->setEnabled( isOk);

    _ui->discoverTypeValue->setText( info.typeString());
    _ui->hostNameValue->setText( info.hostName());
    _ui->hostPortValue->setText( info.hostPortString());
    _ui->hostIpValue->setText( info.hostIpString());

    _ui->propertyTabView->clear();
    for (int i = 0; i < xsmTxt.size(); ++i) {
        QString  propLine = xsmTxt.keyString(i) + " = " + xsmTxt.valueString(i);
        _ui->propertyTabView->addItem( propLine);
    }
}


void DiscoverWindow::on_connectButton_clicked()
{
    if (!_ui->connectButton->isEnabled())  return;  // Button must be enabled to connect

    setResult( QDialog::Accepted);
    close();
}


void  DiscoverWindow::onFilterGroupChanged()
{
    _filterGroup = _ui->groupEdit->text();
    updateBrowse();
}


void  DiscoverWindow::onFilterGroupLeave()
{
    _ui->groupEdit->setText( _filterGroup);  // Restore and skip any unfinnished editing
}


void  DiscoverWindow::onFilterChanged()
{
    updateBrowse();
}


void  DiscoverWindow::onServiceSelectChanged()
{
    // qDebug() << "onServiceSelectChanged";
    int  index = _ui->serviceTabView->currentRow();
    _serviceBrowser->goTowardState( index, ArnDiscoverInfo::State::HostIp);
    updateInfoView( index);
}


void  DiscoverWindow::closeEvent( QCloseEvent* event)
{
    qDebug() << "Close event";
    writeSettings();
    event->accept();
    deleteLater();
}


void  DiscoverWindow::readSettings()
{
    QPoint   pos   = _appSettings->value("discover/pos", QPoint(200, 200)).toPoint();
    QSize    size  = _appSettings->value("discover/size", QSize(500, 400)).toSize();
    QString  group = _appSettings->value("discover/group", QString()).toString();
    resize( size);
    move( pos);
    _ui->groupEdit->setText( group);
}


void  DiscoverWindow::writeSettings()
{
    qDebug() << "Write code settings";
    _appSettings->setValue("discover/pos", pos());
    _appSettings->setValue("discover/size", size());
    _appSettings->setValue("discover/group", _ui->groupEdit->text());
}
