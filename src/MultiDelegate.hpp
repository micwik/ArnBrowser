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

#ifndef MULTIDELEGATE_HPP
#define MULTIDELEGATE_HPP

#include <QItemDelegate>
#include <QModelIndex>
#include <QDialog>
#include <QObject>

class QDateTimeEdit;
class QLabel;
//class QIcon;


class PixmapViewer : public QDialog
{
    Q_OBJECT
public:
    PixmapViewer( QWidget* parent = 0);
    void  setPixmap( const QPixmap& pixmap);

private:
    QLabel*  _img;
};


class MultiDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    MultiDelegate( QObject* parent = 0);
    QWidget*  createEditor( QWidget* parent, const QStyleOptionViewItem& option,
                            const QModelIndex& index)  const;
    void  setupCalenderWidget( QDateTimeEdit* editor)  const;
    QWidget*  createBitSetEditor( QWidget* parent, const QModelIndex& index, const QVariant& value)  const;

    void  setEditorData( QWidget* editor, const QModelIndex& index)  const;
    void  setModelData( QWidget* editor, QAbstractItemModel* model,
                        const QModelIndex& index)  const;
    void  updateEditorGeometry( QWidget* editor,
                                const QStyleOptionViewItem& option, const QModelIndex& index)  const;
    void  paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index )  const;
    void  paintPixmap( QPainter* painter, const QStyleOptionViewItem& option, const QPixmap& pixmap )  const;
    QSize  sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index)  const;

signals:
    void  itemEditTrigged( const QModelIndex& index)  const;

private slots:
    void  closeEmittingEditor();

private:
    //QIcon  _icon;
};

#endif // MULTIDELEGATE_HPP
