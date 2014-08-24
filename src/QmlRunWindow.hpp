#ifndef QMLRUN_HPP
#define QMLRUN_HPP

#include "Connector.hpp"
#include <ArnInc/ArnItem.hpp>
#include <QtQuick/QQuickView>

class QSettings;
class QCloseEvent;


class QmlRunWindow : public QQuickView
{
    Q_OBJECT
public:
    explicit QmlRunWindow( QSettings* appSettings, const ConnectorPath& conPath, QObject* parent = 0);

signals:

public slots:

protected:
    void  closeEvent( QCloseEvent* event);

private slots:
    void  readSettings();
    void  writeSettings();

private:
    QSettings*  _appSettings;
};

#endif // QMLRUN_HPP
