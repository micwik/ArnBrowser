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

#include "LogWindow.hpp"
#include "ui_LogWindow.h"
#include <QSettings>
#include <QCloseEvent>
#include <QFileDialog>
#include <QDesktopServices>
#include <QDir>
#include <QDateTime>
#include <QDebug>


LogWindow::LogWindow(QSettings *appSettings, const ConnectorPath &conPath, QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::LogWindow)
{
    _ui->setupUi(this);

    _logLines = 0;

    QString  path = conPath.localPath();
    QString  rqPath = Arn::providerPathIf( path, false);

    QString  normPath = conPath.toNormPath( rqPath);
    QString  curHost  = conPath.curHost();
    this->setWindowTitle( QString("Log ") + normPath + " @" + curHost);

    _appSettings = appSettings;
    readSettings();

    _pipeRq.open( rqPath);
    _pipePv.open( Arn::twinPath( rqPath));

    _ui->pipePath->setText( normPath);
    _ui->pipePath->setReadOnly( true);
    _ui->lines->setReadOnly( true);
    _ui->fileNameEdit->setFocus();

    connect( &_pipeRq, SIGNAL(arnLinkDestroyed()), this, SLOT(deleteLater()));
    connect( &_pipeRq, SIGNAL(changed(QString)), this, SLOT(doPipeInputRq(QString)));
    connect( &_pipePv, SIGNAL(changed(QString)), this, SLOT(doPipeInputPv(QString)));
}


LogWindow::~LogWindow()
{
    delete _ui;
}


void LogWindow::doPipeInputRq(QString text)
{
    doWriteToLog( text);
}


void LogWindow::doPipeInputPv(QString text)
{
    if (!_logFile.isOpen())  return;

    _logFile.write( text.toUtf8());
}


void LogWindow::on_browseButton_clicked()
{
    qDebug() <<"Button file ...";
    QFileDialog fileDialog(this);
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setDefaultSuffix("txt");
    fileDialog.setNameFilter(tr("Output (*.*)"));
    fileDialog.setWindowTitle("Log file");

    qDebug() << "dirPath=" << _fileDir;
    if (!_fileDir.isNull())
        fileDialog.setDirectory(_fileDir);

    if (!fileDialog.exec())  return;  // File load cancel

    QString  fileName = fileDialog.selectedFiles().first();

    qDebug() << "selected file=" << fileName;
    QFileInfo fileInfo(fileName);
    _ui->fileNameEdit->setText( fileName);

    _fileDir = fileInfo.absolutePath();
}


void  LogWindow::doWriteToLog( const QString& text)
{
    if (!_logFile.isOpen())  return;

    _logFile.write( text.toUtf8() + "\n");

    ++_logLines;
    _ui->lines->setText( QString::number( _logLines));
}


void LogWindow::on_startLogButton_clicked()
{
    QString  fileName = _ui->fileNameEdit->text();
    if (_logFile.isOpen())
        _logFile.close();

    _logFile.setFileName( fileName);
    if (!_logFile.open( QIODevice::WriteOnly | QIODevice::Text))
        return;

    _logLines = 0;
    _ui->lines->setText("0");
}


void  LogWindow::closeEvent( QCloseEvent* event)
{
    // qDebug() << "Close event";
    if (_logFile.isOpen())
        _logFile.close();

    writeSettings();
    event->accept();
    deleteLater();
}


void LogWindow::readSettings()
{
    QString  fn = _appSettings->value("log/fileName").toString();
    _fileDir    = _appSettings->value("log/dirPath").toString();
    QPoint  pos = _appSettings->value("log/pos", QPoint(200, 200)).toPoint();
    QSize  size = _appSettings->value("log/size", QSize(800, 250)).toSize();
    _ui->fileNameEdit->setText( fn);
    resize( size);
    move( pos);
}


void LogWindow::writeSettings()
{
    // qDebug() << "Write log settings";
    _appSettings->setValue("log/fileName", _ui->fileNameEdit->text());
    _appSettings->setValue("log/dirPath", _fileDir);
    _appSettings->setValue("log/pos", pos());
    _appSettings->setValue("log/size", size());
}
