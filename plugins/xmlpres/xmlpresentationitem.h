/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

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
	XmlPresentationItem( QDomNode node, int row, XmlPresentationItem * parent = 0 );
	virtual ~XmlPresentationItem();
	
	int count();
	XmlPresentationItem * child( int i );
	XmlPresentationItem * parent();
	int row();
	
	QString xpath( bool unique = true ) const;
	virtual QString tipsText() const;
	virtual QString xpathName( bool unique = true ) const;

	QString businessData() const;
	QString screenData() const;
	QString errorData() const;
	
	QDomNode node() const;
protected:
	QString m_businessData, m_screenData, m_error;
	QDomNode m_domNode;
	QList<XmlPresentationItem*> m_childItems;
	XmlPresentationItem * m_parentItem;
	int m_rowNumber;
};

class XmlPresentationNodeItem : public XmlPresentationItem {
public:
	XmlPresentationNodeItem( QDomNode node, int row, XmlPresentationItem * parent = 0 );
	virtual ~XmlPresentationNodeItem();

	virtual QString tipsText() const;
	virtual QString xpathName( bool unique = true ) const;
	bool isView() const;
};

class XmlPresentationParamItem : public XmlPresentationItem {
public:
	XmlPresentationParamItem( QDomNode node, int row, XmlPresentationItem * parent = 0 );
	virtual ~XmlPresentationParamItem();

	virtual QString xpathName( bool unique = true ) const;
	virtual QString tipsText() const;
	QString value() const;
};

class XmlPresentationModel : public QAbstractItemModel {
	Q_OBJECT
public:
	enum PresentationRole {
		XNamedPathRole = 32,
		XPathRole = 33,
		NamedViewRole = 34
	};
	
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

	QStringList mimeTypes() const;
	QMimeData * mimeData( const QModelIndexList &indexes ) const;
private:
	QDomNode m_rootElement;
	XmlPresentationItem * m_rootItem;
};

#endif /* __XMLPRESENTATIONITEM_H__ */
