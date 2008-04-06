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
#include <exceptions.h>

// Qt header
#include <QStringList>
#include <QMimeData>
#include <QIcon>

/* XmlPresentationItem */

XmlPresentationItem::XmlPresentationItem( QDomNode node, int row, XmlPresentationItem * parent ) 
	: m_domNode( node ), m_parentItem( parent ), m_rowNumber( row ) {
}

XmlPresentationItem::~XmlPresentationItem() {
	qDeleteAll( m_childItems );
}

QDomNode XmlPresentationItem::node() const {
	return m_domNode;
}

XmlPresentationItem * XmlPresentationItem::parent() {
	return m_parentItem;
}

int XmlPresentationItem::count() {
	if( m_childItems.count() == 0 ) {
		QDomNamedNodeMap attributes = m_domNode.attributes();
		for( int i = 0; i < attributes.count(); i++ ) {
			m_childItems.append( new XmlPresentationParamItem( attributes.item( i ), i, this ) );
		}
		int countAttribute = attributes.count();
		
		QDomElement node = m_domNode.firstChildElement();
		int index = 0;
		while( ! node.isNull() ) {
			m_childItems.append( new XmlPresentationNodeItem( node, index + countAttribute, this ) );
			if( node.nodeName() == "business_data" )
				m_businessData = node.toElement().text(); 
			if( node.nodeName() == "screen_data" )
				m_screenData = node.toElement().text();
			if( node.nodeName() == "error" )
				m_error = node.toElement().text();
			node = node.nextSiblingElement(); index++;
		}
	}
	
	return m_childItems.count();
}

XmlPresentationItem * XmlPresentationItem::child( int i ) {
	return m_childItems.value( i, NULL );
}

int XmlPresentationItem::row() {
	return m_rowNumber;
} 

QString XmlPresentationItem::xpath( bool unique ) const {
	if( m_parentItem )
		return m_parentItem->xpath() + "/" + xpathName( unique );
	else
		return "/" + xpathName( unique );
}

QString XmlPresentationItem::xpathName( bool /* unique */ ) const {
	return m_domNode.nodeName();
}

QString XmlPresentationItem::tipsText() const {
	return QString();
}

QString XmlPresentationItem::businessData() const {
	return m_businessData;
}

QString XmlPresentationItem::screenData() const {
	return m_screenData;
}

QString XmlPresentationItem::errorData() const {
	return m_error;
}


/* XmlPresentationNodeItem */

XmlPresentationNodeItem::XmlPresentationNodeItem( QDomNode node, int row, XmlPresentationItem * parent ) : XmlPresentationItem( node, row, parent ) {
	
}

XmlPresentationNodeItem::~XmlPresentationNodeItem() {
	
}

QString XmlPresentationNodeItem::tipsText() const {
	QString result;
	foreach( XmlPresentationItem * item, m_childItems ) {
		XmlPresentationParamItem * param = dynamic_cast<XmlPresentationParamItem*>( item );
		if( param )
			result += item->xpathName() + "=" + param->value() + "\n"; 
	}
	QDomNode node = m_domNode.firstChild();
	while( ! node.isNull() ) {
		if( node.isText() )
			result += node.toText().data().simplified();
		node = node.nextSibling();
	}
	if( ! m_businessData.isEmpty() )
		result += "Business=" + m_businessData.simplified() + "\n";
	if( ! m_screenData.isEmpty() )
		result += "Screen=" + m_screenData.simplified() + "\n";
	if( result.simplified().isEmpty() ) 
		result = XmlPresentationModel::tr( "(empty)" );
	return result;
}

QString XmlPresentationNodeItem::xpathName( bool unique ) const {
	QString name = XmlPresentationItem::xpathName();
	if( unique && (! m_domNode.attributes().namedItem( "name" ).nodeValue().isEmpty() ) )
		name += "[@name='" + m_domNode.attributes().namedItem( "name" ).nodeValue() + "']";
	return name;
}

bool XmlPresentationNodeItem::isView() const {
	return !m_domNode.attributes().namedItem( "name" ).nodeValue().isEmpty();
}

/* XmlPresentationParamItem */

XmlPresentationParamItem::XmlPresentationParamItem( QDomNode node, int row, XmlPresentationItem * parent ) : XmlPresentationItem( node, row, parent ) {
	XINX_ASSERT( ! m_domNode.toAttr().isNull() );
}

XmlPresentationParamItem::~XmlPresentationParamItem() {
	
}

QString XmlPresentationParamItem::value() const {
	return m_domNode.toAttr().nodeValue().simplified();
}

QString XmlPresentationParamItem::tipsText() const {
	return value();
}

QString XmlPresentationParamItem::xpathName( bool /* unique */ ) const {
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
	if((!index.isValid()) || (index.column() > 0))
		return QVariant();

	XmlPresentationItem *item = static_cast<XmlPresentationItem*>(index.internalPointer());
	QDomNode node = item->node();
	
	if( role == Qt::DisplayRole ) {
		return node.nodeName();
	} else if( role == XmlPresentationModel::XNamedPathRole ) {
		return item->xpath();
	} else if( role == XmlPresentationModel::XPathRole ) {
		return item->xpath( false );
	} else if( role == XmlPresentationModel::NamedViewRole ) {
		return item->xpathName();
	} else if( ( role == Qt::DecorationRole ) && ( index.column() == 0 ) ) {
		if( dynamic_cast<XmlPresentationNodeItem*>( item ) ) 
			return QIcon( ":/images/balise.png" );
		else
			return QIcon( ":/images/variable.png" );
	} else if( role == Qt::ToolTipRole ) {
		return item->tipsText();
	} else if( role == Qt::ForegroundRole ) {
		XmlPresentationNodeItem * node = dynamic_cast<XmlPresentationNodeItem*>( item );
		if( node && node->isView() ) 
			return Qt::blue;
		else if( ! item->screenData().isEmpty() )
			return Qt::darkRed;
		else if( ! item->errorData().isEmpty() )
			return Qt::red;
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
		return parentItem->count();
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
