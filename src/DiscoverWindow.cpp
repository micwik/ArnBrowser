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

#include "DiscoverWindow.hpp"
#include "ui_DiscoverWindow.h"
#include <QInputDialog>
#include <QCloseEvent>
#include <QSettings>
#include <QDebug>


DiscoverWindow::DiscoverWindow( QSettings* appSettings, QWidget* parent) :
    QDialog( parent),
    _ui( new Ui::DiscoverWindow)
{
    _ui->setupUi( this);

    this->setWindowTitle( QString("Discover ARN"));

    _appSettings = appSettings;
    readSettings();

    //_filesUpdated = false;
    //_refUpdated   = false;

    //// Logics

    connect( _ui->arnTypeSelBox, SIGNAL(clicked()), this, SLOT(onTreeChanged()));
    //connect( _ui->treeRepoButton, SIGNAL(clicked()), this, SLOT(onTreeChanged()));

    doUpdate();
}


DiscoverWindow::~DiscoverWindow()
{
    delete _ui;
}


void  DiscoverWindow::doUpdate()
{
    //_ui->commitButton->setDisabled( !_ui->treeWorkButton->isChecked());
/*
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
*/
}

/*
QStringList  DiscoverWindow::getSelFiles()
{
    QList<QListWidgetItem*>  selItems = _ui->filesView->selectedItems();
    QStringList  selFiles;
    foreach (QListWidgetItem* item, selItems) {
        selFiles += item->text();
    }
    return selFiles;
}
*/

void  DiscoverWindow::on_reLoadButton_clicked()
{
}


void  DiscoverWindow::onTreeChanged()
{
    //_ui->filesView->clear();
    doUpdate();
}


void  DiscoverWindow::closeEvent( QCloseEvent* event)
{
    qDebug() << "Close event";
    writeSettings();
    event->accept();
    deleteLater();
}


void  DiscoverWindow::readSettings()
{
    QPoint      pos   = _appSettings->value("discover/pos", QPoint(200, 200)).toPoint();
    QSize       size  = _appSettings->value("discover/size", QSize(500, 400)).toSize();
    // QByteArray  split = _appSettings->value("discover/split", _ui->splitter->saveState()).toByteArray();
    resize( size);
    move( pos);
    //_ui->splitter->restoreState( split);
}


void  DiscoverWindow::writeSettings()
{
    qDebug() << "Write code settings";
    _appSettings->setValue("discover/pos", pos());
    _appSettings->setValue("discover/size", size());
    //_appSettings->setValue("vcs/split", _ui->splitter->saveState());
}
