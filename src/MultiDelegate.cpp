// Copyright (C) 2010-2013 Michael Wiklund.
// All rights reserved.
// Contact: arnlib@wiklunden.se
//
// This file is part of the ArnBrowser - Active Registry Network Browser.
// Parts of ArnBrowser depend on Qt 4 and/or other libraries that have their own
// licenses. ArnBrowser is independent of these licenses; however, use of these other
// libraries is subject to their respective license agreements.
//
// GNU Lesser General Public License Usage
// This file may be used under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation and
// appearing in the file LICENSE.LGPL included in the packaging of this file.
// In addition, as a special exception, you may use the rights described
// in the Nokia Qt LGPL Exception version 1.1, included in the file
// LGPL_EXCEPTION.txt in this package.
//
// GNU General Public License Usage
// Alternatively, this file may be used under the terms of the GNU General
// Public License version 3.0 as published by the Free Software Foundation
// and appearing in the file LICENSE.GPL included in the packaging of this file.
//
// Other Usage
// Alternatively, this file may be used in accordance with the terms and
// conditions contained in a signed written agreement between you and Michael Wiklund.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//

#include "MultiDelegate.hpp"
#include "ItemDataRole.hpp"
#include "Math.hpp"
#include <QStringList>
#include <QVariant>
#include <QDateEdit>
#include <QCalendarWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QIcon>
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QRect>
#include <QGridLayout>
#include <QDebug>


PixmapViewer::PixmapViewer( QWidget* parent)
    : QDialog( parent)
{
    _img = new QLabel;
    QGridLayout*  imgLayout = new QGridLayout;
    imgLayout->addWidget( _img, 0, 0);

    //QFrame*  frame = new QFrame;
    //frame->setLayout( imgLayout);
    //QVBoxLayout*  vLayout = new QVBoxLayout;
    //vLayout->addWidget( frame);
    //this->setLayout( vLayout);

    this->setLayout( imgLayout);
    this->resize(0, 0);  // Minimize size of viewer
    this->setWindowTitle("Image Viewer");

    show();
}


void  PixmapViewer::setPixmap( const QPixmap& pixmap)
{
    qDebug() << "PixmapViewer setPixmap";

    _img->setPixmap( pixmap);
    this->resize(0, 0);  // Minimize size of viewer
    qDebug() << "pixMapSize=" << pixmap.size();
}


MultiDelegate::MultiDelegate( QObject* parent)
    : QItemDelegate( parent)
{
}


void  MultiDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index)  const
{
    QVariant  value = index.model()->data( index, Qt::EditRole);
    //qDebug() << "Paint dataType=" << value.typeName();
    switch (value.type()) {
    case QMetaType::QImage:
        {
            QImage  img = value.value<QImage>();
            return paintPixmap( painter, option, QPixmap::fromImage(img));
        }
        case QMetaType::QPixmap:
        {
            QPixmap  pixmap = value.value<QPixmap>();
            return paintPixmap( painter, option, pixmap);
        }
        case QMetaType::QIcon:
        {
            //qDebug() << "Paint Icon:";
            QIcon  icon = value.value<QIcon>();
            QPixmap  pixmap = icon.pixmap( option.rect.size(), QIcon::Mode(0));
            return paintPixmap( painter, option, pixmap);
        }
    default:
        return QItemDelegate::paint( painter, option, index);
    }
}


void  MultiDelegate::paintPixmap( QPainter* painter, const QStyleOptionViewItem& option, const QPixmap& pixmap)  const
{
    painter->save();

    QRect  rect = option.rect;
    rect.setWidth( 30 * pixmap.width() / pixmap.height());
    painter->drawPixmap( rect, pixmap);
    if (option.state & QStyle::State_Selected) {
        painter->setPen( QColor(0,0,255));
        painter->drawRect( rect);
    }
    painter->restore();
}


QSize  MultiDelegate::sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index)  const
{
    QVariant  value = index.model()->data( index, Qt::EditRole);
    switch (value.type()) {
        case QMetaType::QImage:
            // Fall throu
        case QMetaType::QPixmap:
            // Fall throu
        case QMetaType::QIcon:
        {
            return QSize(45, 30);
        }
    default:
        return QItemDelegate::sizeHint( option, index);
    }
}


