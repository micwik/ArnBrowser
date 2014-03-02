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

#include "TermWindow.hpp"
#include "ui_TermWindow.h"
#include <QSettings>
#include <QCloseEvent>
#include <QDebug>


TermWindow::TermWindow( QSettings* appSettings, const QString& path, QWidget* parent) :
    QDialog( parent),
    _ui( new Ui::TermWindow)
{
    _ui->setupUi( this);
    this->setWindowTitle( QString("Terminal ") + path);

    _appSettings = appSettings;
    readSettings();

    QString  twinPath = Arn::twinPath( path);
    if (Arn::isProviderPath( path)) {
        _pipePv.open( path);
        _pipeRq.open( twinPath);
    }
    else {  // path is requester side (normal)
        _pipeRq.open( path);
        _pipePv.open( twinPath);
    }

    _ui->pipePath->setText( _pipeRq.path());
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
    qDebug() << "Close event";
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
    qDebug() << "Write term settings";
    _appSettings->setValue("term/pos", pos());
    _appSettings->setValue("term/size", size());
}

