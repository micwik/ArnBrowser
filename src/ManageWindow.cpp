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

#include "ManageWindow.hpp"
#include "ArnModel.hpp"
#include "ui_ManageWindow.h"
#include <QCloseEvent>
#include <QSettings>
#include <QRegExpValidator>
#include <QDebug>


ManageWindow::ManageWindow( QSettings* appSettings, const ConnectorPath& conPath, QWidget* parent) :
    QDialog( parent),
    _ui( new Ui::ManageWindow)
{
    _ui->setupUi( this);

    _conPath = conPath;
    this->setWindowTitle( QString("Manage ") + _conPath.normPath());

    _appSettings = appSettings;
    readSettings();

    _isNewMode     = false;
    _isPersistFile = false;
    _isMandatory   = false;

    QRegExp  rx("^[^/ ][^ ]*$");
    QRegExpValidator*  validator = new QRegExpValidator( rx, this);
    _ui->itemEdit->setValidator( validator);

    //// Logics
    _persistSapi.open( _conPath.toLocalPath("//.sys/Persist/Pipes/CommonPipe"));
    connect( &_persistSapi, SIGNAL(rq_lsR(QStringList)), this, SLOT(lsR(QStringList)));
    connect( &_persistSapi, SIGNAL(rq_dbMandatoryLsR(QStringList)), this, SLOT(mandatoryLsR(QStringList)));

    _arnPath.open( conPath.localPath());
    connect( &_arnPath, SIGNAL(arnLinkDestroyed()), this, SLOT(doUpdate()));
    connect( _ui->armDelButton, SIGNAL(clicked()), this, SLOT(doUpdate()));
    connect( _ui->folderButton, SIGNAL(clicked()), this, SLOT(doUpdate()));
    connect( _ui->typeNormalButton, SIGNAL(clicked()), this, SLOT(doUpdate()));
    connect( _ui->typeBidirButton, SIGNAL(clicked()), this, SLOT(doUpdate()));
    connect( _ui->typePipeButton, SIGNAL(clicked()), this, SLOT(doUpdate()));
    connect( _ui->persistNoneButton, SIGNAL(clicked()), this, SLOT(doUpdate()));
    connect( _ui->persistDbButton, SIGNAL(clicked()), this, SLOT(doUpdate()));
    connect( _ui->persistFileButton, SIGNAL(clicked()), this, SLOT(doUpdate()));
    //_wt->finished().connect( this, &ManageWindow::doCloseWindow);

    if (!_arnPath.isFolder()) {
        QString  normPath = _conPath.normPath();
        _persistSapi.pv_ls( normPath);  // Will finally trigger update of persist
        _persistSapi.pv_dbMandatoryLs( normPath);  // Will finally trigger update of mandatory
    }

    doTypeUpdate();
}

ManageWindow::~ManageWindow()
{
    delete _ui;
}


void ManageWindow::on_itemEdit_textChanged( const QString& txt)
{
    Q_UNUSED(txt)

    doUpdate();
}


