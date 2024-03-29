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

#include "ArnModel.hpp"
#include <ArnInc/Arn.hpp>
#include <ArnInc/ArnMonitor.hpp>
#include <QRegExp>
#include <QPixmap>
#include <QBrush>
#include <QStringList>

using Arn::XStringMap;


void  ArnNode::init( ArnNode* parent)
{
    _parent       = parent;
    _valueChild   = arnNullptr;
    _nameChild    = arnNullptr;
    _setChild     = arnNullptr;
    _propChild    = arnNullptr;
    _infoChild    = arnNullptr;
    _arnMon       = arnNullptr;
    _setMap       = arnNullptr;
    _propMap      = arnNullptr;
    _enumTxt      = arnNullptr;
    _folderChildN = 0;
    _leafChildN   = 0;
    _isExpanded   = false;
    _isBitSet     = false;
}


//// Must deallocate everything, also qobjects
void  ArnNode::dealloc()
{
    if (_valueChild)  delete _valueChild;
    if (_nameChild)   delete _nameChild;
    if (_setChild)    delete _setChild;
    if (_propChild)   delete _propChild;
    if (_infoChild)   delete _infoChild;
    if (_arnMon)      delete _arnMon;

    if (_setMap)      delete _setMap;
    if (_propMap)     delete _propMap;
    if (_enumTxt)     delete _enumTxt;
}


void  ArnNode::reInit()
{
    dealloc();
    init( _parent);
}


ArnNode::~ArnNode()
{
    dealloc();
}


ArnNode::ArnNode( QObject *qobjParent) :
        ArnItem( qobjParent)
{
    //// Init Root Node
    init();
}


ArnNode::ArnNode( const QString &path, QObject *qobjParent) :
        ArnItem( path, qobjParent)
{
    //// Root Node / ValueChild Node / SetChild Node
    init();
}


ArnNode::ArnNode( ArnNode* parent, const QString& item, int row) :
        ArnItem( parent->path() + item, parent)     // New item is also a QObject child
{
    init();
    _parent = parent;
    if (row >= 0)
        parent->_children.insert( row, this);
}


ArnModel::ArnModel( Connector* connector, QObject* parent) :
    QAbstractItemModel( parent)
{
    _connector   = connector;
    _rootNode    = new ArnNode( this);
    _isHideBidir = false;
}


void  ArnModel::setClient(ArnClient* client)
{
    _arnClient = client;
    _arnClient->addMountPoint( _connector->toLocalPath("/"), "/");
}


void  ArnModel::setHideBidir( bool isHide)
{
    _isHideBidir = isHide;
}


void  ArnModel::start()
{
    _rootNode->open( _connector->toLocalPath("/"));
}


void  ArnModel::clear()
{
    QList<ArnNode*>  children = _rootNode->_children;
    foreach (ArnNode *child, children) {
        child->destroyLinkLocal();
    }
    _rootNode->reInit();
}


void  ArnModel::doFakePath( const QString& path)
{
    QString      relPath = Arn::fullPath( path).mid(1);  // Leading "/" removed
    if (relPath.isEmpty())  return;

    QModelIndex  nodeIdx = QModelIndex();
    ArnNode*     node    = 0;
    int          pos     = 0;
    forever {
        int nextPos = relPath.indexOf("/", pos) + 1;
        if (nextPos <= 0)
            nextPos = relPath.size();
        QString  itemName = Arn::convertName( relPath.mid( pos, nextPos - pos));
        pos = nextPos;

        node = nodeFromIndex( nodeIdx);
        node->_isExpanded = true;
        netChildFound( itemName, node);
        if (pos >= relPath.size())  break;

        QList<ArnNode*>  children = node->_children;
        int row = 0;
        forever {
            if (row >= children.size())  return;  // Internal failure
            if (children.at( row)->name( Arn::NameF()) == itemName)  break;
            ++row;
        }
        nodeIdx = index( row, 0, nodeIdx);
    }
}


QModelIndex  ArnModel::index( int row, int column, const QModelIndex &parent) const
{
    if (row < 0  ||  column < 0)  return QModelIndex();

    ArnNode*  parentNode = nodeFromIndex( parent);
    ArnNode*  childNode = parentNode->_children.value( row);
    if (!childNode)  return QModelIndex();

    return createIndex( row, column, childNode);
}


