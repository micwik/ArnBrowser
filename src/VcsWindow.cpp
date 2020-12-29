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

#include "VcsWindow.hpp"
#include "ArnModel.hpp"
#include "ui_VcsWindow.h"
#include <QInputDialog>
#include <QCloseEvent>
#include <QSettings>
#include <QDebug>


VcsWindow::VcsWindow( QSettings* appSettings, Connector* connector, QWidget* parent)
    : QDialog( parent),
    _ui( new Ui::VcsWindow)
{
    _ui->setupUi( this);

    _connector = connector;
    this->setWindowTitle( QString("Version Control System"));

    _appSettings = appSettings;
    readSettings();

    _filesUpdated = false;
    _refUpdated   = false;

    //// Logics
    _sapiVcs.open( _connector->toLocalPath("//.sys/Persist/Pipes/CommonPipe"));
    _sapiVcs.batchConnectTo( this, "sapi");

    connect( _ui->treeWorkButton, SIGNAL(clicked()), this, SLOT(onTreeChanged()));
    connect( _ui->treeRepoButton, SIGNAL(clicked()), this, SLOT(onTreeChanged()));
    connect( _ui->refIdButton, SIGNAL(clicked()), this, SLOT(onRefTypeChanged()));
    connect( _ui->refTagButton, SIGNAL(clicked()), this, SLOT(onRefTypeChanged()));
    connect( _ui->refSel, SIGNAL(currentIndexChanged(QString)), this, SLOT(onRefChanged(QString)));

    doUpdate();
}


VcsWindow::~VcsWindow()
{
    delete _ui;
}


void  VcsWindow::appendText( QString text)
{
    _ui->textView->setText( _ui->textView->toPlainText() + "\n" + text);
}


void  VcsWindow::doUpdate()
{
    _ui->commitButton->setDisabled( !_ui->treeWorkButton->isChecked());
    _ui->checkoutButton->setDisabled( !_ui->treeRepoButton->isChecked());
    _ui->diffButton->setDisabled( !_ui->treeRepoButton->isChecked());
    _ui->refTypeBox->setDisabled( !_ui->treeRepoButton->isChecked());
    _ui->statusButton->setDisabled( !_ui->treeWorkButton->isChecked());

    if (_refUpdated) {
        _refUpdated  = false;
        _ui->refSel->clear();
        if (_ui->refIdButton->isChecked()) {
            foreach (QString msg, _refIdMsgList) {
                _ui->refSel->addItem( msg);
            }
        }
        else if (_ui->refTagButton->isChecked()) {
            foreach (QString tag, _refTagList) {
                _ui->refSel->addItem( tag);
            }
        }
    }

    if (_filesUpdated) {
        _filesUpdated  = false;
        _ui->filesView->clear();
        foreach (QString file, _fileList) {
            _ui->filesView->addItem( file);
        }
    }
}


QString  VcsWindow::getRef()
{
    if (!_ui->treeRepoButton->isChecked())  return QString();

    int  index = _ui->refSel->currentIndex();
    if (index < 0)  return QString();

    QString  ref;
    if (_ui->refIdButton->isChecked()) {
        ref = _refIdList.at( index);
    }
    else if (_ui->refTagButton->isChecked()) {
        ref = _refTagList.at( index);
    }
    return ref;
}


QStringList  VcsWindow::getSelFiles()
{
    QList<QListWidgetItem*>  selItems = _ui->filesView->selectedItems();
    QStringList  selFiles;
    foreach (QListWidgetItem* item, selItems) {
        selFiles += item->text();
    }
    return selFiles;
}


void  VcsWindow::on_reLoadButton_clicked()
{
    _ui->textView->setText("");
    _ui->filesView->clear();

    if (_ui->treeWorkButton->isChecked()) {
        emit _sapiVcs.pv_ls();
    }
    else if (_ui->treeRepoButton->isChecked()) {
        emit _sapiVcs.pv_vcsFiles( getRef());
    }
}


void  VcsWindow::on_commitButton_clicked()
{
    QInputDialog*  inpDialog = new QInputDialog(this);
    inpDialog->setWindowTitle("Commiting files");
    inpDialog->setLabelText("Commit message");
    inpDialog->setInputMode( QInputDialog::TextInput);
    inpDialog->show();
    connect( inpDialog, SIGNAL(textValueSelected(QString)), this, SLOT(doCloseCommitWindow(QString)));
/*
    WDialog* dia;
    dia = new WDialog("Commiting files");
    dia->setModal(true);
    dia->show();
    dia->setResizable(false);
    dia->setClosable(true);
    dia->rejectWhenEscapePressed();

    WLabel*  msgLabel = new WLabel("Commit message");
    WTextArea*  msgEdit = new WTextArea;
    msgLabel->setBuddy( msgEdit);
    WPushButton*  cancelButton = new WPushButton("Cancel");
    cancelButton->resize(80, 25);
    WPushButton*  okButton = new WPushButton("Ok");
    okButton->resize(80, 25);

    WHBoxLayout*  butlay = new WHBoxLayout;
    butlay->addWidget( cancelButton);
    butlay->addWidget( okButton);
    butlay->addStretch(1);
    butlay->setSpacing(20);
    WVBoxLayout*  toplay = new WVBoxLayout;
    toplay->addWidget( msgLabel);
    toplay->addWidget( msgEdit, 1);
    toplay->addSpacing(5);
    toplay->addLayout( butlay);
    toplay->setSpacing(10);

    dia->contents()->setPadding(8);
    dia->resize(600, 400);
    dia->contents()->setLayout( toplay);
    msgEdit->setFocus(true);

    cancelButton->clicked().connect( dia, &WDialog::reject);
    okButton->clicked().connect( dia, &WDialog::accept);
    dia->finished().connect( boost::bind( &VcsWindow::doCloseCommitWindow, this, dia, msgEdit));
*/
}


