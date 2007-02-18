/****************************************************************************
 **
 ** Copyright (C) 2004-2006 Trolltech ASA. All rights reserved.
 **
 ** This file is part of the example classes of the Qt Toolkit.
 **
 ** This file may be used under the terms of the GNU General Public
 ** License version 2.0 as published by the Free Software Foundation
 ** and appearing in the file LICENSE.GPL included in the packaging of
 ** this file.  Please review the following information to ensure GNU
 ** General Public Licensing requirements will be met:
 ** http://www.trolltech.com/products/qt/opensource.html
 **
 ** If you are unsure which license is appropriate for your use, please
 ** review the following information:
 ** http://www.trolltech.com/products/qt/licensing.html or contact the
 ** sales department at sales@trolltech.com.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ****************************************************************************/
 
#ifndef _XSLLISTVIEW_H_
#define _XSLLISTVIEW_H_

#include <QObject>
#include <QAbstractItemModel>

class QDomElement;
class XSLProject;

class XSLModelData : public QObject {
	Q_OBJECT
public:
	XSLModelData( XSLModelData * orig = 0, XSLProject * project = 0 );
	virtual ~XSLModelData() { qDeleteAll( m_child ); };

	enum ElementType { etImport, etVariable, etTemplate };
	
	const ElementType & type() const { return m_type; };
	void setType( const ElementType & value ) { m_type = value; };
	
	const QString & name() const { return m_name; };
	void setName( const QString & value )  { m_name = value; };
	
	const QString & value() const { return m_value; };
	void setValue( const QString & value )  { m_value = value; };

	const int & line() const { return m_documentLine; };
	void setLine( int value )  { m_documentLine = value; };

	const QString & filename() const { return m_fileName; };
	void setFilename( const QString & value )  { m_fileName = value; };
	
	XSLModelData * parent() const { return m_parent; };
	int row() const { if( m_parent ) return m_parent->m_child.indexOf( const_cast<XSLModelData*>(this) ); return 0; };
	
	XSLModelData * child(int row) { return m_child.at( row ); };
	int childCount();
	void appendChild(XSLModelData * child) { m_child.append( child ); };
	
	void loadFromXML( const QDomElement& );
	void loadFromFile( const QString& );
	void loadFromContent( const QString& );
	
signals:
	void childReseted();
	
private:
	XSLModelData * m_parent;
	enum ElementType m_type;
	QString m_name, m_value;
	
	int m_documentLine;
	QString m_fileName;

	XSLProject * m_project;
	
	QList<XSLModelData*> m_child;

friend class XSLItemModel;
};

class XSLItemModel : public QAbstractItemModel {
	Q_OBJECT
public:
	XSLItemModel( QObject *parent = 0, XSLProject * project = 0 );
	XSLItemModel( XSLModelData * data, QObject *parent = 0 );
	virtual ~XSLItemModel();
	
	struct user_data {
		int line;
		QString filename;
	};
	
	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;	
	
	XSLModelData* modelData() { return rootItem; };
private:
	XSLModelData* rootItem;
	bool toDelete;
};

Q_DECLARE_METATYPE ( XSLItemModel::user_data )

#endif