QModelIndex  ArnModel::parent(const QModelIndex &child) const
{
    ArnNode*  node = nodeFromIndex( child);
    if (!node)  return QModelIndex();

    ArnNode*  parentNode = node->_parent;
    if (!parentNode)  return QModelIndex();

    ArnNode*  grandParentNode = parentNode->_parent;
    if (!grandParentNode)  return QModelIndex();

    int  row = grandParentNode->_children.indexOf( parentNode);
    return createIndex( row, 0, parentNode);
}


int  ArnModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)  return 0;

    ArnNode*  parentNode = nodeFromIndex( parent);
    if (!parentNode)  return 0;

    return parentNode->_children.size();
}


int  ArnModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    
    return 2;
}


QVariant  ArnModel::data(const QModelIndex &index, int role) const
{
    ArnNode*  node = nodeFromIndex( index);
    if (!node)  return QVariant();

    switch (role) {
    case Qt::DisplayRole:
        // Fall throu

    case Qt::EditRole:
        switch (index.column()) {
        case 0:
            return node->name( Arn::NameF::NoFolderMark);
        case 1:
            return adjustedNodeData( node, role);
        }
        break;

    case Qt::ToolTipRole:
        if (node->isFolder() && node->_infoChild) {
            QString  info = node->_infoChild->toString();
            QRegExp rx("<tt>(.*)</tt>");
            if (rx.indexIn( info) >= 0) {
                return rx.cap(1);
            }
        }
        break;

    case Qt::DecorationRole:
        if (index.column() == 0) {
            if (node->isFolder()) {
                //qDebug() << "Data DecorationRole: path=" << node->path( Arn::NameF())
                //         << " leafChildN=" << node->_leafChildN << " folderChildN=" << node->_folderChildN;
                if (node->_leafChildN == 0)  // No additional leaves as children, only folders
                    return QPixmap("://pic/folder.png");
                else if (node->_folderChildN == 0)  // No additional folders as children, only leaves
                    return QPixmap("://pic/documents.png");
                else  // Mixed folders and leaves
                    return QPixmap("://pic/folder_document.png");
            }
            else {
                if (node->isPipeMode())
                    return QPixmap("://pic/pipe.png");
                else if (node->isBiDirMode())
                    return QPixmap("://pic/document_valid.png");
                else
                    return QPixmap("://pic/document.png");
            }
        }
        break;
    case Qt::ForegroundRole: {
        if ((index.column() == 1) && !node->_valueChild && node->_nameChild)
            return QBrush( Qt::darkBlue);
        return QVariant();
    }
    case Role::Path:
        return node->path();

    case Role::Hidden: {
        QString  name = node->name( Arn::NameF::NoFolderMark);
        return submitRowHidden( index.row(), parent( index), name);
    }
    case ItemDataRole::EnumList: {
        if (index.column() < 1)  return QVariant();

        if (node->isFolder() && node->_setMap && node->_valueChild) {
            QStringList retVal;
            if (node->_isBitSet) {
                retVal = node->_enumTxt->getBasicTextList( 0, true);
            }
            else {
                retVal = node->_setMap->values();
            }
            // qDebug() << "EnumList: " << retVal;
            return retVal;
        }
        break;
    }
    case ItemDataRole::SubEnumNameList: {
        if (index.column() < 1)  return QVariant();

        if (node->isFolder() && node->_enumTxt && node->_valueChild) {
            QStringList retVal;
            int  subEnumCount = node->_enumTxt->subEnumCount();
            for (int i = 0; i < subEnumCount; ++i) {
                QString  subEnumName = node->_enumTxt->subEnumNameAt( i);
                retVal += subEnumName;
            }
            return retVal;
        }
        break;
    }
    case ItemDataRole::SubEnumList: {
        if (index.column() < 1)  return QVariant();

        if (node->isFolder() && node->_enumTxt && node->_valueChild) {
            QStringList retVal;
            int subEnumCount = node->_enumTxt->subEnumCount();
            for (int i = 0; i < subEnumCount; ++i) {
                const Arn::EnumTxt*  subEnumText = node->_enumTxt->subEnumAt( i);
                Q_ASSERT( subEnumText);
                QStringList  enumTextList = subEnumText->getBasicTextList( 0, true);
                foreach( const QString& enumText, enumTextList ) {
                    retVal += QString(":%1:").arg( i) + enumText;
                }
            }
            return retVal;
        }
        break;
    }
    }
    return QVariant();
}


