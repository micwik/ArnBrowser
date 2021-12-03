// Copyright (C) 2010-2016 Michael Wiklund.
// All rights reserved.
// Contact: arnlib@wiklunden.se
//
// This file is part of the ArnLib - Active Registry Network.
// Parts of ArnLib depend on Qt and/or other libraries that have their own
// licenses. Usage of these other libraries is subject to their respective
// license agreements.
//
// The MIT License (MIT) Usage
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this file to deal in its contained Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software in this file.
//
// GNU General Public License Usage
// Alternatively, this file may be used under the terms of the GNU General Public
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

#include "MultiDelegate.hpp"
#include "ItemDataRole.hpp"
#include "Math.hpp"
#include <QStringList>
#include <QVariant>
#include <QDateEdit>
#include <QCalendarWidget>
#include <QComboBox>
#include <QListWidget>
#include <QListWidgetItem>
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
#include <QVBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QApplication>
#include <QDebug>


PixmapViewer::PixmapViewer( QWidget* parent)
    : QDialog( parent)
{
    _img = new QLabel;
    QGridLayout*  imgLayout = new QGridLayout;
    imgLayout->addWidget( _img, 0, 0);

    this->setLayout( imgLayout);
    this->resize(0, 0);  // Minimize size of viewer
    this->setWindowTitle("Image Viewer");

    show();
}


void  PixmapViewer::setPixmap( const QPixmap& pixmap)
{
    // qDebug() << "PixmapViewer setPixmap";

    _img->setPixmap( pixmap);
    this->resize(0, 0);  // Minimize size of viewer
    // qDebug() << "pixMapSize=" << pixmap.size();
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
    const QAbstractItemModel*  model = index.model();
    QVariant  value = model->data( index, Qt::EditRole);
    switch (value.type()) {
    case QMetaType::QTime:
    {
        QTimeEdit*  editor = new QTimeEdit( parent);
        editor->setMaximumWidth( editor->sizeHint().width());

        //// Get value snapshot into editor
        editor->setTime( value.toTime());
        return editor;
    }
    case QMetaType::QDate:
    {
        QDateEdit*  editor = new QDateEdit( parent);
        setupCalenderWidget( editor);
        editor->setMaximumWidth( editor->sizeHint().width());

        //// Get value snapshot into editor
        editor->setDate( value.toDate());
        return editor;
    }
    case QMetaType::QDateTime:
    {
        QDateTimeEdit*  editor = new QDateTimeEdit( parent);
        setupCalenderWidget( editor);
        editor->setMaximumWidth( editor->sizeHint().width());

        editor->setDateTime( value.toDateTime());
        return editor;
    }
    case QMetaType::QImage:
        // Fall throu
    case QMetaType::QPixmap:
        // Fall throu
    case QMetaType::QIcon:
    {
        PixmapViewer*  editor = new PixmapViewer( parent);
        connect( editor, SIGNAL(finished(int)), this, SLOT(closeEmittingEditor()));
        return editor;
    }
    case QMetaType::QStringList:
    {
        QWidget*  editor = createBitSetEditor( parent, index, value);
        if (!editor)  break;
        return editor;
    }
    case QMetaType::QString:
    {
        QVariant  varList = index.model()->data( index, ItemDataRole::EnumList);
        if (varList.isNull())  break;  // Not a enum-list, fall to std

        QComboBox*  editor = new QComboBox( parent);
        editor->setSizeAdjustPolicy(QComboBox::AdjustToContents);
        editor->addItems( varList.toStringList());
        editor->setMaximumWidth( editor->minimumSizeHint().width());

        //// Get value snapshot into editor
        editor->setCurrentIndex( editor->findText( value.toString()));
        return editor;
    }
    default:;
    }

    if (index.column() == 0) {
        emit itemEditTrigged( index);
        return 0;  // No inline editor
    }

    QWidget*  editor = QItemDelegate::createEditor( parent, option, index);

    //// Get value snapshot into editor
    QItemDelegate::setEditorData( editor, index);
    return editor;
}


