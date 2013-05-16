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

#ifndef VCSWINDOW_HPP
#define VCSWINDOW_HPP

#include <ArnLib/MQFlags.hpp>
#include "ArnLib/Arn.hpp"
#include <ArnLib/ArnPersistSapi.hpp>
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
    explicit VcsWindow( QSettings* appSettings, QWidget* parent = 0);
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
