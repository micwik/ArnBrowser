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

#ifndef SETTINGSHANDLER_HPP
#define SETTINGSHANDLER_HPP

#include <QString>

class QSettings;


class SettingsHandler
{
public:
    struct  Data {
        QString  userName;
        QString  contact;
        QString  location;
        QString  maxRows;
    };

    SettingsHandler( QSettings* appSettings);
    QSettings*  appSettings()  const;
    void  readSettings();
    void  writeSettings();

    Data  d;

private:
    QSettings*  _appSettings;
};

#endif // SETTINGSHANDLER_HPP
