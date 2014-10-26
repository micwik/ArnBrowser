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
