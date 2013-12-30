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

#ifndef ARNMODEL_HPP
#define ARNMODEL_HPP

#include "ItemDataRole.hpp"
#include <QAbstractItemModel>
#include <ArnInc/ArnItem.hpp>
#include <ArnInc/ArnClient.hpp>

class ArnMonitor;
class XStringMap;


class ArnNode : public ArnItem
{
Q_OBJECT
public:
    ArnNode( const QString &path, QObject *qobjParent = 0);
    ArnNode( ArnNode* parent, const QString& item, int row);
    ~ArnNode();
    void  init();

    ArnNode*  _parent;
    ArnNode*  _valueChild;
    ArnNode*  _setChild;
    ArnNode*  _propChild;
    ArnNode*  _infoChild;
    ArnMonitor*  _arnMon;
    typedef XStringMap  SetMap;
    typedef XStringMap  PropMap;
    SetMap*  _setMap;
    SetMap*  _propMap;
    int  _folderChildN;  // -1 = Has been at least one,  >= 0 Actual number so far
    int  _leafChildN;    // -1 = Has been at least one,  >= 0 Actual number so far
    bool  _isExpanded;
    QList<ArnNode*>  _children;
};


class ArnModel : public QAbstractItemModel
{
Q_OBJECT
public:
    struct Role
    {
        enum E  {
            Path  = ItemDataRole::UserApp,
            Hidden
        };
        E  e;
        Role( E v_ = E(0)) : e( v_)  {}
    };

    explicit ArnModel( QObject* parent = 0);
    void  setClient( ArnClient* client);
    void  setHideBidir( bool isHide);

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

signals:
    void  hiddenRow( int row, QModelIndex parent, bool isHidden)  const;

public slots:

private slots:
    void  netChildFound( QString path);
    void  nodeDataChanged();
    void  nodeModeChanged();
    void  updateSetMap( ArnNode* node = 0);
    void  updatePropMap( ArnNode* node = 0);
    void  destroyNode();

private:
    ArnNode*  nodeFromIndex( const QModelIndex& index)  const;
    QModelIndex  indexFromNode( ArnNode* node, int column);
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
};

#endif // ARNMODEL_HPP
