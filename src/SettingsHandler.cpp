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

#include "SettingsHandler.hpp"
#include <QSettings>


SettingsHandler::SettingsHandler( QSettings* appSettings)
{
    _appSettings = appSettings;
}


QSettings*  SettingsHandler::appSettings()  const
{
    return _appSettings;
}


void  SettingsHandler::readSettings()
{
    d.userName = _appSettings->value("set/userName", QString()).toString();
    d.contact  = _appSettings->value("set/contact",  QString()).toString();
    d.location = _appSettings->value("set/location", QString()).toString();
    d.font     = _appSettings->value("set/font",     QString()).toString();
    d.maxRows  = _appSettings->value("set/maxRows",  10000).toInt();
}


void  SettingsHandler::writeSettings()
{
    _appSettings->setValue("set/userName", d.userName);
    _appSettings->setValue("set/contact",  d.contact);
    _appSettings->setValue("set/location", d.location);
    _appSettings->setValue("set/font",     d.font);
    _appSettings->setValue("set/maxRows",  d.maxRows);
}

