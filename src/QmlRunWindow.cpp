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

#include "QmlRunWindow.hpp"
#include <ArnInc/ArnItem.hpp>
#include <ArnInc/ArnQml.hpp>
#include <QString>
#include <QSettings>
#include <QFile>
#include <QTimer>
#include <QDebug>


QmlRunWindow::QmlRunWindow( QSettings* appSettings, const ConnectorPath& conPath, QObject* parent)
    : QML_VIEW()
{
    Q_UNUSED(parent)

    this->QML_SET_TITLE( QString("QmlRun ") + conPath.normPath());

    _appSettings = appSettings;
    readSettings();

    ArnQml::setArnRootPath( conPath.toLocalPath("/"));
    ArnQml::setup( engine(), ArnQml::UseFlags::All);

    connect( engine(), SIGNAL(quit()), this, SLOT(onClose()));
#ifndef QML_Qt4
    connect( this, SIGNAL(closing(QQuickCloseEvent*)), this, SLOT(onClose()));
#endif
    // setResizeMode( QQuickView::SizeRootObjectToView);

    _url.setScheme("arn");
    _url.setPath( Arn::convertPath( conPath.normPath(), Arn::NameF()));

    qDebug() << "Qml running Url=" << _url.toString();
    setSource( _url);
}


QmlRunWindow::~QmlRunWindow()
{
    // qDebug() << "Destruct QmlRunWindow";
}


#ifdef QML_Qt4
void QmlRunWindow::closeEvent(QCloseEvent* event)
{
    Q_UNUSED(event)

    onClose();
}
#endif


void  QmlRunWindow::onClose()
{
    // qDebug() << "Close event";
    writeSettings();
    deleteLater();
}


void  QmlRunWindow::readSettings()
{
    QPoint  pos = _appSettings->value("qmlRun/pos", QPoint(200, 200)).toPoint();
    //QSize  size = _appSettings->value("qmlRun/size", QSize(400, 400)).toSize();
    //resize( size);
    QML_SET_POS( pos);
}


void  QmlRunWindow::writeSettings()
{
    // qDebug() << "Write qmlRun settings";
    _appSettings->setValue("qmlRun/pos", QML_POS());
    //_appSettings->setValue("qmlRun/size", size());
}
