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
}


void  SettingsHandler::writeSettings()
{
    _appSettings->setValue("set/userName", d.userName);
    _appSettings->setValue("set/contact",  d.contact);
    _appSettings->setValue("set/location", d.location);
}

