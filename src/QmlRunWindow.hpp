#ifndef QMLRUN_HPP
#define QMLRUN_HPP

#include "Connector.hpp"
#include <ArnInc/ArnItem.hpp>
#include <QtQuick/QQuickView>

class QSettings;
class QQuickCloseEvent;


class QmlRunWindow : public QQuickView
{
    Q_OBJECT
public:
    explicit QmlRunWindow( QSettings* appSettings, const ConnectorPath& conPath, QObject* parent = 0);

signals:

public slots:


private slots:
    void  postSetup();
    void  onClose();
    void  readSettings();
    void  writeSettings();

private:
    QSettings*  _appSettings;
    QUrl  _url;
};

#endif // QMLRUN_HPP
