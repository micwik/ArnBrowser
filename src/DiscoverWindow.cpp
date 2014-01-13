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
#include <ArnInc/ArnDiscover.hpp>
#include <ArnInc/ArnZeroConf.hpp>
#include <QInputDialog>
#include <QCloseEvent>
#include <QSettings>
#include <QDebug>


DiscoverWindow::DiscoverWindow( QSettings* appSettings, QWidget* parent) :
    QDialog( parent),
    _ui( new Ui::DiscoverWindow)
{
    _ui->setupUi( this);
    _ui->connectButton->setEnabled( false);

    this->setWindowTitle( QString("Discover ARN"));

    _appSettings = appSettings;
    readSettings();

    qDebug() << "Start listen !!!";
    _serviceBrowser = new ArnZeroConfBrowser( this);
    connect(_serviceBrowser, SIGNAL(browseError(int)),
            this, SLOT(onBrowseError(int)));
    connect(_serviceBrowser, SIGNAL(serviceAdded(int,QString,QString)),
            this, SLOT(onServiceAdded(int,QString,QString)));
    connect(_serviceBrowser, SIGNAL(serviceRemoved(int,QString,QString)),
            this, SLOT(onServiceRemoved(int,QString,QString)));

    //// Logics

    connect( _ui->typeServerButton, SIGNAL(clicked()), this, SLOT(onDiscoverTypeChanged()));
    connect( _ui->typeClientButton, SIGNAL(clicked()), this, SLOT(onDiscoverTypeChanged()));
    connect( _ui->typeAllButton, SIGNAL(clicked()), this, SLOT(onDiscoverTypeChanged()));
    connect( _ui->serviceTabView, SIGNAL(itemSelectionChanged()), this, SLOT(onServiceSelectChanged()));
    connect( _ui->cancelButton, SIGNAL(clicked()), this, SLOT(close()));

    updateBrowse();
}


DiscoverWindow::~DiscoverWindow()
{
    delete _ui;
}


void  DiscoverWindow::getResult( QString& hostAdr, quint16& hostPort)
{
    hostAdr  = _ui->hostAdrValue->text();
    hostPort = _ui->hostPortValue->text().toUInt();
}


void  DiscoverWindow::updateBrowse()
{
    if (_serviceBrowser->isBrowsing()) {
        _serviceBrowser->stopBrowse();
        _ui->serviceTabView->clear();

        _activeServNames.clear();
        _activeServIds.clear();
        _activeServInfos.clear();
        _ipLookupIds.clear();

        updateInfoView(-1);
    }

    QString  subType;  // Default no subtype
    if (_ui->typeServerButton->isChecked())
        subType = "server";
    else if (_ui->typeClientButton->isChecked())
        subType = "client";
    _serviceBrowser->setSubType( subType);

    _serviceBrowser->browse();
    qDebug() << "Browse in progress ...";
}


void DiscoverWindow::onBrowseError(int code)
{
    qDebug() << "Browse Error code=" << code;
}


void  DiscoverWindow::onServiceAdded( int id, QString name, QString domain)
{
    qDebug() << "Browse Service added: name=" << name << " domain=" << domain
             << " escFullDomain=" << _serviceBrowser->escapedFullDomain();

    XStringMap  xsm;
    xsm.add("sbDomain", domain);
    QByteArray  info = xsm.toXString();
    int  index;
    for (index = 0; index < _activeServNames.size(); ++index) {
        QString  indexName = _activeServNames.at( index);
        Q_ASSERT(name != indexName);
        if (name < indexName)  break;  // Sorting place found
    }
    _activeServNames.insert( index, name);
    _activeServIds.insert( index, id);
    _activeServInfos.insert( index, info);

    ArnZeroConfResolv*  ds = new ArnZeroConfResolv( name, this);
    ds->setId( id);
    connect( ds, SIGNAL(resolveError(int)), this, SLOT(onResolveError(int)));
    connect( ds, SIGNAL(resolved(int,QByteArray)), this, SLOT(onResolved(int,QByteArray)));
    ds->resolve();

    _ui->serviceTabView->insertItem( index, name);
}


void  DiscoverWindow::onServiceRemoved( int id, QString name, QString domain)
{
    qDebug() << "Browse Service removed: name=" << name << " domain=" << domain;
    int  index = _activeServNames.indexOf( name);
    _activeServNames.removeAt( index);
    _activeServIds.removeAt( index);
    _activeServInfos.removeAt( index);

    QListWidgetItem*  item =_ui->serviceTabView->takeItem( index);
    if (item)
        delete item;
    updateInfoView(-1);
}


void  DiscoverWindow::onResolveError( int code)
{
    ArnZeroConfResolv*  ds = qobject_cast<ArnZeroConfResolv*>( sender());
    Q_ASSERT(ds);

    qDebug() << "Resolve Error code=" << code;

    ds->releaseService();
    ds->deleteLater();
}


