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
 
#ifndef _XSLLISTVIEW_H_
#define _XSLLISTVIEW_H_

#include "filecontentitemmodel.h"

/*!
 * This exception is launch when the file can't be parsed.
 */
class XMLParserException : public FileContentException {
public:
	/*!
	 * Create the exception
	 * \param message The message to show.
	 * \param line Line where the exception is throw
	 */
	XMLParserException( const QString & message, int line, int column );
};

class QDomElement;
class XSLProject;

class XSLModelData : public QObject {
	Q_OBJECT
public:
	XSLModelData( XSLModelData * orig = 0 );
	virtual ~XSLModelData() { qDeleteAll( m_child ); };

	enum ElementType { etImport, etVariable, etTemplate, etJavascript };
	
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
	QDomElement loadFromContent( const QString& );
	void loadFromElement( const QDomElement& );
	
	bool operator< ( const XSLModelData & cmp ) const { return m_name < cmp.m_name; };
	bool operator== ( const XSLModelData & cmp ) const { return m_name == cmp.m_name; };
	bool contains( XSLModelData * data );
	
signals:
	void childReseted();
	void childAboutToBeReset();
private:
	XSLModelData * m_parent;
	enum ElementType m_type;
	QString m_name, m_value;
	
	int m_documentLine;
	QString m_fileName;

	QList<XSLModelData*> m_child;

friend class XSLItemModel;
};

class XSLItemModel : public FileContentItemModel {
	Q_OBJECT
public:
	XSLItemModel( QObject *parent = 0 );
	XSLItemModel( XSLModelData * data, QObject *parent = 0 );
	virtual ~XSLItemModel();
	
	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;	
	
	XSLModelData* modelData() { return rootItem; };
protected slots:
	void slotReset() { reset(); };
private:
	XSLModelData* rootItem;
	bool toDelete;
};

#endif