void  ManageWindow::doUpdate()
{
    bool  isPathFolder = _arnPath.isFolder();
    bool  isPathOpen   = _arnPath.isOpen();

    _ui->pathEdit->setText( isPathOpen ? _conPath.normPath() : QString());

    if (!isPathOpen) {  // Path link has been destroyed
        _isNewMode = false;
        _ui->itemEdit->setText("");
        _ui->itemEdit->setDisabled(true);
        _ui->folderButton->setChecked(false);
        _ui->folderButton->setDisabled(true);
        _ui->armDelButton->setChecked(false);
        _ui->armDelButton->setDisabled(true);
    }
    else if (!_arnPath.isFolder()) {  // Edit non folder item
        _ui->itemEdit->setText( _arnPath.name( Arn::NameF()));
        _ui->itemEdit->setDisabled(true);
        _ui->folderButton->setChecked(false);
        _ui->folderButton->setDisabled(true);
        if (_arnPath.isPipeMode()) {

        }
    }
    else if (!_isNewMode) {  // View folder
        _ui->itemEdit->setText("");
        _ui->itemEdit->setDisabled(true);
        _ui->folderButton->setChecked(true);
        _ui->folderButton->setDisabled(true);
    }
    else {  // Edit new item
        _ui->itemEdit->setDisabled(false);
        _ui->folderButton->setDisabled(false);
        _ui->folderButton->setChecked( Arn::isFolderPath( _ui->itemEdit->text()));
        _ui->armDelButton->setChecked(false);
        _ui->armDelButton->setDisabled(true);
    }

    bool  isFolder          = _ui->folderButton->isChecked();
    bool  isTypePersistable = !isFolder && !_ui->typePipeButton->isChecked();
    bool  isPersistDB       = isTypePersistable && (_ui->persistDbButton->isChecked());

    _ui->newButton->setEnabled( !_isNewMode && isPathFolder);
    _ui->saveButton->setEnabled(((_isNewMode && _ui->itemEdit->hasAcceptableInput())
                               || !isPathFolder) && isPathOpen);
    _ui->cancelButton->setEnabled( _isNewMode);
    _ui->deleteButton->setEnabled( _ui->armDelButton->isChecked());
    _ui->resetButton->setEnabled(false);  // Default

    _ui->typeSelBox->setDisabled( isFolder || !isPathOpen);
    _ui->persistSelBox->setDisabled( !isTypePersistable || !isPathOpen);
    _ui->mandatoryButton->setDisabled( !isPersistDB);

    Arn::ObjectMode  modePath = _arnPath.getMode();
    if (!isPathFolder && isPathOpen) {
        if (modePath.is( modePath.Pipe)) {
            _ui->typeNormalButton->setDisabled(true);
            _ui->typeBidirButton->setDisabled(true);
            _ui->resetButton->setEnabled( true);
        }
        else if (modePath.is( modePath.BiDir)) {
            _ui->typeNormalButton->setDisabled(true);
            _ui->typeBidirButton->setDisabled(false);
            _ui->resetButton->setEnabled( true);
        }
        else {  // Normal type
            _ui->typeNormalButton->setDisabled(false);
            _ui->typeBidirButton->setDisabled(false);
        }

        if (_isPersistFile) {
            _ui->typePipeButton->setDisabled(true);
        }
        else if (modePath.is( modePath.Save)) {
            _ui->typePipeButton->setDisabled(true);
            _ui->persistNoneButton->setDisabled(true);
            _ui->resetButton->setEnabled( true);
        }
        else {  // No persist
            _ui->typePipeButton->setDisabled(false);
            _ui->persistNoneButton->setDisabled(false);
        }
    }
}


void  ManageWindow::doTypeUpdate()
{
    Arn::ObjectMode  pathMode = _arnPath.getMode();
    if (!_arnPath.isFolder()) {
        if (pathMode.is( pathMode.Pipe))
            _ui->typePipeButton->setChecked(true);
        else if (pathMode.is( pathMode.BiDir))
            _ui->typeBidirButton->setChecked(true);
        else
            _ui->typeNormalButton->setChecked(true);
    }
    else {
        _ui->typeSelBox->setChecked(false);
    }

    doUpdate();
}


void  ManageWindow::doPersistUpdate()
{
    Arn::ObjectMode  modePath = _arnPath.getMode();
    if (_isPersistFile)
        _ui->persistFileButton->setChecked(true);
    else if (modePath.is( modePath.Save))
        _ui->persistDbButton->setChecked(true);
    else
        _ui->persistNoneButton->setChecked(true);

    _ui->mandatoryButton->setChecked( _isMandatory);

    doUpdate();
}


void  ManageWindow::timeoutSave()
{
    _arnPath.arnImport( _storeValue);

    doUpdate();
}


void  ManageWindow::timeoutReset()
{
    _arnPath.open( _conPath.localPath());
    _arnPath.arnImport( _storeValue);
    QString  normPath = _conPath.normPath();
    _persistSapi.pv_ls( normPath);  // Will finally trigger update of persist
    _persistSapi.pv_dbMandatoryLs( normPath);  // Will finally trigger update of mandatory

    doTypeUpdate();
}


void  ManageWindow::on_newButton_clicked()
{
    _isNewMode = true;
    _isPersistFile = false;

    _ui->itemEdit->setText("");
    _ui->folderButton->setChecked(false);
    _ui->typeNormalButton->setChecked(true);
    _ui->persistNoneButton->setChecked(true);

    doUpdate();
    _ui->itemEdit->setFocus();
}


