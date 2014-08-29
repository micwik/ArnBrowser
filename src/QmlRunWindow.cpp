#include "QmlRunWindow.hpp"
#include <ArnInc/ArnItem.hpp>
#include <ArnInc/ArnQml.hpp>
#include <QQmlEngine>
#include <QString>
#include <QSettings>
#include <QFile>
#include <QTimer>
#include <QDebug>


QmlRunWindow::QmlRunWindow( QSettings* appSettings, const ConnectorPath& conPath, QObject* parent)
    : QQuickView()
{
    QObject::setParent( parent);
    this->setTitle( QString("QmlRun ") + conPath.normPath());

    _appSettings = appSettings;
    readSettings();

    ArnQml::setArnRootPath( conPath.toLocalPath("/"));
    ArnQml::setup( engine(), ArnQml::UseFlags::ArnLib | ArnQml::UseFlags::MSystem);

    connect( engine(), SIGNAL(quit()), this, SLOT(onClose()));
    connect( this, SIGNAL(closing(QQuickCloseEvent*)), this, SLOT(onClose()));
    // setResizeMode( QQuickView::SizeRootObjectToView);

    _url.setScheme("arn");
    _url.setPath( Arn::convertPath( conPath.normPath(), Arn::NameF()));

    qDebug() << "Qml running Url=" << _url.toString();
    setSource( _url);
}


void  QmlRunWindow::onClose()
{
    qDebug() << "Close event";
    writeSettings();
    deleteLater();
}


void  QmlRunWindow::readSettings()
{
    QPoint  pos = _appSettings->value("qmlRun/pos", QPoint(200, 200)).toPoint();
    //QSize  size = _appSettings->value("qmlRun/size", QSize(400, 400)).toSize();
    //resize( size);
    setPosition( pos);
}


void  QmlRunWindow::writeSettings()
{
    qDebug() << "Write code settings";
    _appSettings->setValue("qmlRun/pos", position());
    //_appSettings->setValue("qmlRun/size", size());
}
