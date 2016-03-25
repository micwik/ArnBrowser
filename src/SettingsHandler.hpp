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
