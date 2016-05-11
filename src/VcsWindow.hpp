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

#ifndef VCSWINDOW_HPP
#define VCSWINDOW_HPP

#include "Connector.hpp"
#include <ArnInc/MQFlags.hpp>
#include <ArnInc/Arn.hpp>
#include <ArnInc/ArnPersistSapi.hpp>
#include <QPointer>
#include <QStringList>
#include <QDialog>

class QSettings;
class QCloseEvent;

namespace Ui {
class VcsWindow;
}


class VcsWindow : public QDialog
{
    Q_OBJECT
    
public:
    explicit VcsWindow( QSettings* appSettings, Connector* connector, QWidget* parent = 0);
    ~VcsWindow();
    
private slots:
    void  on_reLoadButton_clicked();
    void  on_commitButton_clicked();
    void  on_checkoutButton_clicked();
    void  on_tagButton_clicked();
    void  on_diffButton_clicked();
    void  on_statusButton_clicked();
    void  onTreeChanged();
    void  onRefTypeChanged();
    void  onRefChanged( QString refTxt);

    void  doCloseTagWindow(QString tagName);
    void  doCloseCommitWindow( QString msg);
    void  readSettings();
    void  writeSettings();

    void  sapiVcsNotify( QString msg);
    void  sapiVcsProgress( int percent, QString msg=QString());
    void  sapiVcsUserSettingsR( QString name, QString eMail);
    void  sapiVcsFilesR( QStringList files);
    void  sapiVcsTagR();
    void  sapiVcsCommitR();
    void  sapiVcsCheckoutR();
    void  sapiVcsBranchesR( QStringList branches);
    void  sapiVcsTagsR( QStringList tags);
    void  sapiVcsStatusR( QString status);
    void  sapiVcsDiffR( QString txt);
    void  sapiVcsLogRecordR( QString txt);
    void  sapiVcsLogR( QStringList refs, QStringList msgs);
    void  sapiLsR( QStringList files);

protected:
    void closeEvent( QCloseEvent *event);

private:
    void  appendText( QString text);
    void  doUpdate();
    QString  getRef();
    QStringList  getSelFiles();

    QPointer<Connector>  _connector;
    ArnPersistSapi  _sapiVcs;
    QStringList  _refIdList;
    QStringList  _refIdMsgList;
    QStringList  _refTagList;
    QStringList  _fileList;
    bool  _filesUpdated;
    bool  _refUpdated;

    Ui::VcsWindow* _ui;
    QSettings*  _appSettings;
};

#endif // VCSWINDOW_HPP
