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

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QTreeView>
#include "ArnModel.hpp"
#include "Connector.hpp"

class MultiDelegate;
class ChatServWindow;
class SettingsHandler;
class QSettings;
class QCloseEvent;
class QLabel;
class QLineEdit;
class QGraphicsColorizeEffect;
class QTimer;

namespace Ui {
    class MainWindow;
}


class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow( QWidget* parent = 0);
    ~MainWindow();

private slots:
    void  itemClicked( const QModelIndex &index);
    void  doClientStateChanged( int status);
    void  doStartLogin( int contextCode);
    void  doEndLogin( int resultCode);
    void  doRinfo( int type, const QByteArray& data);
    void  clientError( QString errorText);
    void  on_connectButton_clicked();
    void  on_discoverButton_clicked();
    void  on_chatButton_clicked();
    void  on_terminalButton_clicked();
    void  on_logButton_clicked();
    void  on_editButton_clicked();
    void  on_runButton_clicked();
    void  on_manageButton_clicked();
    void  on_releaseButton_clicked();
    void  on_vcsButton_clicked();
    void  on_settingsButton_clicked();
    void  on_viewHidden_clicked();
    void  on_hideBidir_clicked();
    void  errorLog( QString errText);
    void  readSettings();
    void  writeSettings();
    //void  dataChanged( const QModelIndex& topLeft, const QModelIndex& bottomRight);
    void  updateHidden( int row, QModelIndex parent, bool isHidden);
    void  onKillRequest();
    void  onChatReceived( const QString& text, int prioType);
    void  doChatAdd( const QString& text);
    void  doAbortKillRequest();
    void  doChatButtonEffect();

protected:
    void changeEvent( QEvent *e);
    void closeEvent( QCloseEvent *event);

private:
    void  doPostLoginCancel();
    void  connection( bool isConnect);
    void  setConnectionState( bool isConnect);
    void  setConnectOffGui();
    void  setFuncButtonOffGui();
    void  updateHiddenTree( const QModelIndex& index);
    void  setChatButEff( bool isOn);

    Ui::MainWindow*  _ui;

    ChatServWindow*  _chatServWin;
    QGraphicsColorizeEffect*  _chatButEff;
    QTimer*  _timerChatButEff;
    int  _countChatButEff;

    QLineEdit*  _curItemPathStatus;
    MultiDelegate*  _delegate;
    ArnModel*  _arnModel;
    ArnClient*  _arnClient;
    Connector*  _connector;

    SettingsHandler*  _settings;
    QSettings*  _appSettings;
    QString  _curItemPath;
    int  _pathWidth;
    bool  _isConnect;
    bool  _wasContact;
    bool  _wasConnect;
    bool  _runPostLoginCancel;
    bool  _isLoginCancel;
    bool  _isLoginDialog;
    int  _loginContextCode;
};

#endif // MAINWINDOW_HPP
