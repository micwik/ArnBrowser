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
#include <QMenu>
#include <QAction>
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
    _filterSearchPaths  = QStringList();
    _filterMatchPaths   = QStringList();
    _curIndexRowNr      = 0;
    _nrIndexRows        = 0;
    _filterState        = FilterState::Init;

    _timerChatButEff = new QTimer( this);
    _timerChatButEff->setInterval(20);
    connect( _timerChatButEff, SIGNAL(timeout()), this, SLOT(doChatButtonEffect()));
    _countChatButEff = 0;
    _chatButEff = new QGraphicsColorizeEffect;
    _chatButEff->setColor( QColor(254,89,6));
    _ui->chatButton->setGraphicsEffect( _chatButEff);
    setChatButEff( false);

    _timerStopFiltering = new QTimer( this);
    _timerStopFiltering->setInterval( 3000);
    connect( _timerStopFiltering, SIGNAL(timeout()), this, SLOT(stopFiltering()));

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
    connect( _arnClient, SIGNAL(replyLs(QStringList,QString)), this, SLOT(commandLsReply(QStringList)));

    //// Setup model
    _arnModel = new ArnModel( _connector, this);
    _arnModel->setClient( _arnClient);
    _arnModel->start();
    connect( _arnModel, SIGNAL(hiddenRow(int,QModelIndex,bool)),
             this, SLOT(updateHidden(int,QModelIndex,bool)));
    connect( _arnModel, SIGNAL(rowInserted(QModelIndex,int)),
             this, SLOT(rowInserted(QModelIndex,int)));
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

    connect( _ui->arnView, SIGNAL(clicked(QModelIndex)), this, SLOT(onItemClick(QModelIndex)));
    connect( _ui->arnView, SIGNAL(customContextMenuRequested(QPoint)),
             this, SLOT(onContextMenuRequested(QPoint)));
    connect( _ui->arnView, SIGNAL(expanded(QModelIndex)), this, SLOT(expandedReply(QModelIndex)));

    setConnectOffGui();
    // Test chat button
    //_ui->chatButton->setVisible(true);
    //setChatButEff( true);

    connect( _ui->filterEdit, SIGNAL(returnPressed()), this, SLOT(startFiltering()));
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
        doEmptyFiltering();
        setFilterText("");
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
    if (_curItemPath == "/") {
        _arnModel->clear();
        _ui->arnView->reset();
    }
    else {
        QModelIndex  parentNodeIdx = _arnModel->parent( _ui->arnView->currentIndex());
        ArnNode  *parentNode = _arnModel->nodeFromIndex( parentNodeIdx);
        Q_ASSERT(parentNode);

        ArnM::destroyLinkLocal( _curItemPath);
        _arnModel->netChildFound( _curItemPath, parentNode);
        setCurItemPath();
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


void  MainWindow::onContextMenuRequested( const QPoint& pos)
{
    // qDebug() << "Context menu: pos=" << pos;
    QString  path = "/";
    int  col = -1;  // Mark not valid
    Q_UNUSED(col);
    QModelIndex  index = _ui->arnView->indexAt( pos);
    if (index.isValid()) {
        ArnNode*  node = _arnModel->nodeFromIndex( index);
        if (node) {
            col  = index.column();
            path = node->path();
        }
    }

    setCurItemPath( path);
    // qDebug() << "Context menu: path=" << _curItemPath << " col=" << col;

    QMenu*  menu = new QMenu( this);
    if ( _ui->terminalButton->isEnabled()) {
        QAction*  action = new QAction( QIcon(":/pic/terminal.png"), "Terminal", this);
        menu->addAction( action);
        connect( action, SIGNAL(triggered()), this, SLOT(on_terminalButton_clicked()));
    }
    if ( _ui->logButton->isEnabled()) {
        QAction*  action = new QAction( QIcon(":/pic/log.png"), "Log", this);
        menu->addAction( action);
        connect( action, SIGNAL(triggered()), this, SLOT(on_logButton_clicked()));
    }
    if ( _ui->editButton->isEnabled()) {
        QAction*  action = new QAction( QIcon(":/pic/Pencil-icon.png"), "Edit", this);
        menu->addAction( action);
        connect( action, SIGNAL(triggered()), this, SLOT(on_editButton_clicked()));
    }
    if ( _ui->runButton->isEnabled()) {
        QAction*  action = new QAction( QIcon(":/pic/Play.png"), "Run", this);
        menu->addAction( action);
        connect( action, SIGNAL(triggered()), this, SLOT(on_runButton_clicked()));
    }
    if ( _ui->manageButton->isEnabled()) {
        QAction*  action = new QAction( QIcon(":/pic/tools-icon.png"), "Manage", this);
        menu->addAction( action);
        connect( action, SIGNAL(triggered()), this, SLOT(on_manageButton_clicked()));
    }
    {
        QString  menuTxt = (_curItemPath == "/") ? "Release all" : "Release item(s)";
        QAction*  action = new QAction( QIcon(":/pic/close.png"), menuTxt, this);
        menu->addAction( action);
        connect( action, SIGNAL(triggered()), this, SLOT(on_releaseButton_clicked()));
    }
    menu->popup( _ui->arnView->viewport()->mapToGlobal( pos));
}


void  MainWindow::updateHiddenTree( const QModelIndex& index)
{
    _arnModel->data( index, ArnModel::Role::Hidden);  // Hidden row is updated by signal

    int  rows = _arnModel->rowCount( index);
    for (int i = 0; i < rows; ++i) {
        updateHiddenTree( _arnModel->index(i, 0, index));
    }
}


void MainWindow::commandLsReply( const QStringList& itemList )
{
    QString prefixPath = _filterSearchPaths.first();
    _filterSearchPaths.removeFirst();
    foreach ( QString item, itemList ) {
        QString path = prefixPath + item;
        item = Arn::convertPath( item, Arn::NameF::EmptyOk );
        if ( matchesFilter( item)) {
            path = Arn::convertPath( path, Arn::NameF::EmptyOk);
            _filterMatchPaths.append( path);
        }
        else if ( !ArnM::isLeaf( "/@host" + path )) {
            _filterSearchPaths.append( path);
        }
    }
    if ( _filterSearchPaths.isEmpty() ) {
        _filterState = FilterState::ExpandNextMatch;
    }
    handleFiltering();
}


bool MainWindow::matchesFilter(const QString& string)
{
    return string.contains( QRegExp(_curFilterText, Qt::CaseSensitive, QRegExp::PatternSyntax::RegExp2));
}


void MainWindow::setFilterText( const QString& filterText)
{
    _ui->filterEdit->setText( filterText);
    _curFilterText = filterText;
}


void MainWindow::doEmptyFiltering()
{
    _curFilterText = "";
    doFiltering();
}


void MainWindow::doFiltering()
{
    if (_curFilterText == "" ) {
        _filterState = FilterState::FilterTree;
        _ui->arnView->collapseAll();
    }
    else {
        _filterState = FilterState::Init;
    }
    handleFiltering();
}


void MainWindow::stopFiltering()
{
    _timerStopFiltering->stop();
    _filterState = FilterState::Init;
}


void MainWindow::startFiltering()
{
    doEmptyFiltering();
    setFilterText( _ui->filterEdit->text());
    doFiltering();
}


void MainWindow::expandedReply(QModelIndex index)
{
    if ( _arnModel->rowCount( index) > 0) {
        expandNextFolder();
    }
}


void MainWindow::rowInserted(QModelIndex index, int row)
{
    QString newPath = _connector->toNormPath( _arnModel->data( _arnModel->index(row, 0, index),
                                                               ArnModel::Role::Path).toString() );
    QString curIndexPath = "";
    if (_lastIndexMatch != QModelIndex()) {
        curIndexPath = _connector->toNormPath( _arnModel->data( _lastIndexMatch,
                                                                ArnModel::Role::Path).toString() );
    }
    if (_filterState == FilterState::FilterTree) {
        filterTree( index);
        _timerStopFiltering->start();
    }
    else if ( newPath.contains( curIndexPath)) {
        expandNextFolder( row);
    }
}


void MainWindow::expandNextFolder( const int row)
{
    if ( _filterState == FilterState::WaitingForExpand) {
        _curIndexRowNr = row;
        _filterState = FilterState::ExpandNextFolder;
        handleFiltering();
    }
}


void MainWindow::handleFiltering()
{
    if ( _filterState == FilterState::Init) {
        _filterMatchPaths  = QStringList();
        _filterSearchPaths = QStringList();
        _filterSearchPaths.append( "/" );
        _filterState = FilterState::FindMatches;
        handleFiltering();
    }
    else if ( _filterState == FilterState::FindMatches) {
        _arnClient->commandLs( _filterSearchPaths.first());
    }
    else if ( _filterState == FilterState::ExpandNextMatch) {
        if ( !_filterMatchPaths.isEmpty() ) {
            QString nextMatchPath = _filterMatchPaths.first();
            _filterMatchPaths.removeFirst();
            expandPath( nextMatchPath );
        }
        else {
            _filterState = FilterState::FilterTree;
            handleFiltering();
        }
    }
    else if ( _filterState == FilterState::ExpandNextFolder) {
        if ( !_foldersToExpand.isEmpty() ) {
            _nrIndexRows = _arnModel->rowCount( _lastIndexMatch);
            if (_curIndexRowNr < _nrIndexRows) {
                expandIndexIfMatchingFilter( _arnModel->index(_curIndexRowNr, 0, _lastIndexMatch) );
            }
            else {
                qCritical() << "ERROR: List of folders to expand is not empty but current row nr "
                               "is higher than the nr of available rows. Not supposed to ever happen.";
            }
        }
        else {
            _filterState = FilterState::ExpandNextMatch;
            handleFiltering();
        }
    }
    else if ( _filterState == FilterState::FilterTree) {
        filterTree( _ui->arnView->rootIndex() );
        _timerStopFiltering->start();
    }
}


void MainWindow::expandPath( const QString& path )
{
    QString curPath = path.left( path.size() - 1 );
    _foldersToExpand = curPath.split("/");
    for ( int i = 0; i < _foldersToExpand.size(); i++) {
        if (_foldersToExpand[i] == "") {
            _foldersToExpand[i] = "/";
        }
    }
    if ( !_foldersToExpand.isEmpty() ) {
        expandIndexIfMatchingFilter( _ui->arnView->rootIndex() );
    }
}


void MainWindow::expandIndexIfMatchingFilter( const QModelIndex index )
{
    QString curFolder = _foldersToExpand.first();
    QString curIndexHasPath = _connector->toNormPath( _arnModel->data( index, ArnModel::Role::Path).toString() );
    if ( curIndexHasPath.contains( curFolder ) ) {
        _filterState = FilterState::ExpandNextFolder;
        _lastIndexMatch = index;
        _foldersToExpand.removeFirst();
        _curIndexRowNr = 0;
        if (!_ui->arnView->isExpanded( _lastIndexMatch) && curIndexHasPath != "/" && !_foldersToExpand.isEmpty() ) {
            _filterState = FilterState::WaitingForExpand;
            _ui->arnView->expand( _lastIndexMatch);
            return;
        }
    }
    else if (_curIndexRowNr + 1 < _nrIndexRows) {
        _curIndexRowNr++;
    }
    else {
        _filterState = FilterState::WaitingForExpand;
    }
    if (_filterState != FilterState::FilterTree) {
        handleFiltering();
    }
}


void MainWindow::filterTree( const QModelIndex& index)
{
    updateFilterIndex( index );

    int  rows = _arnModel->rowCount( index);
    for (int i = 0; i < rows; ++i) {
        filterTree( _arnModel->index(i, 0, index));
    }
}


void MainWindow::updateFilterIndex(QModelIndex index)
{
    QString path = _connector->toNormPath( _arnModel->data( index, ArnModel::Role::Path).toString() );
    bool hideRow = false;
    QModelIndex parentIndex = _arnModel->parent( index);
    // Never hide root aka '//':
    if ( _curFilterText != "" && !matchesFilter( path) && path.length() > 2 ) {
        hideRow = true;
    }
    _ui->arnView->setRowHidden( index.row(), parentIndex, hideRow );

    // Show all parents of a child that is matching with filter
    if (hideRow == false) {
        int curRow = parentIndex.row();
        parentIndex = _arnModel->parent( parentIndex);
        while (parentIndex != QModelIndex()) {
            _ui->arnView->setRowHidden( curRow, parentIndex, hideRow );
            curRow = parentIndex.row();
            parentIndex = _arnModel->parent( parentIndex);
        }
    }

    // Make sure that hidden rows stay hidden when filter is empty
    _arnModel->data( index, ArnModel::Role::Hidden);
}


void  MainWindow::onItemClick( const QModelIndex& index)
{
    QString  curItemPath = _arnModel->data( index, ArnModel::Role::Path).toString();
    // qDebug() << "curItem=" << curItemPath << " rect=" << _ui->arnView->visualRect( index);
    if (curItemPath == _curItemPath) {
        setCurItemPath();
        _ui->arnView->clearSelection();
    }
    else {
        setCurItemPath( curItemPath);
    }
}


void  MainWindow::onItemEditTrigg( const QModelIndex& index)
{
    QString  itemPath = _arnModel->data( index, ArnModel::Role::Path).toString();
    // qDebug() << "EditItem=" << itemPath << " rect=" << _ui->arnView->visualRect( index);
    if (itemPath.isEmpty())  return;

    setCurItemPath( itemPath);
    if ( _ui->terminalButton->isEnabled()) {
        on_terminalButton_clicked();
    }
    else if ( _ui->editButton->isEnabled()) {
        on_editButton_clicked();
    }
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
        if (!_delegate) {
            _delegate = new MultiDelegate;
            connect( _delegate, SIGNAL(itemEditTrigged(QModelIndex)),
                     this, SLOT(onItemEditTrigg(QModelIndex)));
        }
        _ui->arnView->setItemDelegate( _delegate);
        _ui->arnView->setEnabled( true);
        _ui->arnView->setColumnWidth(0, _pathWidth);
        _ui->arnView->setColumnWidth(1, 10);

        _chatServWin->reset();

        setCurItemPath();
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


void  MainWindow::setCurItemPath( const QString& path)
{
    ArnItem  arnItem;
    Arn::DataType  type;

    if (path.isEmpty() || (path == "/")) {  // Not selected item
        _curItemPath = "/";
        _ui->pathEdit->setText("");
    }
    else {
        _curItemPath = path;
        _ui->pathEdit->setText( _connector->toNormPath( _curItemPath));
        arnItem.open( _curItemPath);
        type = arnItem.type();
    }

    // Set state for Terminal & Log button
    _ui->terminalButton->setEnabled( arnItem.isPipeMode());
    _ui->logButton->setEnabled( arnItem.isPipeMode());

    // Set state for Edit button
    bool  editEn =  arnItem.isOpen()
                && !arnItem.isFolder()
                && ((type == type.Null)
                 || (type == type.ByteArray)
                 || (type == type.String));
    _ui->editButton->setEnabled( editEn);

    // Set state for Run button
#ifdef QMLRUN
    bool  runEn = !arnItem.isFolder() && arnItem.name( Arn::NameF()).endsWith(".qml");
    _ui->runButton->setEnabled( runEn);
#endif

    bool  manageEn = arnItem.isOpen();
    _ui->manageButton->setEnabled( manageEn);
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