QVariant  ArnModel::adjustedNodeData( const ArnNode *node, int role) const
{
    if (!node)  return "";

    const ArnNode*  valueNode = node;
    QString  unit;
    int  prec = -1;

    if (node->isFolder()) {
        if (!node->_valueChild) {
            if (node->_nameChild)
                return node->_nameChild->toString();
            else
                return QString();
        }

        valueNode = node->_valueChild;

        if (node->_isBitSet) {
            int  itemVal = valueNode->toInt();
            QStringList  valList = node->_enumTxt->flagsToStringList( itemVal);
            if (role == Qt::DisplayRole)
                return "[ " + valList.join(" | ") + " ]";
            else
                return valList;
        }

        if (node->_setMap) {
            QByteArray  itemVal = valueNode->toByteArray();
            int  index = node->_setMap->indexOf( itemVal);
            QString  format = (role == Qt::DisplayRole) ? "< %1 >" : "%1";
            if (index >= 0)
                return format.arg( node->_setMap->valueString( index));
            else
                return format.arg( QString::fromUtf8( itemVal + "?"));
        }

        if (node->_propMap) {
            if (role == Qt::DisplayRole) {
                unit = node->_propMap->valueString("unit");
                if (!unit.isEmpty())
                    unit.insert(0, "  ");
            }
            prec = node->_propMap->value("prec", "-1").toInt();
        }
    }

    Arn::DataType  type = valueNode->type();
    if (prec >= 0) { // The value should be a floating-point
        return QString::number( valueNode->toDouble(), 'f', prec) + unit;
    }
    else if ((type == type.ByteArray)
         ||  (type == type.String)) {
        QString  value = valueNode->toString();
        int  i = value.indexOf('\n');
        if (i < 0) {
            return value + unit;
        }
        else
            return value.left(i);
    }
    else {
        return valueNode->toVariant();
    }
}


QVariant  ArnModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal  &&  role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return "Path";
        case 1:
            return "Value";
        }
    }
    return QVariant();
}


ArnNode*  ArnModel::nodeFromIndex( const QModelIndex& index) const
{
    if (index.isValid()) {
        return static_cast<ArnNode*>( index.internalPointer());
    }
    else {
        return _rootNode;
    }
}


QModelIndex  ArnModel::indexFromNode( ArnNode *node, int column)
{
    QModelIndex  index;
    if (!node)  return index;

    ArnNode*  parentNode = node->_parent;
    if (parentNode && (column >= 0)) {
        int  row = parentNode->_children.indexOf( node);
        if (row >= 0)
            index = createIndex( row, column, node);
    }
    return index;
}


bool  ArnModel::hasChildren( const QModelIndex& parent ) const
{
    ArnNode*  parentNode = nodeFromIndex( parent);
    if (!parentNode)  return false;

    return parentNode->isFolder();
}


bool  ArnModel::canFetchMore( const QModelIndex& parent ) const
{
    ArnNode*  parentNode = nodeFromIndex( parent);
    if (!parentNode)  return false;

    if (parentNode->_isExpanded)  return false;  // Node is already exposing its children
    return parentNode->isFolder();
}


void  ArnModel::fetchMore( const QModelIndex& parent )
{
    ArnNode*  parentNode = nodeFromIndex( parent);
    if (!parentNode)  return;
    if (parentNode->_isExpanded)  return;  // Node is already exposing its children

    // qDebug() << "arnModel fetchMore: Monitor path=" << parentNode->path();
    arnMonStart( parentNode);
    parentNode->_isExpanded = true;
}


void  ArnModel::arnMonStart( ArnNode* node)
{
    if (!node)  return;

    ArnMonitor*  arnMon = node->_arnMon;
    if (arnMon) {  // NetMon already exist
        // qDebug() << "arnModel netMonReStart: Monitor path=" << node->path();
        arnMon->reStart();
    }
    else {
        // qDebug() << "arnModel netMonStart: Monitor path=" << node->path();
        arnMon = new ArnMonitor( node);
        node->_arnMon = arnMon;
        connect( arnMon, SIGNAL(arnChildFound(QString)), this, SLOT(netChildFound(QString)));
        QString  path = node->path();
        ArnClient*  client = path.startsWith( _connector->toLocalPath("/")) ? _arnClient : 0;
        arnMon->start( path, client);
    }
}


Qt::ItemFlags  ArnModel::flags( const QModelIndex& index) const
{
    if (!index.isValid()) {
        return Qt::ItemIsEnabled;
    }

    Qt::ItemFlags  iFlags = QAbstractItemModel::flags( index);  // Default
    ArnNode*  node = nodeFromIndex( index);
    if (!node)  return iFlags;

    if (index.column() == 1) {
        if (!node->isFolder() || node->_valueChild)
            iFlags |= Qt::ItemIsEditable;
    }
    else {
        if (!node->isFolder())
            iFlags |= Qt::ItemIsEditable;
    }

    return iFlags;
}


