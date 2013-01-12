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

#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "TermWindow.hpp"
#include "CodeWindow.hpp"
#include "MultiDelegate.hpp"
#include <ArnLib/ArnClient.hpp>
#include <ArnLib/ArnLink.hpp>
#include <QMessageBox>
#include <QImage>
#include <QSettings>
#include <QCloseEvent>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui( new Ui::MainWindow)
{
    _ui->setupUi( this);

    _appSettings = new QSettings("MicTech", "ArnBrowser");

    //// Error log from Arn system
    Arn::setConsoleError( false);
    connect( &Arn::getInstance(), SIGNAL(errorLogSig(QString,uint,void*)),
             this, SLOT(errorLog(QString)));

    //// Prepare connect to Arn server
    _arnClient = new ArnClient( this);
    connect( _arnClient, SIGNAL(tcpConnected()), this, SLOT(clientConnected()));
    connect( _arnClient, SIGNAL(tcpError(QString,QAbstractSocket::SocketError)),
             this, SLOT(clientError(QString)));

    /// Setup model
    _arnModel = new ArnModel( this);
    _arnModel->setClient( _arnClient);

    _ui->arnView->setEnabled( false);

    readSettings();
    QString  host = _appSettings->value("connect/host").toString();
    int      port = _appSettings->value("connect/port").toInt();
    if (host.isNull())
        host = "localhost";  // Default host
    if (port == 0)
        port = 2022;
    _ui->hostEdit->setText( host);    // Default host
    _ui->portEdit->setValue( port);

    connect( _ui->arnView, SIGNAL(clicked(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));

    _ui->terminalButton->setEnabled( false);
    _ui->editButton->setEnabled( false);
}


MainWindow::~MainWindow()
{
    delete _ui;
}


void  MainWindow::on_connectButton_clicked()
{
    _arnClient->connectToArn( _ui->hostEdit->text(), _ui->portEdit->value());
    _ui->hostEdit->setEnabled( false);
    _ui->portEdit->setEnabled( false);
}


void  MainWindow::on_terminalButton_clicked()
{
    TermWindow*  termWindow = new TermWindow( _appSettings, _curItemPath, 0);
    termWindow->show();
}


void  MainWindow::on_editButton_clicked()
{
    CodeWindow*  codeWindow = new CodeWindow( _appSettings, _curItemPath, 0);
    codeWindow->show();
}


void  MainWindow::on_viewHidden_clicked()
{
    updateHiddenTree( _ui->arnView->rootIndex());
}


void  MainWindow::on_hideBidir_clicked()
{
    _arnModel->setHideBidir( _ui->hideBidir->isChecked());
    updateHiddenTree( _ui->arnView->rootIndex());
}


void  MainWindow::updateHiddenTree( const QModelIndex& index)
{
    _arnModel->data( index, ArnModel::Role::Hidden);  // Hidden row is updated by signal

    int  rows = _arnModel->rowCount( index);
    for (int i = 0; i < rows; ++i) {
        updateHiddenTree( _arnModel->index(i, 0, index));
    }
}


void  MainWindow::itemClicked( const QModelIndex& index)
{
    _curItemPath = _arnModel->data( index, ArnModel::Role::Path).toString();
    //qDebug() << "ItemClicked: path=" << _curItemPath;
    ArnItem  arnItem( _curItemPath);
    ArnLink::Type  type = arnItem.type();

    // Set state for Terminal button
    _ui->terminalButton->setEnabled( arnItem.isPipeMode());

    // Set state for Edit button
    bool editEn = !arnItem.isFolder() && ((type.e == type.Null)
                                      ||  (type.e == type.ByteArray)
                                      ||  (type.e == type.String));
    _ui->editButton->setEnabled( editEn);
}


void  MainWindow::clientConnected()
{
    _arnClient->setAutoConnect( true, 2);
    disconnect( _arnClient, SIGNAL(tcpError(QString,QAbstractSocket::SocketError)),
             this, SLOT(clientError(QString)));

    _ui->connectButton->setEnabled( false);
    _ui->arnView->setModel( _arnModel);
    _arnModel->setHideBidir( _ui->hideBidir->isChecked());
    connect( _arnModel, SIGNAL(hiddenRow(int,QModelIndex,bool)),
             this, SLOT(updateHidden(int,QModelIndex,bool)));
    _delegate = new MultiDelegate;
    _ui->arnView->setItemDelegate( _delegate);
    _ui->arnView->setEnabled( true);
    _ui->arnView->setColumnWidth(0, _pathWidth);
    _ui->arnView->setColumnWidth(1, 10);

    _appSettings->setValue("connect/host", _ui->hostEdit->text());
    _appSettings->setValue("connect/port", _ui->portEdit->value());

/*
    // Test
    ArnItem arnTestValue("/test/value");
    arnTestValue = "Testing ...";
    QImage  img;
    img.load("hus.png");
    //img.load("vattenmat.ppm");
    ArnItem arnImg("/test/husImg");
    QVariant valImg = QVariant( img);
    arnImg = valImg;
    qDebug() << "Set Test Image: type=" << arnImg.toVariant().typeName();
*/
}


void  MainWindow::clientError( QString errorText)
{
    // cout << "Client TCP error: " << errorText << endl;
    QMessageBox msgBox;
    msgBox.setWindowTitle( tr("Error"));
    msgBox.setIcon( QMessageBox::Information);
    msgBox.setText( tr("Can not Connect to Host"));
    msgBox.setInformativeText( errorText);
    msgBox.exec();
    _ui->hostEdit->setEnabled( true);
    _ui->portEdit->setEnabled( true);
}


void  MainWindow::updateHidden(int row, QModelIndex parent, bool isHidden)
{
    bool  hide = isHidden && !_ui->viewHidden->isChecked();
    if (_ui->arnView->isRowHidden( row, parent) != hide) {
        _ui->arnView->setRowHidden( row, parent, hide);
        // qDebug() << "UpdateHidden: row=" << row << "hide=" << hide;
    }
}


void  MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        _ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


void  MainWindow::closeEvent( QCloseEvent* event)
{
    qDebug() << "Close event";
    writeSettings();
    event->accept();
}


void  MainWindow::errorLog( QString errText)
{
    qDebug() << "MW-Err:" << errText;
}


void  MainWindow::readSettings()
{
    QPoint  pos = _appSettings->value("main/pos", QPoint(200, 200)).toPoint();
    QSize  size = _appSettings->value("main/size", QSize(400, 400)).toSize();
    _pathWidth = _appSettings->value("main/pathW", 50).toInt();
    bool  viewHidden = _appSettings->value("main/viewHidden", false).toBool();
    bool  hideBidir  = _appSettings->value("main/hideBidir",  false).toBool();
    resize( size);
    move( pos);
    _ui->viewHidden->setChecked( viewHidden);
    _ui->hideBidir->setChecked( hideBidir);
}


void  MainWindow::writeSettings()
{
    qDebug() << "Write main settings";
    _appSettings->setValue("main/pos", pos());
    _appSettings->setValue("main/size", size());
    if (_ui->arnView->isEnabled())
        _appSettings->setValue("main/pathW", _ui->arnView->columnWidth(0));
    _appSettings->setValue("main/viewHidden", _ui->viewHidden->isChecked());
    _appSettings->setValue("main/hideBidir",  _ui->hideBidir->isChecked());
}

