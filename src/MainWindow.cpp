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
#include "SettingsWindow.hpp"
#include "DiscoverWindow.hpp"
#include "ChatServWindow.hpp"
#include "LoginDialog.hpp"
#include "MultiDelegate.hpp"
#include "SettingsHandler.hpp"
#include <ArnInc/ArnClient.hpp>
#include <ArnInc/XStringMap.hpp>
#include <QMessageBox>
#include <QSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QSettings>
#include <QCloseEvent>
#include <QGraphicsColorizeEffect>
#include <QDebug>
#include <math.h>
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

    _isConnect          = false;
    _wasContact         = false;
    _wasConnect         = false;
    _isLoginCancel      = false;
    _runPostLoginCancel = false;
    _isLoginDialog      = false;
    _loginContextCode   = 0;

    QLabel*  curItemPathLabel = new QLabel;
    curItemPathLabel->setText("Path:");
    _curItemPathStatus = new QLineEdit;
    _curItemPathStatus->setReadOnly( true);
    _curItemPathStatus->setStyleSheet("QLineEdit{background: lightgrey;}");
    _ui->statusBar->addPermanentWidget( curItemPathLabel);
    _ui->statusBar->addPermanentWidget( _curItemPathStatus);

    _timerChatButEff = new QTimer( this);
    _timerChatButEff->setInterval(20);
    connect( _timerChatButEff, SIGNAL(timeout()), this, SLOT(doChatButtonEffect()));
    _countChatButEff = 0;
    _chatButEff = new QGraphicsColorizeEffect;
    _chatButEff->setColor( QColor(254,89,6));
    _ui->chatButton->setGraphicsEffect( _chatButEff);
    setChatButEff( false);

    _appSettings = new QSettings("MicTron", "ArnBrowser");
    _settings    = new SettingsHandler( _appSettings);
    _settings->readSettings();

    //// Error log from Arn system
    ArnM::setConsoleError( false);
    connect( &ArnM::instance(), SIGNAL(errorLogSig(QString,uint,void*)),
             this, SLOT(errorLog(QString)));

    //// Prepare connect to Arn server
    _connector = new Connector( this);
    _arnClient = new ArnClient( this);
    _arnClient->setDemandLogin( false);
    // _arnClient->setReceiveTimeout(5);  // Base time for receiver timeout
    connect( _arnClient, SIGNAL(connectionStatusChanged(int,int)), this, SLOT(doClientStateChanged(int)));
    connect( _arnClient, SIGNAL(loginRequired(int)), this, SLOT(doStartLogin(int)));
    connect( _arnClient, SIGNAL(replyInfo(int,QByteArray)), SLOT(doRinfo(int,QByteArray)));
    connect( _arnClient, SIGNAL(killRequested()), this, SLOT(onKillRequest()));
    connect( _arnClient, SIGNAL(chatReceived(QString,int)), this, SLOT(onChatReceived(QString,int)));

    //// Setup model
    _arnModel = new ArnModel( _connector, this);
    _arnModel->setClient( _arnClient);
    _arnModel->start();
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

    _chatServWin = new ChatServWindow( _appSettings, this);
    connect( _chatServWin, SIGNAL(txtOutput(QString)), this, SLOT(doChatAdd(QString)));
    connect( _chatServWin, SIGNAL(abortKillRequest()), this , SLOT(doAbortKillRequest()));

#if QT_VERSION >= 0x050000 && QT_VERSION < 0x050300
    _ui->portEdit->setRange( 1, 1065535);  // Fix early Qt5 layout bug giving to little space
