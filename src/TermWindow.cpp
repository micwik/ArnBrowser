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

#include "TermWindow.hpp"
#include "ui_TermWindow.h"
#include <QSettings>
#include <QCloseEvent>
#include <QDebug>


TermWindow::TermWindow( QSettings* appSettings, const ConnectorPath& conPath, QWidget* parent)
    : QDialog( parent),
    _ui( new Ui::TermWindow)
{
    _ui->setupUi( this);

    QString  path = conPath.localPath();
    QString  rqPath = Arn::providerPathIf( path, false);

    QString  normPath = conPath.toNormPath( rqPath);
    QString  curHost  = conPath.curHost();
    this->setWindowTitle( QString("Terminal ") + normPath + " @" + curHost);

    _appSettings = appSettings;
    readSettings();

    _pipeRq.open( rqPath);
    _pipePv.open( Arn::twinPath( rqPath));

    _ui->pipePath->setText( normPath);
    _ui->pipePath->setReadOnly( true);
    _ui->textEdit->setReadOnly( true);
    _ui->lineEditRq->setFocus();

    connect( &_pipeRq, SIGNAL(arnLinkDestroyed()), this, SLOT(deleteLater()));
    connect( &_pipeRq, SIGNAL(changed(QString)), this, SLOT(doPipeInputRq(QString)));
    connect( &_pipePv, SIGNAL(changed(QString)), this, SLOT(doPipeInputPv(QString)));
    connect( _ui->lineEditRq, SIGNAL(returnPressed()), this, SLOT(doLineInputRq()));
    connect( _ui->lineEditPv, SIGNAL(returnPressed()), this, SLOT(doLineInputPv()));
}


TermWindow::~TermWindow()
{
    delete _ui;
}


void  TermWindow::doPipeInputRq( QString text)
{
    _ui->textEdit->setTextColor( Qt::blue);
    _ui->textEdit->append( text);
}


void  TermWindow::doPipeInputPv( QString text)
{
    _ui->textEdit->setTextColor( Qt::black);
    _ui->textEdit->append( text);
}


void  TermWindow::doLineInputRq()
{
    QString  text = _ui->lineEditRq->text();
    _ui->lineEditRq->clear();
    _pipeRq = text;
}


void  TermWindow::doLineInputPv()
{
    QString  text = _ui->lineEditPv->text();
    _ui->lineEditPv->clear();
    _pipePv = text;
}


void  TermWindow::closeEvent( QCloseEvent* event)
{
    // qDebug() << "Close event";
    writeSettings();
    event->accept();
    deleteLater();
}


void  TermWindow::readSettings()
{
    QPoint  pos = _appSettings->value("term/pos", QPoint(200, 200)).toPoint();
    QSize  size = _appSettings->value("term/size", QSize(400, 400)).toSize();
    resize( size);
    move( pos);
}


void  TermWindow::writeSettings()
{
    // qDebug() << "Write term settings";
    _appSettings->setValue("term/pos", pos());
    _appSettings->setValue("term/size", size());
}

