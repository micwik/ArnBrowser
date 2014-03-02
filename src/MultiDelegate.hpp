// Copyright (C) 2010-2014 Michael Wiklund.
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

    void  setEditorData( QWidget* editor, const QModelIndex& index)  const;
    void  setModelData( QWidget* editor, QAbstractItemModel* model,
                      const QModelIndex& index)  const;
    void  updateEditorGeometry( QWidget* editor,
                              const QStyleOptionViewItem& option, const QModelIndex& index)  const;
    void  paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index )  const;
    void  paintPixmap( QPainter* painter, const QStyleOptionViewItem& option, const QPixmap& pixmap )  const;
    QSize  sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index)  const ;
signals:

public slots:

private:
    //QIcon  _icon;
};

#endif // MULTIDELEGATE_HPP
