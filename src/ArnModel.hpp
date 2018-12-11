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

#ifndef ARNMODEL_HPP
#define ARNMODEL_HPP

#include "ItemDataRole.hpp"
#include "Connector.hpp"
#include <QAbstractItemModel>
#include <ArnInc/ArnItem.hpp>
#include <ArnInc/ArnClient.hpp>

class ArnMonitor;

namespace Arn {
class XStringMap;
}


class ArnNode : public ArnItem
{
    Q_OBJECT
public:
    ArnNode( QObject *qobjParent = 0);
    ArnNode( const QString &path, QObject *qobjParent = 0);
    ArnNode( ArnNode* parent, const QString& item, int row);
    ~ArnNode();
    void  init( ArnNode* parent = 0);
    void  dealloc();
    void  reInit();

    ArnNode*  _parent;
    ArnNode*  _valueChild;
    ArnNode*  _nameChild;
    ArnNode*  _setChild;
    ArnNode*  _propChild;
    ArnNode*  _infoChild;
    ArnMonitor*  _arnMon;
    typedef Arn::XStringMap  SetMap;
    typedef Arn::XStringMap  PropMap;
    SetMap*  _setMap;
    SetMap*  _propMap;
    int  _folderChildN;  // -1 = Has been at least one,  >= 0 Actual number so far
    int  _leafChildN;    // -1 = Has been at least one,  >= 0 Actual number so far
    bool  _isExpanded;
    bool  _isBitSet;
    QList<ArnNode*>  _children;
};


class ArnModel : public QAbstractItemModel
{
Q_OBJECT
public:
    struct Role
    {
        enum E  {
            Path   = ItemDataRole::Path,
            Hidden = ItemDataRole::UserApp
        };
        E  e;
        Role( E v_ = E(0)) : e( v_)  {}
    };

    explicit ArnModel( Connector* connector, QObject* parent = 0);
    void  setClient( ArnClient* client);
    void  setHideBidir( bool isHide);
    void  start();
    void  clear();
    void  doFakePath( const QString& path);

    QModelIndex  index( int row, int column, const QModelIndex &parent)  const;
    QModelIndex  parent(const QModelIndex &child)  const;
    int  rowCount(const QModelIndex &parent)  const;
    int  columnCount(const QModelIndex &parent)  const;
    QVariant  data(const QModelIndex &index, int role)  const;
    QVariant  headerData(int section, Qt::Orientation orientation, int role)  const;
    bool  hasChildren( const QModelIndex& parent = QModelIndex())  const;
    bool  canFetchMore( const QModelIndex& parent )  const;
    void  fetchMore( const QModelIndex& parent );
    Qt::ItemFlags  flags( const QModelIndex& index)  const;
    bool  setData( const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

    ArnNode*  nodeFromIndex( const QModelIndex& index)  const;
    QModelIndex  indexFromNode( ArnNode* node, int column);

signals:
    void  hiddenRow( int row, QModelIndex parent, bool isHidden)  const;
    void  rowInserted( QModelIndex index, int row)  const;

public slots:
    void  netChildFound( QString path, ArnNode* node = 0);

private slots:
    void  nodeDataChanged();
    void  nodeModeChanged();
    void  updateSetMap( ArnNode* node = 0);
    void  updateBitSetMap( ArnNode* node = 0);
    void  updatePropMap( ArnNode* node = 0);
    void  destroyNode();

private:
    void  arnMonStart( ArnNode* node);
    void  doInsertItem( const QModelIndex& index, ArnNode* node, QString path);
    bool  submitRowHidden( int row, const QModelIndex& index, const QString& name)  const;
    QVariant  adjustedNodeData( const ArnNode* node, int role)  const;
    bool  setAdjustedNodeData( ArnNode* node, const QVariant& data);

    ArnNode*  _rootNode;
    bool  _isHideBidir;
    QModelIndex  _fetchIndex;
    QList<ArnNode*>  _folderChildren;

    ArnClient*  _arnClient;
    Connector*  _connector;
};

#endif // ARNMODEL_HPP
