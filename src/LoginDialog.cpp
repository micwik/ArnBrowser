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

#include "LoginDialog.hpp"
#include "ui_LoginDialog.h"
#include <QDialogButtonBox>
#include <QCloseEvent>
#include <QDebug>


LoginDialog::LoginDialog( int contextCode, QWidget* parent) :
    QDialog( parent),
    _ui( new Ui::LoginDialog)
{
    _ui->setupUi( this);
    this->setWindowTitle( QString("Login ARN"));
    setModal( true);
    show();

    switch (contextCode) {
    case 0:
        _ui->messageLabel->setText("Login to ARN server");
        break;
    case 1:
        _ui->messageLabel->setText("Login failed, retry login to ARN server");
        break;
    case 2:
        _ui->messageLabel->setText("ARN server gave bad password, it's not trusted");
        setGuiNoLogin();
        break;
    case 3:
        _ui->messageLabel->setText("ARN server don't support login as demanded");
        setGuiNoLogin();
        break;
    case 4:
        _ui->messageLabel->setText("ARN server bad sequence during negotiation");
        setGuiNoLogin();
        break;
    default:
        _ui->messageLabel->setText("Unknown Arn protocol problem");
        setGuiNoLogin();
        break;
    }
}


LoginDialog::~LoginDialog()
{
    delete _ui;
}


void LoginDialog::setGuiNoLogin()
{
    _ui->userEdit->setEnabled( false);
    _ui->passEdit->setEnabled( false);
    _ui->buttonBox->setStandardButtons( QDialogButtonBox::Abort);
}


void  LoginDialog::getResult( QString& userName, QString& password)
{
    userName = _ui->userEdit->text();
    password = _ui->passEdit->text();
}


void LoginDialog::accept()
{
    QDialog::accept();
    close();
}


void LoginDialog::reject()
{
    QDialog::reject();
    close();
}


void  LoginDialog::closeEvent( QCloseEvent* event)
{
    // qDebug() << "LoginDialog Close event";
    event->accept();
    deleteLater();
}
