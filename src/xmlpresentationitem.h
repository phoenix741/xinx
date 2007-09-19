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

#ifndef __XMLPRESENTATIONITEM_H__
#define __XMLPRESENTATIONITEM_H__

// Qt header
#include <QDomNode>
#include <QHash>
#include <QAbstractItemModel>
#include <QDomDocument>
#include <QModelIndex>
#include <QVariant>

class XmlPresentationItem {
public:
	XmlPresentationItem( QDomNode & node, int row, XmlPresentationItem * parent = 0 );
	virtual ~XmlPresentationItem();
	
	XmlPresentationItem * child( int i );
	XmlPresentationItem * parent();
	
	QDomNode node() const;
	
	int row();
protected:
	QDomNode m_domNode;
	QHash<int,XmlPresentationItem*> m_childItems;
	XmlPresentationItem * m_parentItem;
	int m_rowNumber;
};

class XmlPresentationNodeItem : public XmlPresentationItem {
public:
	XmlPresentationNodeItem( QDomNode & node, int row, XmlPresentationItem * parent = 0 );
	virtual ~XmlPresentationNodeItem();
};

class XmlPresentationParamItem : public XmlPresentationItem {
public:
	XmlPresentationParamItem( QDomNode & node, int row, XmlPresentationItem * parent = 0 );
	virtual ~XmlPresentationParamItem();
};

class XmlPresentationModel : public QAbstractItemModel {
	Q_OBJECT
public:
	XmlPresentationModel( QDomDocument document, QObject *parent = 0 );
	~XmlPresentationModel();

	QVariant data( const QModelIndex &index, int role ) const;
	Qt::ItemFlags flags( const QModelIndex &index ) const;
	QVariant headerData( int section, Qt::Orientation orientation,
						 int role = Qt::DisplayRole ) const;
	QModelIndex index( int row, int column,
					   const QModelIndex &parent = QModelIndex() ) const;
	QModelIndex parent( const QModelIndex &child ) const;
	int rowCount( const QModelIndex &parent = QModelIndex() ) const;
	int columnCount( const QModelIndex &parent = QModelIndex() ) const;
private:
	QDomDocument m_domDocument;
	XmlPresentationItem * m_rootItem;
};

#endif /* __XMLPRESENTATIONITEM_H__ */