void  DiscoverWindow::onResolved( int id, QByteArray escFullDomain)
{
    ArnZeroConfResolv*  ds = qobject_cast<ArnZeroConfResolv*>( sender());
    Q_ASSERT(ds);

    QString  name = ds->serviceName();
    qDebug() << "Resolved Service: name=" << name << " escFullDomainR=" << escFullDomain
             << " escFullDomain=" << ds->escapedFullDomain();
    int  index = _activeServNames.indexOf( name);
    if (index >= 0) {  // Service still exist
        XStringMap  xsm( _activeServInfos.at( index));
        XStringMap  xsmTxt;
        ds->getTxtRecordMap( xsmTxt);
        xsm.add("rFullDomain", escFullDomain.constData());
        xsm.add("rServ",       ds->serviceType());
        xsm.add("rDomain",     ds->domain());
        xsm.add("rHost",       ds->host());
        xsm.add("rPort",       QString::number( ds->port()));
        xsm.add("rTxt",        xsmTxt.toXString());
        QByteArray  info = xsm.toXString();
        _activeServInfos[ index] = info;

        int  ipLookupId = QHostInfo::lookupHost( ds->host(), this, SLOT(onIpLookup(QHostInfo)));
        _ipLookupIds.insert( ipLookupId, id);
        qDebug() << "LookingUp host=" << ds->host() << " lookupId=" << ipLookupId;

        updateInfoView( index);
    }

    ds->releaseService();
    ds->deleteLater();
}


void  DiscoverWindow::onIpLookup( const QHostInfo& host)
{
    int  ipLookupId = host.lookupId();
    int  id = _ipLookupIds.value( ipLookupId, -1);
    qDebug() << "onIpLookup: lookupId=" << ipLookupId;
    if (id < 0)  return;  // Service not valid anymore

    _ipLookupIds.remove( ipLookupId);

    if (host.error() != QHostInfo::NoError) {
         qDebug() << "Lookup failed:" << host.errorString();
         return;
    }

    foreach (const QHostAddress &address, host.addresses())
        qDebug() << "Found address:" << address.toString();

    int  index = _activeServIds.indexOf( id);
    XStringMap  xsm( _activeServInfos.at( index));
    xsm.add("lIp", host.addresses().first().toString());
    _activeServInfos[ index] = xsm.toXString();

    updateInfoView( index);
}


void DiscoverWindow::updateServiceView()
{
    QString info;
    _ui->serviceTabView->clear();

    for (int i = 0; i < _activeServNames.size(); ++i) {
        _ui->serviceTabView->addItem( _activeServNames.at(i));
    }
}


void  DiscoverWindow::updateInfoView( int index)
{
    int  curIndex = _ui->serviceTabView->currentRow();
    qDebug() << "updateInfoView: curRow=" << curIndex << " index=" << index;
    if (index != curIndex)  return;

    bool  isOk = (curIndex >= 0);
    XStringMap  xsm( isOk ? _activeServInfos.at( curIndex) : QByteArray());
    XStringMap  xsmTxt( xsm.value("rTxt"));

    _ui->connectButton->setEnabled( isOk);

    int  arnType = xsmTxt.value("server", "-1").toInt();
    _ui->discoverTypeValue->setText( arnType < 0 ? "" : (arnType ? "Server" : "Client"));
    _ui->hostAdrValue->setText( xsm.valueString("rHost"));
    _ui->hostPortValue->setText( xsm.valueString("rPort"));
    _ui->hostIpValue->setText( xsm.valueString("lIp"));

    _ui->propertyTabView->clear();
    for (int i = 0; i < xsmTxt.size(); ++i) {
        QString  propLine = xsmTxt.keyString(i) + " = " + xsmTxt.valueString(i);
        _ui->propertyTabView->addItem( propLine);
    }
}


void DiscoverWindow::on_connectButton_clicked()
{
    setResult( QDialog::Accepted);
    close();
}


void  DiscoverWindow::onDiscoverTypeChanged()
{
    updateBrowse();
}


void  DiscoverWindow::onServiceSelectChanged()
{
    qDebug() << "onServiceSelectChanged";
    updateInfoView( _ui->serviceTabView->currentRow());
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
    QPoint      pos   = _appSettings->value("discover/pos", QPoint(200, 200)).toPoint();
    QSize       size  = _appSettings->value("discover/size", QSize(500, 400)).toSize();
    // QByteArray  split = _appSettings->value("discover/split", _ui->splitter->saveState()).toByteArray();
    resize( size);
    move( pos);
    //_ui->splitter->restoreState( split);
}


void  DiscoverWindow::writeSettings()
{
    qDebug() << "Write code settings";
    _appSettings->setValue("discover/pos", pos());
    _appSettings->setValue("discover/size", size());
    //_appSettings->setValue("vcs/split", _ui->splitter->saveState());
}
