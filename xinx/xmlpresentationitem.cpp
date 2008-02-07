/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

// Xinx header
#include "xmlpresentationitem.h"

// Qt header
#include <QStringList>
#include <QMimeData>
#include <QIcon>

/* XmlPresentationItem */

XmlPresentationItem::XmlPresentationItem( QDomNode &node, int row, XmlPresentationItem * parent ) {
	m_domNode = node;
	m_rowNumber = row;
	m_parentItem = parent;
 }

XmlPresentationItem::~XmlPresentationItem() {
	QHash<int,XmlPresentationItem*>::iterator it;
	for( it = m_childItems.begin(); it != m_childItems.end(); ++it )
		delete it.value();
}

QDomNode XmlPresentationItem::node() const {
	return m_domNode;
}

XmlPresentationItem *XmlPresentationItem::parent() {
	return m_parentItem;
}

XmlPresentationItem *XmlPresentationItem::child(int i) {
	if( m_childItems.contains( i ) )
		return m_childItems[i];

	if( i >= 0 && i < m_domNode.attributes().count() ) {
		QDomNode attribute = m_domNode.attributes().item( i );
		XmlPresentationItem * childItem = new XmlPresentationParamItem( attribute, i, this );
		m_childItems[i] = childItem;
		return childItem;
	}
	
	if( i >= m_domNode.attributes().count() && i < m_domNode.childNodes().count() + m_domNode.attributes().count() ) {
		QDomNode childNode = m_domNode.childNodes().item( i - m_domNode.attributes().count() );
		XmlPresentationItem *childItem = new XmlPresentationNodeItem( childNode, i, this );
		m_childItems[i] = childItem;
		return childItem;
	}
	
	return 0;
}

int XmlPresentationItem::row() {
	return m_rowNumber;
} 

QString XmlPresentationItem::xpath() const {
	if( m_parentItem )
		return m_parentItem->xpath() + "/" + xpathName();
	else
		return "/" + xpathName();
}

QString XmlPresentationItem::xpathName() const {
	return m_domNode.nodeName();
}


/* XmlPresentationNodeItem */

XmlPresentationNodeItem::XmlPresentationNodeItem( QDomNode & node, int row, XmlPresentationItem * parent ) : XmlPresentationItem( node, row, parent ) {
	
}

XmlPresentationNodeItem::~XmlPresentationNodeItem() {
	
}

QString XmlPresentationNodeItem::xpathName() const {
	QString name = XmlPresentationItem::xpathName();
	if( ! m_domNode.attributes().namedItem( "name" ).nodeValue().isEmpty() )
		name += "[@name='" + m_domNode.attributes().namedItem( "name" ).nodeValue() + "']";
	return name;
}

/* XmlPresentationParamItem */

XmlPresentationParamItem::XmlPresentationParamItem( QDomNode & node, int row, XmlPresentationItem * parent ) : XmlPresentationItem( node, row, parent ) {
	
}

XmlPresentationParamItem::~XmlPresentationParamItem() {
	
}

QString XmlPresentationParamItem::xpathName() const {
	return "@" + XmlPresentationItem::xpathName();
}


/* XmlPresentationModel */

XmlPresentationModel::XmlPresentationModel( QDomDocument document, QObject *parent ) : QAbstractItemModel(parent) {
	m_rootElement = document.documentElement();
	m_rootItem = new XmlPresentationItem( m_rootElement, 0 );
}

XmlPresentationModel::~XmlPresentationModel() {
	delete m_rootItem;
}

int XmlPresentationModel::columnCount(const QModelIndex &/*parent*/) const {
	return 1;
}

QVariant XmlPresentationModel::data(const QModelIndex &index, int role) const {
	if (!index.isValid())
		return QVariant();

	XmlPresentationItem *item = static_cast<XmlPresentationItem*>(index.internalPointer());
	QDomNode node = item->node();

	if( role == Qt::DisplayRole ) {
		switch (index.column()) {
		case 0:
			return node.nodeName();
		default:
			return QVariant();
		} 
	} else if( role == Qt::UserRole ) {
		switch( index.column() ) {
		case 0:
			return item->xpath();
		default:
			return QVariant();
		}
	} else if( ( role == Qt::DecorationRole ) && ( index.column() == 0 ) ) {
		if( dynamic_cast<XmlPresentationNodeItem*>( item ) ) 
			return QIcon( ":/images/balise.png" );
		else
			return QIcon( ":/images/variable.png" );
	} else if( role == Qt::ToolTipRole ) {
		QString params;

		for( int i = 0 ; i < node.attributes().count() ; i++ ) {
			QDomNode attribute = node.attributes().item( i );
			params += "@" + attribute.nodeName() + "=" + attribute.nodeValue();
			if( i < node.attributes().count() - 1 )
				params += "\n";
		}
		params += node.nodeValue().split("\n").join(" ");
		
		return params;
	}

	return QVariant();
}

Qt::ItemFlags XmlPresentationModel::flags(const QModelIndex &index) const {
	if (!index.isValid())
		return 0;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
}

QVariant XmlPresentationModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		switch (section) {
		case 0:
			return tr( "Node" );
		default:
			return QVariant();
		}
	}

	return QVariant();
}

QModelIndex XmlPresentationModel::index(int row, int column, const QModelIndex &parent) const {
	if( ! hasIndex(row, column, parent) )
		return QModelIndex();

	XmlPresentationItem *parentItem;

	if( !parent.isValid() )
		parentItem = m_rootItem;
	else
		parentItem = static_cast<XmlPresentationItem*>(parent.internalPointer());

	XmlPresentationItem *childItem = parentItem->child(row);
	if(childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

QModelIndex XmlPresentationModel::parent(const QModelIndex &child) const {
	if (!child.isValid())
		return QModelIndex();

	XmlPresentationItem *childItem = static_cast<XmlPresentationItem*>(child.internalPointer());
	XmlPresentationItem *parentItem = childItem->parent();

	if(!parentItem || parentItem == m_rootItem)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

int XmlPresentationModel::rowCount(const QModelIndex &parent) const {
	if (parent.column() > 0)
		return 0;

	XmlPresentationItem *parentItem;

	if(!parent.isValid())
		parentItem = m_rootItem;
	else
		parentItem = static_cast<XmlPresentationItem*>(parent.internalPointer());
	
	if( dynamic_cast<XmlPresentationParamItem*>(parentItem) )
		return 0;
	else
		return parentItem->node().childNodes().count() + parentItem->node().attributes().count();
} 

QStringList XmlPresentationModel::mimeTypes() const {
	return QStringList() << "text/plain";
}

QMimeData * XmlPresentationModel::mimeData( const QModelIndexList &indexes ) const {
	QMimeData *mimeData = new QMimeData();
	QString text;

	foreach( QModelIndex index, indexes ) {
		if ( index.isValid() ) {
			text += data( index, Qt::UserRole ).toString();
		}
	}

	mimeData->setText( text );
	return mimeData;
} 