bool  ArnModel::setData( const QModelIndex& index, const QVariant& value, int role)
{
    if (index.isValid()  &&  role == Qt::EditRole) {
        ArnNode*  node = nodeFromIndex( index);
        if (!node)  return false;

        if (node->isFolder()) {
            if (node->_valueChild) {
                if (node->_isBitSet) {
                    QStringList  listVal = value.toStringList();
                    int  itemVal = node->_enumTxt->flagsFromStringList( listVal);
                    node->_valueChild->setValue( itemVal);
                }
                else if (node->_setMap) {
                    QString  itemVal = node->_setMap->keyString( value.toString());
                    node->_valueChild->setValue( itemVal);
                }
                else {
                    setAdjustedNodeData( node->_valueChild, value);
                }
                int  row = node->_children.indexOf( node->_valueChild);
                QModelIndex  indexVC = createIndex( row, 1, node->_valueChild);
                emit dataChanged( indexVC, indexVC);
            }
        }
        else {
            setAdjustedNodeData( node, value);
            emit dataChanged(index, index);
        }
        return true;
    }
    return false;
}


bool  ArnModel::setAdjustedNodeData( ArnNode* node, const QVariant& data)
{
    switch (data.type()) {
    case QMetaType::QByteArray:
        node->setValue( data.toByteArray());
        break;
    case QMetaType::QString:
    case QMetaType::Bool:
    case QMetaType::Int:
    case QMetaType::Double:
    case QMetaType::UInt:
        node->setValue( data.toString());
        break;
    default:
        node->setValue( data);
    }

    return true;
}


void  ArnModel::netChildFound(QString path, ArnNode* node)
{
    // qDebug() << "arnModel netChildFound: path=" << path;
    if (!node) {
        ArnMonitor*  arnMon = qobject_cast<ArnMonitor*>( sender());
        Q_ASSERT( arnMon);
        // if (!netMon)  return;

        node = qobject_cast<ArnNode*>( arnMon->parent());
    }
    Q_ASSERT( node);
    // if (!node)  return;

    QModelIndex  index = indexFromNode( node, 0);

    if (node->_isExpanded) {  // Node is exposing its children
        doInsertItem( index, node, path);
    }

    QString itemName = Arn::itemName( path);
    if (!node->_valueChild && (itemName == "value")) {
        //// Peek at child item "value"
        node->_valueChild = new ArnNode( path, node);
        connect( node->_valueChild, SIGNAL(changed()), node, SIGNAL(changed()));
        connect( node->_valueChild, SIGNAL(arnLinkDestroyed()), this, SLOT(destroyNode()));
        QModelIndex  valueIndex = createIndex( index.row(), 1, node);
        emit dataChanged( valueIndex, valueIndex);
    }
    if (!node->_nameChild && (itemName == "name")) {
        //// Peek at child item "name"
        node->_nameChild = new ArnNode( path, node);
        connect( node->_nameChild, SIGNAL(changed()), node, SIGNAL(changed()));
        connect( node->_nameChild, SIGNAL(arnLinkDestroyed()), this, SLOT(destroyNode()));
        QModelIndex  valueIndex = createIndex( index.row(), 1, node);
        emit dataChanged( valueIndex, valueIndex);
    }
    else if (!node->_setChild && (itemName == "set")) {
        //// Peek at child item "set"
        node->_setMap   = new ArnNode::SetMap;
        node->_setChild = new ArnNode( path, node);
        connect( node->_setChild, SIGNAL(changed()), this, SLOT(updateSetMap()));
        connect( node->_setChild, SIGNAL(arnLinkDestroyed()), this, SLOT(destroyNode()));
        updateSetMap( node->_setChild);
    }
    else if (!node->_setChild && (itemName == "bitSet")) {
        //// Peek at child item "bitSet"
        node->_isBitSet = true;
        node->_setMap   = new ArnNode::SetMap;
        node->_enumTxt  = new ArnNode::EnumTxt( true);
        node->_setChild = new ArnNode( path, node);
        connect( node->_setChild, SIGNAL(changed()), this, SLOT(updateBitSetMap()));
        connect( node->_setChild, SIGNAL(arnLinkDestroyed()), this, SLOT(destroyNode()));
        updateBitSetMap( node->_setChild);
    }
    else if (!node->_propChild && (itemName == "property")) {
        //// Peek at child item "property"
        node->_propMap   = new ArnNode::PropMap;
        node->_propChild = new ArnNode( path, node);
        connect( node->_propChild, SIGNAL(changed()), this, SLOT(updatePropMap()));
        connect( node->_propChild, SIGNAL(arnLinkDestroyed()), this, SLOT(destroyNode()));
        updatePropMap( node->_propChild);
    }
    else if (!node->_infoChild && (itemName == "info")) {
        //// Peek at child item "info"
        node->_infoChild = new ArnNode( path, node);
        connect( node->_infoChild, SIGNAL(changed()), node, SIGNAL(changed()));
        connect( node->_infoChild, SIGNAL(arnLinkDestroyed()), this, SLOT(destroyNode()));
        QModelIndex  valueIndex = createIndex( index.row(), 1, node);
        emit dataChanged( valueIndex, valueIndex);
    }

    if (Arn::isFolderPath( path)) {
        if (node->_isExpanded) {
            if (node->_folderChildN >= 0)
                node->_folderChildN++;
            else
                node->_folderChildN = 1;
        }
        else
            node->_folderChildN = -1;  // Mark at least one folder child found (not exact for unexpanded nodes)
        emit dataChanged( index, index);
    }
    else {
        if (node->_isExpanded) {
            if (node->_leafChildN >= 0)
                node->_leafChildN++;
            else
                node->_leafChildN = 1;
        }
        else {
            node->_leafChildN = -1;  // Mark at least one leaf child found (not exact for unexpanded nodes)
        }
        emit dataChanged( index, index);
    }
}