#endif

    connect( _ui->arnView, SIGNAL(clicked(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));

    setConnectOffGui();
    // Test chat button
    //_ui->chatButton->setVisible(true);
    //setChatButEff( true);
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
    bool  wasConnect = _wasConnect;
    setConnectionState( isConnect);

    _isLoginCancel = false;
    _ui->connectButton->setEnabled( false);
    if (isConnect) {
        connect( _arnClient, SIGNAL(tcpError(QString,QAbstractSocket::SocketError)),
                 this, SLOT(clientError(QString)));
        //// Setup WhoIAm
        Arn::XStringMap  wimXsm;
        wimXsm.add("Agent", "Arn Browser - " + ver);
        wimXsm.add("UserName", _settings->d.userName);
        wimXsm.add("Contact",  _settings->d.contact);
        wimXsm.add("Location", _settings->d.location);
        _arnClient->setWhoIAm( wimXsm);

        _arnClient->connectToArn( _ui->hostEdit->text(), _ui->portEdit->value());
        _connector->setCurHost( _ui->hostEdit->text());
        _ui->discoverButton->setVisible( false);
        _ui->hostEdit->setEnabled( false);
        _ui->portEdit->setEnabled( false);
    }
    else if (wasConnect) {
        _arnModel->clear();
        ArnClient::ConnectStat  stat = _arnClient->connectStatus();
        if ((stat != ArnClient::ConnectStat::Connected)  // No contact with server
        &&  (stat != ArnClient::ConnectStat::Negotiating))
            setConnectOffGui();
        _arnClient->close();
    }
    else {  // Never got connected but got contact, model not dirty
        _arnClient->close();
        setConnectOffGui();
    }
}


void  MainWindow::setConnectionState( bool isConnect)
{
    _isConnect = isConnect;
    if (!isConnect) {
        _wasContact = false;
        _wasConnect = false;
    }
    _ui->connectButton->setChecked( isConnect);
}


void MainWindow::setConnectOffGui()
{
    setFuncButtonOffGui();

    _ui->connectStat->setVisible( false);
    _ui->connectButton->setEnabled( true);
    _ui->discoverButton->setVisible( true);
    _ui->chatButton->setVisible( false);

    _ui->hostEdit->setEnabled( true);
    _ui->portEdit->setEnabled( true);
}