void  MultiDelegate::setupCalenderWidget( QDateTimeEdit* editor)  const
{
    editor->setCalendarPopup(true);
    QCalendarWidget*  calendar = editor->calendarWidget();
    if (calendar) {
        calendar->setFirstDayOfWeek(Qt::Monday);
        calendar->setVerticalHeaderFormat( QCalendarWidget::ISOWeekNumbers);
    }
}


QWidget*  MultiDelegate::createBitSetEditor( QWidget* parent, const QModelIndex& index,
                                             const QVariant& value)  const
{
    QVariant  varList = index.model()->data( index, ItemDataRole::EnumList);
    if (varList.isNull())  return arnNullptr;  // Not a enum-list

    int  maxHeight           = 100;  // Default
    QStringList  valList     = value.toStringList();
    QStringList  bitTextList = varList.toStringList();
    int  bitItemCount        = bitTextList.size();

    QFrame*  frame = new QFrame();
    frame->setObjectName( "Map");
    frame->setAutoFillBackground( true);
    frame->setFrameStyle( QFrame::Panel | QFrame::Plain);
    frame->setLineWidth( 1);
    QVBoxLayout*  lay = new QVBoxLayout( frame);
    lay->setContentsMargins( 5, 5, 5, 5);
    lay->setSpacing( 5);

    //// Setup Bit editor
    if (bitItemCount > 0) {
        QListWidget*  bitEditor = new QListWidget();
        bitEditor->setObjectName( "Bits");
        bitEditor->setFrameStyle( QFrame::NoFrame);
        foreach (const QString& bitItemText, bitTextList) {
            QListWidgetItem*  bitItem = new QListWidgetItem( bitItemText, bitEditor);
            bitItem->setFlags( bitItem->flags() | Qt::ItemIsUserCheckable);
            bitItem->setCheckState(Qt::Unchecked);
        }

        int width  = bitEditor->sizeHintForColumn(0) + 5;
        int height = bitEditor->sizeHintForRow(0) * bitEditor->count() + 10;
        maxHeight = bitEditor->sizeHintForRow(0) * 10 + 10;

        bitEditor->setMinimumWidth( width);
        bitEditor->setMaximumWidth( width);
        bitEditor->setMinimumHeight( height);
        bitEditor->setMaximumHeight( height);

        lay->addWidget( bitEditor);

        //// Get value snapshot into Bit editor
        for (int i = 0; i < bitItemCount; ++i) {
            QListWidgetItem*  bitItem = bitEditor->item( i);
            bool  isActive = valList.contains( bitItem->text());
            bitItem->setCheckState( isActive ? Qt::Checked : Qt::Unchecked);
        }
    }

    //// Setup SubEnum editors
    QStringList  subEnumNames = index.model()->data( index, ItemDataRole::SubEnumNameList).toStringList();
    QStringList  subEnumsAll  = index.model()->data( index, ItemDataRole::SubEnumList).toStringList();
    int  subEnumCount = subEnumNames.size();
    for (int i = 0; i < subEnumCount; ++i) {
        QLabel*  label = new QLabel();
        label->setText( subEnumNames.at( i));

        QStringList  subEnums = subEnumsAll.filter( QString( ":%1:").arg( i));
        subEnums = subEnums.replaceInStrings( QString( ":%1:").arg( i), "");
        QComboBox* subEditor = new QComboBox();
        subEditor->setObjectName( QString( "SubEnum:%1").arg( i));
        subEditor->setSizeAdjustPolicy( QComboBox::AdjustToContents);
        subEditor->addItems( subEnums);

        //// Get value snapshot into SubEnum editor
        for (int j = valList.size() - 1; j >=0; --j) {
            int idx = subEditor->findText( valList.at( j));
            if (idx >= 0) {
                subEditor->setCurrentIndex( idx);
                break;
            }
        }

        lay->addWidget( label);
        lay->addWidget( subEditor);
        if (bitItemCount == 0)
            maxHeight = (label->sizeHint().height() + subEditor->sizeHint().height()) * 4;
    }

    //// Finalize the total editor
    frame->setLayout( lay);
    QScrollArea*  editor = new QScrollArea( parent);
    editor->setObjectName( "EnumBits");
    editor->setFrameStyle( QFrame::NoFrame);
    editor->setWidget( frame);
    editor->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff);
    editor->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded);

    int editHeight = frame->height() + 5;
    int editWidth  = frame->width()  + 3 ;
    if (editHeight > maxHeight) {
        editHeight = maxHeight;
        editWidth += QApplication::style()->pixelMetric( QStyle::PM_ScrollBarExtent);
    }
    editor->setMinimumWidth( editWidth);
    editor->setMaximumWidth( editWidth);
    editor->setMinimumHeight( editHeight);
    editor->setMaximumHeight( editHeight);

    return editor;
}


