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

#ifndef CONNECTOR_HPP
#define CONNECTOR_HPP

#include <QObject>
#include <QString>
#include <QPointer>


class Connector : public QObject
{
    Q_OBJECT
public:
    explicit Connector( QObject* parent = 0);

    QString  toNormPath( const QString& path)  const;  // Normal path, also as remote path and viewed path
    QString  toLocalPath( const QString& path)  const;

private:
    const QString  _hostRootPath;
};


class ConnectorPath
{
public:
    explicit ConnectorPath( Connector* connector = 0, QString localPath = QString());

    QString  normPath()  const;  // Normal path, also as remote path and viewed path
    QString  localPath()  const;
    QString  toNormPath( const QString& path)  const;  // Normal path, also as remote path and viewed path
    QString  toLocalPath( const QString& path)  const;

private:
    QPointer<Connector>  _connector;
    QString  _localPath;
};

#endif // CONNECTOR_HPP
