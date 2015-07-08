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

#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "TermWindow.hpp"
#include "LogWindow.hpp"
#include "CodeWindow.hpp"
#include "ManageWindow.hpp"
#include "VcsWindow.hpp"
#include "DiscoverWindow.hpp"
#include "MultiDelegate.hpp"
#include <ArnInc/ArnClient.hpp>
#include <QMessageBox>
#include <QSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QSettings>
#include <QCloseEvent>
#include <QDebug>
#ifdef QMLRUN
  #include "QmlRunWindow.hpp"
  #include <ArnInc/ArnQml.hpp>
#endif

extern const QString ver;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui( new Ui::MainWindow)
{
    _ui->setupUi( this);

    _isConnect = false;

    QLabel*  curItemPathLabel = new QLabel;
    curItemPathLabel->setText("Path:");
    _curItemPathStatus = new QLineEdit;
    _curItemPathStatus->setReadOnly( true);
    _curItemPathStatus->setStyleSheet("QLineEdit{background: lightgrey;}");
    _ui->statusBar->addPermanentWidget( curItemPathLabel);
    _ui->statusBar->addPermanentWidget( _curItemPathStatus);

    _appSettings = new QSettings("MicTron", "ArnBrowser");

    //// Error log from Arn system
    ArnM::setConsoleError( false);
    connect( &ArnM::instance(), SIGNAL(errorLogSig(QString,uint,void*)),
             this, SLOT(errorLog(QString)));

    //// Prepare connect to Arn server
    _connector = new Connector( this);
    _arnClient = new ArnClient( this);
    // _arnClient->setReceiveTimeout(5);  // Base time for receiver timeout
    connect( _arnClient, SIGNAL(tcpConnected(QString,quint16)), this, SLOT(clientConnected()));
    connect( _arnClient, SIGNAL(connectionStatusChanged(int,int)), this, SLOT(doClientStateChanged(int)));
    connect( _arnClient, SIGNAL(tcpError(QString,QAbstractSocket::SocketError)),
             this, SLOT(clientError(QString)));

    //// Setup model
    _arnModel = new ArnModel( _connector, this);
    _arnModel->setClient( _arnClient);
    connect( _arnModel, SIGNAL(hiddenRow(int,QModelIndex,bool)),
             this, SLOT(updateHidden(int,QModelIndex,bool)));

    setWindowTitle(tr("Arn Browser  ") + ver);
    _ui->arnView->setEnabled( false);

    readSettings();
    QString  host = _appSettings->value("connect/host").toString();
    int      port = _appSettings->value("connect/port").toInt();
    if (host.isNull())
        host = "localhost";  // Default host
    if (port == 0)
        port = Arn::defaultTcpPort;
    _ui->hostEdit->setText( host);    // Default host
    _ui->portEdit->setValue( port);
    _ui->connectStat->hide();

#if QT_VERSION >= 0x050000 && QT_VERSION < 0x050300
    _ui->portEdit->setRange( 1, 1065535);  // Fix early Qt5 layout bug giving to little space
#endif

    connect( _ui->arnView, SIGNAL(clicked(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));

    setConnectOffGui();
}


MainWindow::~MainWindow()
{
    // qDebug() << "MainWindow: Destructor";
    delete _ui;
}


void  MainWindow::on_connectButton_clicked()
{
    bool isConnect = _ui->connectButton->isChecked();
    connection( isConnect);
}


void  MainWindow::connection( bool isConnect)
{
    _isConnect = isConnect;
    _ui->connectButton->setChecked( isConnect);

    _ui->connectButton->setEnabled( false);
    if (isConnect) {
        _arnClient->connectToArn( _ui->hostEdit->text(), _ui->portEdit->value());
        _connector->setCurHost( _ui->hostEdit->text());
        _ui->discoverButton->setEnabled( false);
        _ui->hostEdit->setEnabled( false);
        _ui->portEdit->setEnabled( false);
    }
    else {
        _arnModel->stop();
        _arnClient->close();
    }
}


void MainWindow::setConnectOffGui()
{
    _ui->terminalButton->setEnabled( false);
    _ui->logButton->setEnabled( false);
    _ui->editButton->setEnabled( false);
    _ui->runButton->setEnabled( false);
    _ui->manageButton->setEnabled( false);
    _ui->vcsButton->setEnabled( false);

    _ui->connectButton->setEnabled( true);
    _ui->discoverButton->setEnabled( true);

    _ui->hostEdit->setEnabled( true);
    _ui->portEdit->setEnabled( true);
}


void  MainWindow::on_discoverButton_clicked()
{
    DiscoverWindow*  discoverWindow = new DiscoverWindow( _appSettings, 0);
    discoverWindow->exec();
    if (discoverWindow->result() != QDialog::Accepted)  return;

    QString  hostName;
    quint16  hostPort;
    QString  hostAddr;
    discoverWindow->getResult( hostName, hostPort, hostAddr);
    if (!hostAddr.isEmpty())
        hostName = Arn::makeHostWithInfo( hostAddr, hostName);
    _ui->hostEdit->setText( hostName);
    _ui->portEdit->setValue( hostPort);

    connection( true);
}


void  MainWindow::on_terminalButton_clicked()
{
    ConnectorPath  conPath( _connector, _curItemPath);
    TermWindow*  termWindow = new TermWindow( _appSettings, conPath, 0);
    termWindow->show();
}


void MainWindow::on_logButton_clicked()
{
    ConnectorPath  conPath( _connector, _curItemPath);
    LogWindow*  logWindow = new LogWindow( _appSettings, conPath, 0);
    logWindow->show();
}


void  MainWindow::on_editButton_clicked()
{
    ConnectorPath  conPath( _connector, _curItemPath);
    CodeWindow*  codeWindow = new CodeWindow( _appSettings, conPath, 0);
    codeWindow->show();
}


void  MainWindow::on_runButton_clicked()
{
#ifdef QMLRUN
    ConnectorPath  conPath( _connector, _curItemPath);
    QmlRunWindow*  qmlRunWindow = new QmlRunWindow( _appSettings, conPath, 0);
    qmlRunWindow->show();
#endif
}


void  MainWindow::on_manageButton_clicked()
{
    ConnectorPath  conPath( _connector, _curItemPath);
    ManageWindow*  manageWindow = new ManageWindow( _appSettings, conPath, 0);
    manageWindow->show();
}


void  MainWindow::on_vcsButton_clicked()
{
    VcsWindow*  vcsWindow = new VcsWindow( _appSettings, _connector, 0);
    vcsWindow->show();
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
    _curItemPathStatus->setText( _connector->toNormPath( _curItemPath));
    ArnItem  arnItem( _curItemPath);
    Arn::DataType  type = arnItem.type();

    // Set state for Terminal & Log button
    _ui->terminalButton->setEnabled( arnItem.isPipeMode());
    _ui->logButton->setEnabled( arnItem.isPipeMode());

    // Set state for Edit button
    bool  editEn = !arnItem.isFolder() && ((type == type.Null)
                                       ||  (type == type.ByteArray)
                                       ||  (type == type.String));
    _ui->editButton->setEnabled( editEn);

    // Set state for Run button
#ifdef QMLRUN
    bool  runEn = !arnItem.isFolder() && arnItem.name( Arn::NameF()).endsWith(".qml");
    _ui->runButton->setEnabled( runEn);
#endif

    _ui->manageButton->setEnabled( true);
}


void  MainWindow::clientConnected()
{
    _arnClient->setAutoConnect( true, 2);
    disconnect( _arnClient, SIGNAL(tcpError(QString,QAbstractSocket::SocketError)),
             this, SLOT(clientError(QString)));

    _arnModel->start();
    if (_ui->arnView->model()) // model already set, just reset viewer
        _ui->arnView->reset();
    else    // model has not been set, do it now
        _ui->arnView->setModel( _arnModel);
    _arnModel->setHideBidir( _ui->hideBidir->isChecked());
    _ui->vcsButton->setEnabled( true);
    _ui->connectButton->setEnabled( true);

    _delegate = qobject_cast<MultiDelegate*>( _ui->arnView->itemDelegate());
    if (!_delegate)
        _delegate = new MultiDelegate;
    _ui->arnView->setItemDelegate( _delegate);
    _ui->arnView->setEnabled( true);
    _ui->arnView->setColumnWidth(0, _pathWidth);
    _ui->arnView->setColumnWidth(1, 10);

    _curItemPath = "/";
    _ui->manageButton->setEnabled( true);

    _appSettings->setValue("connect/host", _ui->hostEdit->text());
    _appSettings->setValue("connect/port", _ui->portEdit->value());
}


void MainWindow::doClientStateChanged(int status)
{
    // qDebug() << "ClientStateChanged: state=" << status;
    _ui->connectStat->setChecked( status == ArnClient::ConnectStat::Connected);
    _ui->connectStat->setVisible( _isConnect);

    if ((status == ArnClient::ConnectStat::Disconnected) && !_isConnect) {
        //// Manual disconnection from user
        setConnectOffGui();
    }
}


void  MainWindow::clientError( QString errorText)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle( tr("Error"));
    msgBox.setIcon( QMessageBox::Information);
    msgBox.setText( tr("Can not Connect to Host"));
    msgBox.setInformativeText( errorText);
    msgBox.exec();
    _ui->connectButton->setEnabled( true);
    _ui->discoverButton->setEnabled( true);
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
    qDebug() << "MainWindow: Close event";
    writeSettings();
    event->accept();
}


void  MainWindow::errorLog( QString errText)
{
    if (_ui->connectStat->isChecked())    
        qDebug() << "MW-Err:" << errText;
}


void  MainWindow::readSettings()
{
    QPoint  pos = _appSettings->value("main/pos", QPoint(200, 200)).toPoint();
    QSize  size = _appSettings->value("main/size", QSize(400, 400)).toSize();
    _pathWidth = _appSettings->value("main/pathW", 200).toInt();
    bool  viewHidden = _appSettings->value("main/viewHidden", false).toBool();
    bool  hideBidir  = _appSettings->value("main/hideBidir",  true).toBool();
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

