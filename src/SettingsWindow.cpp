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

#include "SettingsWindow.hpp"
#include "ui_SettingsWindow.h"
#include "SettingsHandler.hpp"
#include <ArnInc/ArnClient.hpp>
#include <ArnInc/Arn.hpp>
#include <QDialogButtonBox>
#include <QCloseEvent>
#include <QSettings>
#include <QDebug>
#include <QFontDialog>


SettingsWindow::SettingsWindow( SettingsHandler* settings, QWidget* parent) :
    QDialog( parent),
    _ui( new Ui::SettingsWindow)
{
    _ui->setupUi( this);
    this->setWindowTitle( QString("Settings - ArnBrowser"));
    _ui->encryptEdit->addItems( Arn::EncryptPolicy::txt().getBasicTextList());
    setModal( true);
    show();

    _settings    = settings;
    _appSettings = _settings->appSettings();
    readSettings();
}


SettingsWindow::~SettingsWindow()
{
    delete _ui;
}


void  SettingsWindow::accept()
{
    QDialog::accept();
    close();
}


void  SettingsWindow::reject()
{
    QDialog::reject();
    close();
}


void  SettingsWindow::closeEvent( QCloseEvent* event)
{
    // qDebug() << "SettingsWindow Close event";
    writeSettings();
    event->accept();
    deleteLater();
}


void  SettingsWindow::on_generateButton_clicked()
{
    QString  pwHashed = ArnClient::passwordHash( _ui->passwordEdit->text());
    _ui->hashEdit->setText( pwHashed);
}

void SettingsWindow::on_fontButton_clicked()
{
    bool ok = false;
    QFont currentFont;
    currentFont.fromString( _settings->d.font );
    QFont font = QFontDialog::getFont( &ok, currentFont, this);
    _settings->d.font = font.toString();
    return;
}


void  SettingsWindow::readSettings()
{
    QPoint  pos   = _appSettings->value("set/pos", QPoint(200, 200)).toPoint();
    // QSize   size  = _appSettings->value("set/size", QSize(600, 500)).toSize();
    // resize( size);
    move( pos);

    _ui->userNameEdit->setText( _settings->d.userName);
    _ui->contactEdit->setText(  _settings->d.contact);
    _ui->locationEdit->setText( _settings->d.location);
    _ui->rowsEdit->setText( QString::number( _settings->d.maxRows));
    _ui->encryptEdit->setCurrentIndex( _settings->d.encryptPol);
}


void  SettingsWindow::writeSettings()
{
    // qDebug() << "Write settings";
    _appSettings->setValue("set/pos", pos());
    _appSettings->setValue("set/size", size());

    _settings->d.userName   = _ui->userNameEdit->text();
    _settings->d.contact    = _ui->contactEdit->text();
    _settings->d.location   = _ui->locationEdit->text();
    _settings->d.maxRows    = _ui->rowsEdit->text().toInt();
    _settings->d.encryptPol = _ui->encryptEdit->currentIndex();
    _settings->writeSettings();
}
