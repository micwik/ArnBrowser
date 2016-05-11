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
#ifndef QMLRUN_HPP
#define QMLRUN_HPP

#include "Connector.hpp"
#include <ArnInc/ArnItem.hpp>

#if QT_VERSION >= 0x050000
#  include <QtQuick/QQuickView>
#  include <QQmlEngine>
#  define QML_VIEW  QQuickView
#  define QML_SET_TITLE     setTitle
#  define QML_SET_POS       setPosition
#  define QML_POS           position
#else
#  include <QDeclarativeView>
#  include <QDeclarativeEngine>
#  include <QCloseEvent>
#  define QML_Qt4
#  define QML_VIEW          QDeclarativeView
#  define QML_SET_TITLE     setWindowTitle
#  define QML_SET_POS       move
#  define QML_POS           pos
#endif

class QSettings;
class QQuickCloseEvent;


class QmlRunWindow : public QML_VIEW
{
    Q_OBJECT
public:
    explicit QmlRunWindow( QSettings* appSettings, const ConnectorPath& conPath, QObject* parent = 0);
    ~QmlRunWindow();

signals:

public slots:


protected:
#ifdef QML_Qt4
    void closeEvent( QCloseEvent *event);
#endif

private slots:
    void  onClose();
    void  readSettings();
    void  writeSettings();

private:
    QSettings*  _appSettings;
    QUrl  _url;
};

#endif // QMLRUN_HPP