void  ManageWindow::on_deleteButton_clicked()
{
    QString  normPath = _conPath.normPath();
    _persistSapi.pv_rm( normPath);  // Any persist file/tree is deleted
    if (Arn::isFolderPath( normPath) || _arnPath.isSaveMode())
        _persistSapi.pv_dbMandatory( normPath, false);
    _persistSapi.pv_ls( normPath);
    _arnPath.destroyLink();

    doUpdate();
}


void  ManageWindow::on_saveButton_clicked()
{
    ArnItem  arnItemNew;
    ArnItem*  arnItem;
    QString  normPath;

    if (_isNewMode) {
        _ui->itemEdit->setFocus();
        QString  item = _ui->itemEdit->text();

        QString  itemPath = Arn::addPath( _conPath.localPath(), item);
        arnItemNew.open( itemPath);
        arnItem = &arnItemNew;
        normPath = _conPath.toNormPath( itemPath);
    }
    else {
        arnItem = &_arnPath;
        normPath = _conPath.normPath();
    }
    if (!arnItem->isOpen())  return;

    Arn::ObjectMode  mode;

    if (_ui->typeBidirButton->isChecked())
        arnItem->addMode( mode.BiDir);
    else if (_ui->typePipeButton->isChecked())
        arnItem->addMode( mode.Pipe);

    if (_ui->persistDbButton->isChecked()) {
        arnItem->addMode( mode.Save);
        _persistSapi.pv_rm( normPath);
    }
    else if (_ui->persistFileButton->isChecked())
        _persistSapi.pv_touch( normPath);
    else if (_ui->persistNoneButton->isChecked())
        _persistSapi.pv_rm( normPath);

    _persistSapi.pv_dbMandatory( normPath, _ui->mandatoryButton->isChecked());

    _persistSapi.pv_ls( normPath);
    _persistSapi.pv_dbMandatoryLs( normPath);

    if (!_arnPath.isFolder()) {
        _storeValue = _arnPath.arnExport();
        QTimer::singleShot(300, this, SLOT(timeoutSave()));
    }

    _isNewMode = false;
    doUpdate();
}


void  ManageWindow::on_cancelButton_clicked()
{
    _isNewMode = false;

    doUpdate();
}


void  ManageWindow::on_resetButton_clicked()
{
    if (!_arnPath.isFolder()) {
        _storeValue = _arnPath.arnExport();
        if (_arnPath.isSaveMode())
            _persistSapi.pv_dbMandatory( _conPath.normPath(), false);
        _arnPath.destroyLink();
        QTimer::singleShot(300, this, SLOT(timeoutReset()));
    }
}


void  ManageWindow::on_folderButton_clicked()
{
    QString  subPath = _ui->itemEdit->text();
    bool  isFolder   = Arn::isFolderPath( subPath);
    bool  wantFolder = _ui->folderButton->isChecked();
    if (wantFolder && !isFolder)
        _ui->itemEdit->setText( subPath + "/");
    else if (!wantFolder && isFolder) {
        subPath.chop(1);
        _ui->itemEdit->setText( subPath);
    }
}


void  ManageWindow::lsR( QStringList files)
{
    _isPersistFile = false;
    QString  normPath = _conPath.normPath();
    foreach (QString file, files) {
        if (file == normPath)
            _isPersistFile = true;
    }

    doPersistUpdate();
}


void  ManageWindow::mandatoryLsR( QStringList files)
{
    _isMandatory = false;
    QString  normPath = _conPath.normPath();
    foreach (QString file, files) {
        if (file == normPath)
            _isMandatory = true;
    }

    doPersistUpdate();
}


void  ManageWindow::closeEvent( QCloseEvent* event)
{
    // qDebug() << "Close event";
    writeSettings();
    event->accept();
    deleteLater();
}


void  ManageWindow::readSettings()
{
    QPoint  pos = _appSettings->value("manage/pos", QPoint(200, 200)).toPoint();
    QSize  size = _appSettings->value("manage/size", QSize(450, 300)).toSize();
    resize( size);
    move( pos);
}


void  ManageWindow::writeSettings()
{
    // qDebug() << "Write manage settings";
    _appSettings->setValue("manage/pos", pos());
    _appSettings->setValue("manage/size", size());
}