void  ArnModel::doInsertItem( const QModelIndex& index, ArnNode* node, QString path)
{
    if (!node)  return;

    QString  itemName = Arn::itemName( path);
    int  insRow;
    for (insRow = 0; insRow < node->_children.size(); ++insRow) {
        ArnNode*  child = node->_children.at( insRow);
        QString  childName = child->name( Arn::NameF());
        // qDebug() << "arnModel doInsertItem: item=" << itemName << " child=" << childName;
        if (itemName == childName)  return;  // Item already exist
        if (itemName < childName)  break;  // Sorting place found
    }

    this->beginInsertRows( index, insRow, insRow);
    // qDebug() << "arnModel netChildFound: create item=" << itemName;
    ArnNode*  child = new ArnNode( node, itemName, insRow);
    connect( child, SIGNAL(changed()), this, SLOT(nodeDataChanged()));
    connect( child, SIGNAL(modeChanged(Arn::ObjectMode)), this, SLOT(nodeModeChanged()));
    connect( child, SIGNAL(arnLinkDestroyed()), this, SLOT(destroyNode()));
    this->endInsertRows();

    submitRowHidden( insRow, index, itemName);
    emit rowInserted( index, insRow);

    if (child->isFolder()) {
        // qDebug() << "arnModel netChildFound: Monitor path=" << child->path();
        arnMonStart( child);
    }
}


bool  ArnModel::submitRowHidden(  int row, const QModelIndex& index, const QString& name)  const
{
    if (name.startsWith('.')) {
        emit hiddenRow( row, index, true);
        return true;
    }
    if (name.endsWith('!')) {
        emit hiddenRow( row, index, _isHideBidir);
        return _isHideBidir;
    }
    return false;
}


void  ArnModel::nodeDataChanged()
{
    ArnNode*  node = qobject_cast<ArnNode*>( sender());
    QModelIndex  index = indexFromNode( node, 1);
    emit dataChanged( index, index);
}


void ArnModel::nodeModeChanged()
{
    ArnNode*  node = qobject_cast<ArnNode*>( sender());
    QModelIndex  index = indexFromNode( node, 0);
    emit dataChanged( index, index);
}



void  ArnModel::updateSetMap( ArnNode* node)
{
    ArnNode*  node_ = node;
    if (!node_)
        node_ = qobject_cast<ArnNode*>( sender());
    Q_ASSERT( node_);
    ArnNode*  parent = qobject_cast<ArnNode*>( node_->parent());
    Q_ASSERT( parent);
    Q_ASSERT( parent->_setMap);

    if (node_ != parent->_setChild)  return;

    parent->_setMap->fromXString( node_->toByteArray());
    XStringMap&  setMap = *parent->_setMap;
    for (int i = 0; i < setMap.size(); ++i) {
        bool  isOk = true;
        int  enumVal = Arn::EnumTxt::strToNum( setMap.keyRef( i), &isOk);
        if (isOk) {  // The key is a number dec/hex
            setMap.setKey( i, QByteArray::number( enumVal));  // Change the key to dec num
        }
    }
    parent->_setMap->setEmptyKeysToValue();

    QModelIndex  valueIndex = indexFromNode( parent, 1);
    emit dataChanged( valueIndex, valueIndex);
}