QWidget*  MultiDelegate::createEditor( QWidget* parent,
                                     const QStyleOptionViewItem& option,
                                     const QModelIndex& index)  const
{
    QVariant  value = index.model()->data( index, Qt::EditRole);
    switch (value.type()) {
    case QMetaType::QTime: {
        QTimeEdit*  editor = new QTimeEdit( parent);
        editor->setMaximumWidth( editor->sizeHint().width());
        return editor;
    }
    case QMetaType::QDate: {
        QDateEdit*  editor = new QDateEdit( parent);
        setupCalenderWidget( editor);
        editor->setMaximumWidth( editor->sizeHint().width());
        return editor;
    }
    case QMetaType::QDateTime: {
        QDateTimeEdit*  editor = new QDateTimeEdit( parent);
        setupCalenderWidget( editor);
        editor->setMaximumWidth( editor->sizeHint().width());
        return editor;
    }
    case QMetaType::QImage:
        // Fall throu
    case QMetaType::QPixmap:
        // Fall throu
    case QMetaType::QIcon:
    {
        PixmapViewer*  editor = new PixmapViewer( parent);
        return editor;
    }
    case QMetaType::QString: {
        QVariant  varList = index.model()->data( index, ItemDataRole::EnumList);
        if (varList.isNull())  break;  // Not a enum-list, fall to std

        QComboBox*  editor = new QComboBox( parent);
        editor->setSizeAdjustPolicy(QComboBox::AdjustToContents);
        editor->addItems( varList.toStringList());
        editor->setMaximumWidth( editor->minimumSizeHint().width());
        return editor;
    }
    default:;
    }

    return QItemDelegate::createEditor( parent, option, index);
}


void  MultiDelegate::setupCalenderWidget( QDateTimeEdit* editor)  const
{
    editor->setCalendarPopup(true);
    // editor->setMinimumDate( QDate::currentDate());  // MW: Kolla ...
    QCalendarWidget*  calendar = editor->calendarWidget();
    if (calendar) {
        calendar->setFirstDayOfWeek(Qt::Monday);
        calendar->setVerticalHeaderFormat( QCalendarWidget::ISOWeekNumbers);
    }
}


void  MultiDelegate::setEditorData( QWidget* editor,
                                    const QModelIndex& index)  const
{
    QVariant value = index.model()->data( index, Qt::EditRole);

    QString  className = editor->metaObject()->className();
    if (className == "QTimeEdit") {
        QTimeEdit*  ed = qobject_cast<QTimeEdit*>(editor);
        Q_ASSERT( ed);
        ed->setTime( value.toTime());
    }
    else if (className == "QDateEdit") {
        QDateEdit*  ed = qobject_cast<QDateEdit*>(editor);
        Q_ASSERT( ed);
        ed->setDate( value.toDate());
    }
    else if (className == "QDateTimeEdit") {
        QDateTimeEdit*  ed = qobject_cast<QDateTimeEdit*>(editor);
        Q_ASSERT( ed);
        ed->setDateTime( value.toDateTime());
    }
    else if (className == "PixmapViewer") {
        PixmapViewer*  ed = qobject_cast<PixmapViewer*>(editor);
        Q_ASSERT( ed);
        QPixmap  pixmap;
        if (value.type() == QVariant::Image) {
            pixmap = QPixmap::fromImage( value.value<QImage>());
        }
        else if (value.type() == QVariant::Pixmap) {
            pixmap = value.value<QPixmap>();
        }
        else {
            QIcon  icon( value.value<QIcon>());
            pixmap = icon.pixmap( QSize( 640, 480), QIcon::Normal, QIcon::Off);
        }
        ed->setPixmap( pixmap);
    }
    else if (className == "QComboBox") {
        QComboBox*  ed = qobject_cast<QComboBox*>(editor);
        Q_ASSERT( ed);
        ed->setCurrentIndex( ed->findText( value.toString()));
    }
    else if (className == "QCheckBox") {
        QCheckBox*  ed = qobject_cast<QCheckBox*>(editor);
        Q_ASSERT( ed);
        ed->setChecked( value.Bool);
    }
    else {
        QItemDelegate::setEditorData( editor, index);
        return;
    }
}


void MultiDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    QVariant value;

    QString  className = editor->metaObject()->className();
    if (className == "QTimeEdit") {
        QTimeEdit*  ed = qobject_cast<QTimeEdit*>(editor);
        Q_ASSERT( ed);
        value = QVariant( ed->time());
    }
    else if (className == "QDateEdit") {
        QDateEdit*  ed = qobject_cast<QDateEdit*>(editor);
        Q_ASSERT( ed);
        value = QVariant( ed->date());
    }
    else if (className == "QDateTimeEdit") {
        QDateTimeEdit*  ed = qobject_cast<QDateTimeEdit*>(editor);
        Q_ASSERT( ed);
        value = QVariant( ed->dateTime());
    }
    else if (className == "IconViewer") {
        return;
    }
    else if (className == "QComboBox") {
        QComboBox*  ed = qobject_cast<QComboBox*>(editor);
        Q_ASSERT( ed);
        value = QVariant( ed->currentText());
    }
    else if (className == "QCheckBox") {
        QCheckBox*  ed = qobject_cast<QCheckBox*>(editor);
        Q_ASSERT( ed);
        value = QVariant( ed->isChecked());
    }
    else {
        QItemDelegate::setModelData( editor, model, index);
        return;
    }
    model->setData(index, value, Qt::EditRole);
}


void MultiDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED( index);
    editor->setGeometry(option.rect);
}