void  VcsWindow::on_checkoutButton_clicked()
{
    _ui->textView->setText("");

    _sapiVcs.pv_vcsCheckout( getRef(), getSelFiles());
}


void  VcsWindow::on_tagButton_clicked()
{
    QInputDialog*  inpDialog = new QInputDialog(this);
    inpDialog->setWindowTitle("Set tag name");
    inpDialog->setLabelText("Tag");
    inpDialog->setInputMode( QInputDialog::TextInput);
    inpDialog->show();
    connect( inpDialog, SIGNAL(textValueSelected(QString)), this, SLOT(doCloseTagWindow(QString)));
}


void  VcsWindow::on_diffButton_clicked()
{
    _ui->textView->setText("");

    emit _sapiVcs.pv_vcsDiff( getRef(), getSelFiles());
}


void  VcsWindow::on_statusButton_clicked()
{
    _ui->textView->setText("");

    emit _sapiVcs.pv_vcsStatus();
}


void  VcsWindow::onTreeChanged()
{
    _ui->filesView->clear();
    doUpdate();
}


void  VcsWindow::onRefTypeChanged()
{
    _ui->textView->setText("");
    _ui->refSel->clear();
    if (_ui->refIdButton->isChecked()) {
        emit _sapiVcs.pv_vcsLog(100);
    }
    else if (_ui->refTagButton->isChecked()) {
        emit _sapiVcs.pv_vcsTags();
    }
}


void  VcsWindow::onRefChanged( QString refTxt)
{
    Q_UNUSED( refTxt);

    _ui->textView->setText("");

    QString ref = getRef();
    if (ref.isEmpty())  return;

    emit _sapiVcs.pv_vcsLogRecord( ref);
}


void  VcsWindow::doCloseTagWindow(QString tagName)
{
    if (tagName.isEmpty())  return;

    ARN_RegExp  rx("[^ \\/\\\\]+");
    ARN_RegExpValidator*  validator = new ARN_RegExpValidator( rx, this);
    int pos = 0;
    if (validator->validate(tagName, pos) != ARN_RegExpValidator::Acceptable) {
        _ui->textView->setText("Invalid tagname, not used!");
        return;
    }

    emit _sapiVcs.pv_vcsTag( tagName, getRef());
    _ui->refTagButton->setChecked( false);  // User forced to update refs
}


void  VcsWindow::doCloseCommitWindow(QString msg)
{
    if (msg.isEmpty())  return;

    emit _sapiVcs.pv_vcsCommit( msg, getSelFiles());
    _ui->refIdButton->setChecked( false);  // User forced to update refs
}


void  VcsWindow::sapiVcsNotify( QString msg)
{
    appendText( msg);
}


void  VcsWindow::sapiVcsProgress( int percent, QString msg)
{
    Q_UNUSED( percent);
    Q_UNUSED( msg);
}


void  VcsWindow::sapiVcsUserSettingsR( QString name, QString eMail)
{
    Q_UNUSED( name);
    Q_UNUSED( eMail);
}


void  VcsWindow::sapiVcsFilesR( QStringList files)
{
    _fileList     = files;
    _filesUpdated = true;

    doUpdate();
}


void  VcsWindow::sapiVcsTagR()
{
}


void  VcsWindow::sapiVcsCommitR()
{
}


void  VcsWindow::sapiVcsCheckoutR()
{
}


void  VcsWindow::sapiVcsBranchesR( QStringList branches)
{
    Q_UNUSED( branches);
}


void  VcsWindow::sapiVcsTagsR( QStringList tags)
{
    _refTagList = tags;
    _refUpdated = true;

    doUpdate();
}


void  VcsWindow::sapiVcsStatusR( QString status)
{
    appendText( status);
}


void  VcsWindow::sapiVcsDiffR( QString txt)
{
    appendText( txt);
}


void  VcsWindow::sapiVcsLogRecordR( QString txt)
{
    appendText( txt);
}


void  VcsWindow::sapiVcsLogR( QStringList refs, QStringList msgs)
{
    _refIdList    = refs;
    _refIdMsgList = msgs;
    _refUpdated   = true;

    doUpdate();
}


void  VcsWindow::sapiLsR( QStringList files)
{
    _fileList     = files;
    _filesUpdated = true;

    doUpdate();
}


void  VcsWindow::closeEvent( QCloseEvent* event)
{
    // qDebug() << "Close event";
    writeSettings();
    event->accept();
    deleteLater();
}


void  VcsWindow::readSettings()
{
    QPoint      pos   = _appSettings->value("vcs/pos", QPoint(200, 200)).toPoint();
    QSize       size  = _appSettings->value("vcs/size", QSize(500, 400)).toSize();
    QByteArray  split = _appSettings->value("vcs/split", _ui->splitter->saveState()).toByteArray();
    resize( size);
    move( pos);
    _ui->splitter->restoreState( split);
}


void  VcsWindow::writeSettings()
{
    // qDebug() << "Write vcs settings";
    _appSettings->setValue("vcs/pos", pos());
    _appSettings->setValue("vcs/size", size());
    _appSettings->setValue("vcs/split", _ui->splitter->saveState());
}
