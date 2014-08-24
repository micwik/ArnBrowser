#include "QmlRunWindow.hpp"
#include <ArnInc/ArnItem.hpp>
#include <ArnInc/ArnQml.hpp>
#include <QQmlEngine>
#include <QString>
#include <QSettings>
#include <QCloseEvent>
#include <QFile>
#include <QDebug>


QmlRunWindow::QmlRunWindow( QSettings* appSettings, const ConnectorPath& conPath, QObject* parent)
    : QQuickView()
{
    QObject::setParent( parent);
    //this->setWindowTitle( QString("CodeEdit ") + conPath.normPath());

    _appSettings = appSettings;
    readSettings();

    ArnQml::setArnRootPath( conPath.toLocalPath("/"));
    QString  path = conPath.localPath();
    ArnItem  arnItem( path);
    QFile file("tmp.qml");
    if (!file.open(QFile::WriteOnly | QFile::Truncate))
        return;

    file.write( arnItem.toByteArray());
    file.close();

    connect( engine(), SIGNAL(quit()), this, SLOT(close()));
    setResizeMode( QQuickView::SizeRootObjectToView);

    setSource( QUrl::fromLocalFile("tmp.qml"));
}


void  QmlRunWindow::closeEvent( QCloseEvent* event)
{
    qDebug() << "Close event";
    writeSettings();
    event->accept();
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
