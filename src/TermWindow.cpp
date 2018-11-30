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

#include "TermWindow.hpp"
#include "ui_TermWindow.h"
#include <QInputDialog>
#include <QSettings>
#include <QKeyEvent>
#include <QKeySequence>
#include <QCloseEvent>
#include <QApplication>
#include <QDebug>


TermWindow::History::History()
{
    cur = -1;
}


void  TermWindow::History::addEntry( const QString& text)
{
    if (!text.isEmpty()) {
        if (list.isEmpty() || (text != list.first())) {
            list.prepend( text);
        }
    }
    cur = -1;
}



TermWindow::TermWindow( QSettings* appSettings, const ConnectorPath& conPath, QWidget* parent)
    : QDialog( parent),
    _ui( new Ui::TermWindow)
{
    _ui->setupUi( this);

    QString  path = conPath.localPath();
    QString  rqPath = Arn::providerPathIf( path, false);

    QString  normPath = conPath.toNormPath( rqPath);
    QString  curHost  = conPath.curHost();
    this->setWindowTitle( QString("Terminal ") + normPath + " @" + curHost);

    _appSettings = appSettings;
    readSettings();

    _pipeRq.open( rqPath);
    _pipePv.open( Arn::twinPath( rqPath));

    _ui->pipePath->setText( normPath);
    _ui->pipePath->setReadOnly( true);
    _ui->textEdit->setReadOnly( true);
    _ui->clearButton->clearFocus();
    _ui->lineEditRq->setFocus();

    connect( &_pipeRq, SIGNAL(arnLinkDestroyed()), this, SLOT(deleteLater()));
    connect( &_pipeRq, SIGNAL(changed(QString)), this, SLOT(doPipeInputRq(QString)));
    connect( &_pipePv, SIGNAL(changed(QString)), this, SLOT(doPipeInputPv(QString)));
    connect( _ui->lineEditRq, SIGNAL(returnPressed()), this, SLOT(doLineInputRq()));
    connect( _ui->lineEditPv, SIGNAL(returnPressed()), this, SLOT(doLineInputPv()));
}


TermWindow::~TermWindow()
{
    delete _ui;
}


void  TermWindow::doPipeInputRq( QString text)
{
    _ui->textEdit->setTextColor( Qt::blue);
    appendText(text);
}


void  TermWindow::doPipeInputPv( QString text)
{
    _ui->textEdit->setTextColor( Qt::black);
    appendText(text);
}


void  TermWindow::doLineInputRq()
{
    QString  text = _ui->lineEditRq->text();
    _ui->lineEditRq->clear();
    _pipeRq = text;
    _historyRq.addEntry( text);
}


void  TermWindow::doLineInputPv()
{
    QString  text = _ui->lineEditPv->text();
    _ui->lineEditPv->clear();
    _pipePv = text;
    _historyPv.addEntry( text);
}


void  TermWindow::on_clearButton_clicked()
{
    // qDebug() << "ClearButton!";
    _ui->textEdit->clear();
    _nrRows = 0;
}


void  TermWindow::keyPressEvent( QKeyEvent* ev)
{
    // qDebug() << "KeyPressEvent: keyCode=" << ev->key();
    QWidget*  widget = QApplication::focusWidget();

    switch (ev->key() | ev->modifiers()) {
    case Qt::Key_Up:
    {
        // qDebug() << "KeyPressEvent: key=Up";
        ev->accept();

        History*  hi = selHistory( widget);
        if (hi && (hi->cur + 1 < hi->list.size())) {
            //// History backwards (older)
            ++hi->cur;

            QLineEdit*  lineEdit = qobject_cast<QLineEdit*>( widget);
            Q_ASSERT(lineEdit);
            lineEdit->setText( hi->list.at( hi->cur));
            break;
        }
        break;
    }
    case Qt::Key_Down:
    {
        // qDebug() << "KeyPressEvent: key=Down";
        ev->accept();

        History*  hi = selHistory( widget);
        if (hi && (hi->cur - 1 >= -1)) {
            //// History forwards (newer)
            --hi->cur;

            QLineEdit*  lineEdit = qobject_cast<QLineEdit*>( widget);
            Q_ASSERT(lineEdit);
            QString  text = hi->cur < 0 ? QString() : hi->list.at( hi->cur);
            lineEdit->setText( text);
            break;
        }
        break;
    }
    case Qt::ControlModifier + Qt::Key_F:
    {   //// Find
        // qDebug() << "KeyPressEvent: key=Ctrl-F";
        ev->accept();

        QTextCursor  textCursor = _ui->textEdit->textCursor();
        QString  text = textCursor.selectedText();
        bool  isOk;
        text = QInputDialog::getText(this, "Find", "Text:", QLineEdit::Normal, text, &isOk);
        if (isOk && !text.isEmpty()) {
            _lastFind = text;
            if (!_ui->textEdit->find( _lastFind)) {
                _ui->textEdit->moveCursor( QTextCursor::Start);
                _ui->textEdit->find( _lastFind);
            }
        }
        break;
    }
    case Qt::Key_F3:
    {   //// Find next
        // qDebug() << "KeyPressEvent: key=F3";
        ev->accept();

        if (!_ui->textEdit->find( _lastFind)) {
            _ui->textEdit->moveCursor( QTextCursor::Start);
            _ui->textEdit->find( _lastFind);
        }
        break;
    }
    case Qt::ShiftModifier + Qt::Key_F3:
    {   //// Find previous
        // qDebug() << "KeyPressEvent: key=Shift-F3";
        ev->accept();
        if (!_ui->textEdit->find( _lastFind, QTextDocument::FindBackward)) {
            _ui->textEdit->moveCursor( QTextCursor::End);
            _ui->textEdit->find( _lastFind, QTextDocument::FindBackward);
        }
        break;
    }
    default:
        QDialog::keyPressEvent( ev);
        break;
    }
}


void  TermWindow::closeEvent( QCloseEvent* event)
{
    // qDebug() << "Close event";
    writeSettings();
    event->accept();
    deleteLater();
}


void  TermWindow::readSettings()
{
    QPoint  pos = _appSettings->value("term/pos", QPoint(200, 200)).toPoint();
    QSize  size = _appSettings->value("term/size", QSize(400, 400)).toSize();
    _maxRows = _appSettings->value("set/maxRows", QVariant()).toInt();
    resize( size);
    move( pos);
}


void  TermWindow::writeSettings()
{
    // qDebug() << "Write term settings";
    _appSettings->setValue("term/pos", pos());
    _appSettings->setValue("term/size", size());
}


void TermWindow::appendText(QString text)
{
    QTextCursor cursor;
    if (_nrRows > _maxRows) {
        _ui->textEdit->setReadOnly( false);
        cursor = _ui->textEdit->textCursor();
        cursor.movePosition( QTextCursor::Start );
        cursor.movePosition( QTextCursor::Down, QTextCursor::KeepAnchor, _maxRows / 10 );
        _nrRows = _nrRows - _maxRows / 10;
        cursor.removeSelectedText();
        cursor.movePosition( QTextCursor::End );
        _ui->textEdit->setTextCursor(cursor);
        _ui->textEdit->setReadOnly( true);
    }
    _ui->textEdit->append( text);
    _nrRows++;
    _ui->rowsLabel->setText(QString::number(_nrRows));
}


TermWindow::History*  TermWindow::selHistory( const QWidget* widget)
{
    if (widget == _ui->lineEditRq)  return &_historyRq;
    if (widget == _ui->lineEditPv)  return &_historyPv;

    return 0;
}