void  ArnModel::updateBitSetMap( ArnNode* node)
{
    ArnNode*  node_ = node;
    if (!node_)
        node_ = qobject_cast<ArnNode*>( sender());
    Q_ASSERT( node_);
    ArnNode*  parent = qobject_cast<ArnNode*>( node_->parent());
    Q_ASSERT( parent);
    Q_ASSERT( parent->_setMap);
    Q_ASSERT( parent->_enumTxt);

    if (node_ != parent->_setChild)  return;

    parent->_setMap->fromXString( node_->toByteArray());
    parent->_enumTxt->loadBitSet( *parent->_setMap);

    QModelIndex  valueIndex = indexFromNode( parent, 1);
    emit dataChanged( valueIndex, valueIndex);
}


void  ArnModel::updatePropMap( ArnNode* node_)
{
    ArnNode*  node = node_;
    if (!node)
        node = qobject_cast<ArnNode*>( sender());
    Q_ASSERT( node);
    ArnNode*  parent = qobject_cast<ArnNode*>( node->parent());
    Q_ASSERT( parent);
    Q_ASSERT( parent->_propMap);

    if (node != parent->_propChild)  return;

    parent->_propMap->fromXString( node->toByteArray());
    parent->_propMap->setEmptyKeysToValue();

    QModelIndex  valueIndex = indexFromNode( parent, 1);
    emit dataChanged( valueIndex, valueIndex);
}


void  ArnModel::destroyNode()
{
    ArnNode*  node = qobject_cast<ArnNode*>( sender());
    Q_ASSERT( node);
    ArnNode*  parent = qobject_cast<ArnNode*>( node->parent());
    Q_ASSERT( parent);

    if (node == parent->_valueChild) { // Remove Value-child
        parent->_valueChild = 0;
        QModelIndex  valueIndex = indexFromNode( parent, 1);
        emit dataChanged( valueIndex, valueIndex);
        node->deleteLater();
        return;
    }
    if (node == parent->_nameChild) { // Remove name-child
        parent->_nameChild = 0;
        QModelIndex  valueIndex = indexFromNode( parent, 1);
        emit dataChanged( valueIndex, valueIndex);
        node->deleteLater();
        return;
    }
    if (node == parent->_setChild) { // Remove Set-child
        delete parent->_setMap;
        if (parent->_enumTxt)
            delete parent->_enumTxt;
        parent->_isBitSet = false;
        parent->_setMap   = arnNullptr;
        parent->_enumTxt  = arnNullptr;
        parent->_setChild = arnNullptr;
        QModelIndex  valueIndex = indexFromNode( parent, 1);
        emit dataChanged( valueIndex, valueIndex);
        node->deleteLater();
        return;
    }
    if (node == parent->_propChild) { // Remove Property-child
        delete parent->_propMap;
        parent->_propMap = 0;
        parent->_propChild = 0;
        QModelIndex  valueIndex = indexFromNode( parent, 1);
        emit dataChanged( valueIndex, valueIndex);
        node->deleteLater();
        return;
    }
    if (node == parent->_infoChild) { // Remove Info-child
        parent->_infoChild = 0;
        QModelIndex  valueIndex = indexFromNode( parent, 1);
        emit dataChanged( valueIndex, valueIndex);
        node->deleteLater();
        return;
    }

    // parent->_arnMon->foundChildDeleted( node->path());  // Remove from found list
    int  row = parent->_children.indexOf( node);
    if (row >= 0) {  // Remove normal child node
        QModelIndex  index = indexFromNode( parent, 0);
        beginRemoveRows( index, row, row);
        if (node->isFolder()) {
            if (parent->_folderChildN > 0)
                parent->_folderChildN--;
        }
        else {
            if (parent->_leafChildN > 0)
                parent->_leafChildN--;
        }
        parent->_children.at( row)->deleteLater();  // Delete node
        parent->_children.removeAt( row);
        endRemoveRows();
        emit dataChanged( index, index);
        return;
    }
}
