// Copyright (C) 2010-2016 Michael Wiklund.
// All rights reserved.
// Contact: arnlib@wiklunden.se
//
// This file is part of the ArnLib - Active Registry Network.
// Parts of ArnLib depend on Qt and/or other libraries that have their own
// licenses. Usage of these other libraries is subject to their respective
// license agreements.
//
// The MIT License (MIT) Usage
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this file to deal in its contained Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software in this file.
//
// GNU General Public License Usage
// Alternatively, this file may be used under the terms of the GNU General Public
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

    QString curHost() const;
    void setCurHost(const QString &curHost);

private:
    const QString  _hostRootPath;
    QString _curHost;
};


class ConnectorPath
{
public:
    explicit ConnectorPath( Connector* connector = 0, QString localPath = QString());

    QString  normPath()  const;  // Normal path, also as remote path and viewed path
    QString  localPath()  const;
    QString  toNormPath( const QString& path)  const;  // Normal path, also as remote path and viewed path
    QString  toLocalPath( const QString& path)  const;
    QString  curHost()  const;

private:
    QPointer<Connector>  _connector;
    QString  _localPath;
};

#endif // CONNECTOR_HPP