void  MultiDelegate::setEditorData( QWidget* editor,
                                    const QModelIndex& index)  const
{
    QVariant value = index.model()->data( index, Qt::EditRole);

    QString  className = editor->metaObject()->className();
    if (className == "QTimeEdit") {
    }
    else if (className == "QDateEdit") {
    }
    else if (className == "QDateTimeEdit") {
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
    }
    else if (className == "QListWidget") {
    }
    else if (className == "QCheckBox") {
        QCheckBox*  ed = qobject_cast<QCheckBox*>(editor);
        Q_ASSERT( ed);
        ed->setChecked( value.Bool);
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
    else if (className == "QListWidget") {
        QListWidget*  ed = qobject_cast<QListWidget*>(editor);
        Q_ASSERT( ed);
        QStringList  valList;
        int  itemCount = ed->count();
        for (int i = 0; i < itemCount; ++i) {
            QListWidgetItem*  bitItem = ed->item(i);
            bool  isChecked = (bitItem->checkState() == Qt::Checked);
            if (isChecked)
                valList += bitItem->text();
        }
        value = QVariant( valList);
    }
    else if (className == "QCheckBox") {
        QCheckBox*  ed = qobject_cast<QCheckBox*>(editor);
        Q_ASSERT( ed);
        value = QVariant( ed->isChecked());
    }
    else {
        QString  edName = editor->objectName();
        if (edName == "EnumBits") {
            QStringList  valList;
            QWidget*  edMap = editor->findChild<QWidget*>( "Map");
            foreach( QObject* edPart, edMap->children()) {
                // qDebug() << "EnumBits: " << edPart->objectName() << edPart->metaObject()->className();
                QStringList  parts = edPart->objectName().split(":");
                QString  partName = parts.at( 0);
                if (partName == "Bits") {
                    QListWidget*  ed = qobject_cast<QListWidget*>( edPart);
                    Q_ASSERT( ed);
                    int  itemCount = ed->count();
                    for (int i = 0; i < itemCount; ++i) {
                        QListWidgetItem*  bitItem = ed->item(i);
                        bool  isChecked = (bitItem->checkState() == Qt::Checked);
                        if (isChecked)
                            valList += bitItem->text();
                    }
                }
                else if (partName == "SubEnum") {
                    QComboBox*  ed = qobject_cast<QComboBox*>( edPart);
                    Q_ASSERT( ed);
                    valList += ed->currentText();
                }
            }
            value = QVariant( valList);
        }
        else {
            QItemDelegate::setModelData( editor, model, index);
            return;
        }
    }
    model->setData(index, value, Qt::EditRole);
}


void MultiDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED( index);
    editor->setGeometry(option.rect);
}


//// Experimental ...
void MultiDelegate::closeEmittingEditor()
{
    QWidget*  ed = qobject_cast<QWidget*>( sender());
    Q_ASSERT(ed);
    // qDebug() << "Closing editor: type=" << ed->metaObject()->className();
    emit closeEditor( ed);
}