void MainWindow::setFuncButtonOffGui()
{
    _ui->terminalButton->setEnabled( false);
    _ui->logButton->setEnabled( false);
    _ui->editButton->setEnabled( false);
    _ui->runButton->setEnabled( false);
    _ui->manageButton->setEnabled( false);
    _ui->releaseButton->setEnabled( false);
    _ui->vcsButton->setEnabled( false);
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


void  MainWindow::on_chatButton_clicked()
{
    _chatServWin->setVisible( !_chatServWin->isVisible());
    setChatButEff( false);
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


void  MainWindow::on_releaseButton_clicked()
{
    if (_curItemPath.isEmpty()) {
        _arnModel->clear();
        _ui->arnView->reset();
    }
    else {
        QModelIndex  parentNodeIdx = _arnModel->parent( _ui->arnView->currentIndex());
        ArnNode  *parentNode = _arnModel->nodeFromIndex( parentNodeIdx);
        Q_ASSERT(parentNode);

        ArnM::destroyLinkLocal( _curItemPath);
        _arnModel->netChildFound( _curItemPath, parentNode);
        _curItemPath = "";
        _curItemPathStatus->setText("");
        _ui->arnView->clearSelection();
    }
}


void  MainWindow::on_vcsButton_clicked()
{
    VcsWindow*  vcsWindow = new VcsWindow( _appSettings, _connector, 0);
    vcsWindow->show();
}


void MainWindow::on_settingsButton_clicked()
{
    SettingsWindow*  settingsWin = new SettingsWindow( _settings, 0);
    Q_UNUSED(settingsWin);
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
    ArnItem  arnItem;
    Arn::DataType  type;
    QString  curItemPath = _arnModel->data( index, ArnModel::Role::Path).toString();
    if (curItemPath == _curItemPath) {
        _curItemPath = "";
        _curItemPathStatus->setText("");
        _ui->arnView->clearSelection();
    }
    else {
        _curItemPath = curItemPath;
        _curItemPathStatus->setText( _connector->toNormPath( _curItemPath));
        arnItem.open( _curItemPath);
        type = arnItem.type();
    }

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


void MainWindow::doClientStateChanged( int status)
{
    // qDebug() << "ClientStateChanged: state=" << status;

    if ((status == ArnClient::ConnectStat::Negotiating) && !_arnClient->isReContact()) {
        //// Initialy connected for negotiation and login
        // qDebug() << "ClientStateChanged Negotiating (!reContact):";
        _wasContact = true;
        _arnClient->setAutoConnect( true, 2);
        disconnect( _arnClient, SIGNAL(tcpError(QString,QAbstractSocket::SocketError)),
                    this, SLOT(clientError(QString)));

        _appSettings->setValue("connect/host", _ui->hostEdit->text());
        _appSettings->setValue("connect/port", _ui->portEdit->value());
    }
    else if ((status == ArnClient::ConnectStat::Connected) && !_arnClient->isReConnect()) {
        //// Fully connected also after any negotiation and login, but not reconnected
        // qDebug() << "ClientStateChanged Connected (!reConnect):";
        _wasConnect = true;
        if (_ui->arnView->model()) { // model already set, just reset viewer
            _arnModel->clear();  // Needed after a canceled login (why?)
            _ui->arnView->reset();
        }
        else    // model has not been set, do it now
            _ui->arnView->setModel( _arnModel);
        _arnModel->setHideBidir( _ui->hideBidir->isChecked());
        _ui->releaseButton->setEnabled( true);
        _ui->vcsButton->setEnabled( true);
        _ui->connectButton->setEnabled( true);
        _ui->manageButton->setEnabled( true);

        _delegate = qobject_cast<MultiDelegate*>( _ui->arnView->itemDelegate());
        if (!_delegate)
            _delegate = new MultiDelegate;
        _ui->arnView->setItemDelegate( _delegate);
        _ui->arnView->setEnabled( true);
        _ui->arnView->setColumnWidth(0, _pathWidth);
        _ui->arnView->setColumnWidth(1, 10);

        _chatServWin->reset();

        _curItemPath = "/";
    }
    else if ((status == ArnClient::ConnectStat::Disconnected) && !_isConnect) {
        //// Manual disconnection from user
        // qDebug() << "ClientStateChanged Manual Disconnect:";
        setConnectOffGui();
    }

    _ui->connectStat->setChecked( (status == ArnClient::ConnectStat::Connected)
                              || ((status == ArnClient::ConnectStat::Negotiating) && _isLoginCancel));
    _ui->connectStat->setVisible( _wasContact);

    _ui->chatButton->setVisible( _wasConnect);
}


void  MainWindow::doStartLogin( int contextCode)
{
    if (_isLoginDialog)  return;  // Login dialog already in use
    if (_isLoginCancel)  return;  // User has canceled login earlier

    _loginContextCode = contextCode;
    _isLoginDialog    = true;
    LoginDialog*  loginDialog = new LoginDialog( contextCode, 0);
    connect( loginDialog, SIGNAL(finished(int)), this, SLOT(doEndLogin(int)));
}


void  MainWindow::doEndLogin( int resultCode)
{
    _isLoginDialog = false;

    if (resultCode != QDialog::Accepted) {
        if (_loginContextCode >= 2) {  // Fatal connection problem, abort
            connection( false);
            return;
        }
        //// Proceed with only viewing freePaths
        doClientStateChanged( ArnClient::ConnectStat::Connected);
        _arnClient->loginToArn("", "");  // Empty login to set local allow (all)

        _runPostLoginCancel = true;
        _isLoginCancel      = true;
        _arnClient->commandInfo( Arn::InfoType::Custom);

        setFuncButtonOffGui();
        return;
    }

    LoginDialog*  loginDialog = qobject_cast<LoginDialog*>( sender());
    Q_ASSERT(loginDialog);

    QString  userName;
    QString  password;
    loginDialog->getResult( userName, password);
    // qDebug() << "doEndLogin user=" << userName << " pass=" << password;

    _arnClient->loginToArn( userName, password);    

    if (_arnClient->connectStatus() != ArnClient::ConnectStat::Negotiating)  // Lost ArnServer ?
        _ui->connectButton->setEnabled( true);
}


void MainWindow::doPostLoginCancel()
{
    if (!_runPostLoginCancel)  return;  // Only run in apropriate sequence
    _runPostLoginCancel = false;
    QStringList  freePaths = _arnClient->freePaths();
    // qDebug() << "doPostLoginCancel: freePaths=" << freePaths;

    foreach (const QString& path, freePaths) {
        _arnModel->doFakePath( path);
    }
}


void  MainWindow::doRinfo( int type, const QByteArray& data)
{
    switch (type) {
    case Arn::InfoType::Custom:
    {
        Arn::XStringMap  xm( data);
        Q_UNUSED(xm);

        doPostLoginCancel();
        break;
    }
    default:;
    }
}


void  MainWindow::clientError( QString errorText)
{
    disconnect( _arnClient, SIGNAL(tcpError(QString,QAbstractSocket::SocketError)),
             this, SLOT(clientError(QString)));

    setConnectionState( false);
    setConnectOffGui();

    QMessageBox msgBox;
    msgBox.setWindowTitle( tr("Error"));
    msgBox.setIcon( QMessageBox::Warning);
    msgBox.setText( tr("Can not Connect to Host"));
    msgBox.setInformativeText( errorText);
    msgBox.exec();
}


void  MainWindow::updateHidden(int row, QModelIndex parent, bool isHidden)
{
    bool  hide = isHidden && !_ui->viewHidden->isChecked();
    if (_ui->arnView->isRowHidden( row, parent) != hide) {
        _ui->arnView->setRowHidden( row, parent, hide);
        // qDebug() << "UpdateHidden: row=" << row << "hide=" << hide;
    }
}


void MainWindow::onKillRequest()
{
    connection( false);
}


void  MainWindow::setChatButEff( bool isOn)
{
    if (isOn && _chatButEff->isEnabled())  return;  // Already enabled (don't disturb cycle)

    _chatButEff->setEnabled( isOn);
    if (isOn) {
        _timerChatButEff->start();
    }
    else {
        _timerChatButEff->stop();
    }
    _countChatButEff = 0;
}


void  MainWindow::doChatButtonEffect()
{
    _countChatButEff = (_countChatButEff + 10) % 1000;
    double  str = .5 + .5 * sin( 2 * M_PI * _countChatButEff / 1000);
    _chatButEff->setStrength( str);
}


void  MainWindow::onChatReceived( const QString& text, int prioType)
{
    if (prioType == 1)
        _chatServWin->addTxtPrio( text);
    else {
        _chatServWin->addTxtNormal( text);
        if (!_chatServWin->isVisible())
            setChatButEff( true);
    }
}


void  MainWindow::doChatAdd( const QString& text)
{
    _arnClient->chatSend( text, 2);
}


void MainWindow::doAbortKillRequest()
{
    _arnClient->abortKillRequest();
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
    // qDebug() << "MainWindow: Close event";
    writeSettings();
    event->accept();

    _chatServWin->close();
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
    // qDebug() << "Write main settings";
    _appSettings->setValue("main/pos", pos());
    _appSettings->setValue("main/size", size());
    if (_ui->arnView->isEnabled())
        _appSettings->setValue("main/pathW", _ui->arnView->columnWidth(0));
    _appSettings->setValue("main/viewHidden", _ui->viewHidden->isChecked());
    _appSettings->setValue("main/hideBidir",  _ui->hideBidir->isChecked());
}

