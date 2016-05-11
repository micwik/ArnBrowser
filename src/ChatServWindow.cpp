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

#include "ChatServWindow.hpp"
#include "ui_ChatServWindow.h"
#include <QSettings>
#include <QCloseEvent>
#include <QDebug>


ChatServWindow::ChatServWindow( QSettings* appSettings, QWidget* parent)
    : QDialog( parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint),
    _ui( new Ui::ChatServWindow)
{
    _ui->setupUi( this);

    _appSettings = appSettings;
    readSettings();

    _ui->textEdit->setReadOnly( true);
    _ui->lineEdit->setFocus();
    reset();

    connect( _ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(doLineInput()));
}


ChatServWindow::~ChatServWindow()
{
    delete _ui;
}


void  ChatServWindow::reset()
{
    _ui->textEdit->clear();
}


void  ChatServWindow::on_closeButton_clicked()
{
    setVisible( false);
}


void  ChatServWindow::on_clearButton_clicked()
{
    _ui->textEdit->clear();
}


void  ChatServWindow::on_abortKillButton_clicked()
{
    // qDebug() << "AbortButton clicked";
    emit abortKillRequest();
}


void  ChatServWindow::addTxtNormal( const QString& text)
{
    _ui->textEdit->setTextColor( Qt::blue);
    _ui->textEdit->append( text);
}


void  ChatServWindow::addTxtPrio( const QString& text)
{
    _ui->textEdit->setTextColor( Qt::red);
    _ui->textEdit->append( text);

    setVisible( true);
}


void ChatServWindow::addTxtLocal(const QString& text)
{
    _ui->textEdit->setTextColor( Qt::black);
    _ui->textEdit->append( text);
}


void  ChatServWindow::doLineInput()
{
    QString  text = _ui->lineEdit->text();
    _ui->lineEdit->clear();

    addTxtLocal( text);
    emit txtOutput( text);
}


void  ChatServWindow::closeEvent( QCloseEvent* event)
{
    // qDebug() << "Close event ChatServWindow";
    writeSettings();
    event->accept();
}


void  ChatServWindow::readSettings()
{
    QPoint  pos = _appSettings->value("chatServ/pos", QPoint(200, 200)).toPoint();
    QSize  size = _appSettings->value("chatServ/size", QSize(400, 400)).toSize();
    resize( size);
    move( pos);
}


void  ChatServWindow::writeSettings()
{
    // qDebug() << "Write chatServ settings";
    _appSettings->setValue("chatServ/pos", pos());
    _appSettings->setValue("chatServ/size", size());
}
