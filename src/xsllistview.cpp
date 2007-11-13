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
#include "globals.h"
#include "xsllistview.h"
#include "xslproject.h"

// Qt header
#include <QDebug>
#include <QDomElement>
#include <QFile>
#include <QMessageBox>
#include <QApplication>
#include <QIcon>
#include <QDir>
#include <QFileInfo>

/* XMLParserException */

XMLParserException::XMLParserException( const QString & message, int line, int column ) : FileContentException( message, line, column ) {
}

/* PrivateXSLFileContentImport */

class PrivateXSLFileContentImport {
public:
	PrivateXSLFileContentImport( XSLFileContentImport * parent );
	~PrivateXSLFileContentImport();
	
	QString m_filename;
private:
	XSLFileContentImport * m_parent;
};

PrivateXSLFileContentImport::PrivateXSLFileContentImport( XSLFileContentImport * parent ) : m_parent( parent ) {
	
}

PrivateXSLFileContentImport::~PrivateXSLFileContentImport() {
	
}

/* XSLFileContentImport */

XSLFileContentImport::XSLFileContentImport( FileContentElement * parent, const QString & filename, const QDomElement & node ) : XSLFileContentParser( parent, filename, node.lineNumber() ) {
	d = new PrivateXSLFileContentImport( this );
	d->m_filename = filename;
}

XSLFileContentImport::~XSLFileContentImport() {
	delete d;
}

const QString & XSLFileContentImport::filename() const {
	return d->m_filename;
}

XSLFileContentImport * XSLFileContentImport::createImportFromLocation( FileContentElement * parent, const QDomElement & node ) {
	if( ! global.m_project ) return NULL; 
	
	QString name = node.attribute( "href" );
	
	foreach( QString path, global.m_project->processedSearchPathList() ) {
		if( QFile::exists( QDir( path ).absoluteFilePath( name ) ) )
			return new XSLFileContentImport( parent, QDir( path ).absoluteFilePath( name ), node );
	}
	return NULL;
}

int XSLFileContentImport::rowCount() {
	if( XSLFileContentParser::rowCount() == 0 )
		loadFromFile( d->m_filename );
	return XSLFileContentParser::rowCount();
}

/* PrivateXSLFileContentParams */

class PrivateXSLFileContentParams {
public:
	PrivateXSLFileContentParams( XSLFileContentParams * parent );
	virtual ~PrivateXSLFileContentParams();
	
	QString m_value;
private:
	XSLFileContentParams * m_parent;
};

/* XSLFileContentParams */

XSLFileContentParams::XSLFileContentParams( FileContentElement * parent, const QDomElement & node )
					 : FileContentElement( parent, node.attribute( "name" ), node.lineNumber() ) {
	d = new PrivateXSLFileContentParams( this );
	d->m_value = node.attribute( "select", node.text() );
}

XSLFileContentParams::~XSLFileContentParams() {
	delete d;
}

const QString & XSLFileContentParams::value() const {
	return d->m_value;
}

/* XSLFileContentVariable */

XSLFileContentVariable::XSLFileContentVariable( FileContentElement * parent, const QDomElement & node )
					   : XSLFileContentParams( parent, node ) {
	
}

XSLFileContentVariable::~XSLFileContentVariable() {
	
}

/* PrivateXSLFileContentTemplate */

class PrivateXSLFileContentTemplate {
public:
	PrivateXSLFileContentTemplate( XSLFileContentTemplate * parent );
	~PrivateXSLFileContentTemplate();
	
	QList<FileContentElement*> m_imports;
	QList<XSLFileContentParams*> m_variables;
	
	QString m_mode;
private:
	XSLFileContentTemplate * m_parent;
};

PrivateXSLFileContentTemplate::PrivateXSLFileContentTemplate( XSLFileContentTemplate * parent ) : m_parent( parent ) {
	
}

PrivateXSLFileContentTemplate::~PrivateXSLFileContentTemplate() {
	qDeleteAll( m_imports );
	qDeleteAll( m_variables );
}

/* XSLFileContentTemplate */

XSLFileContentTemplate::XSLFileContentTemplate( FileContentElement * parent, const QDomElement & node, const QString & name )
					   : FileContentElement( parent, name, node.lineNumber() ) {
	d = new PrivateXSLFileContentTemplate( this );
	d->m_mode = node.attribute( "mode" );
	
	QDomNodeList list = node.elementsByTagName( "script" );
	for( int i = 0 ; i < list.count(); i++ ) {
		QDomElement child = list.at( i ).toElement();
/*		if( child.attribute( "type" ).toLower().contains( "javascript" ) && (!child.attribute( "src" ).isEmpty()) ) 
			d->m_imports.append( new XSLFileContentImportJavaScript( this, child ) );*/
	}
	
	list = node.elementsByTagName( "variable" );
	for( int i = 0 ; i < list.count(); i++ ) 
		d->m_variables.append( new XSLFileContentVariable( this, list.at( i ).toElement() ) );
	
	list = node.elementsByTagName( "param" );
	for( int i = 0 ; i < list.count(); i++ ) 
		d->m_variables.append( new XSLFileContentParams( this, list.at( i ).toElement() ) );
}

const QString & XSLFileContentTemplate::mode() const {
	return d->m_mode;
}

XSLFileContentTemplate::~XSLFileContentTemplate() {
	delete d;
}

QList<XSLFileContentParams*> & XSLFileContentTemplate::variables() {
	return d->m_variables;
}

QList<FileContentElement*> & XSLFileContentTemplate::imports() {
	return d->m_imports;
}

int XSLFileContentTemplate::rowCount() {
	return d->m_imports.size() + d->m_variables.size();
}

FileContentElement * XSLFileContentTemplate::element( int index ) {
	if( index < d->m_imports.size() ) 
		return d->m_imports.at( index );
	else
	if( ( index - d->m_imports.size() ) < d->m_variables.size() ) 
		return d->m_variables.at( index - d->m_imports.size() );
	else
		return NULL;
}

/* PrivateXSLFileContentTemplate */

class PrivateXSLFileContentParser {
public:
	PrivateXSLFileContentParser( XSLFileContentParser * parent );
	~PrivateXSLFileContentParser();
	
	QList<FileContentElement*> m_imports;
	QList<XSLFileContentParams*> m_variables;
	QList<XSLFileContentTemplate*> m_templates;
private:
	XSLFileContentParser * m_parent;
};

PrivateXSLFileContentParser::PrivateXSLFileContentParser( XSLFileContentParser * parent ) : m_parent( parent ) {
	
}

PrivateXSLFileContentParser::~PrivateXSLFileContentParser() {
	qDeleteAll( m_imports );
	qDeleteAll( m_variables );
	qDeleteAll( m_templates );
}

/* XSLFileContentParser */

XSLFileContentParser::XSLFileContentParser( const QString & content ) : FileContentElement( NULL, QString(), 0 ) {
	d = new PrivateXSLFileContentParser( this );
	loadFromContent( content );
}

XSLFileContentParser::XSLFileContentParser( FileContentElement * parent, const QString & filename, int lineNumber ) : FileContentElement( parent, QFileInfo( filename ).fileName(), lineNumber ) {
	d = new PrivateXSLFileContentParser( this );
	loadFromFile( filename );
}


XSLFileContentParser::~XSLFileContentParser() {
	delete d;
}

void XSLFileContentParser::loadFromFile( const QString & filename ) {
	QFile file( filename );
	QDomDocument xsl;

	// Open the file
	if (!file.open(QFile::ReadOnly | QFile::Text)) 
		throw XMLParserException( QObject::tr("Cannot read file %1:\n%2.").arg(filename).arg(file.errorString()), 0, 0 );

	QTextStream text( &file );
	QString content = text.readAll();
	loadFromContent( content );
}

void XSLFileContentParser::loadFromContent( const QString & content ) {
	QDomDocument xsl;

	// Load XML Document
	QString errorStr;
	int errorLine = 0;
	int errorColumn = 0;  
	if( xsl.setContent(content, true, &errorStr, &errorLine, &errorColumn) ) {
		QDomElement root = xsl.documentElement();
		if( root.prefix() == "xsl" && root.tagName() == "stylesheet" )	
			loadFromXML( root );
	} else 
		throw XMLParserException( errorStr, errorLine, errorColumn );
	
}

void XSLFileContentParser::loadFromXML( const QDomElement & element ) {
	QDomElement child = element.firstChildElement();
	while (! child.isNull()) {
		if( child.prefix() != "xsl" ) continue;
		
		if( ( child.tagName() == "import" ) || ( child.tagName() == "include" ) ) 
			d->m_imports.append( XSLFileContentImport::createImportFromLocation( this, child ) );
		else if( child.tagName() == "variable" )
			d->m_variables.append( new XSLFileContentVariable( this, child ) );
		else if( child.tagName() == "variable" )  
			d->m_variables.append( new XSLFileContentParams( this, child ) );
		else if( child.tagName() == "template" ) {
			QStringList list = child.attribute( "name", child.attribute( "match" ) ).split( "|", QString::SkipEmptyParts );
			foreach( QString template_name, list )
				d->m_templates.append( new XSLFileContentTemplate( this, child, template_name ) );
		}
		
		child = child.nextSiblingElement();
	} 
}

QList<XSLFileContentTemplate*> & XSLFileContentParser::templates() {
	return d->m_templates;
}

QList<XSLFileContentParams*> & XSLFileContentParser::variables() {
	return d->m_variables;
}

QList<FileContentElement*> & XSLFileContentParser::imports() {
	return d->m_imports;
}

int XSLFileContentParser::rowCount() {
	return d->m_imports.size() + d->m_variables.size() + d->m_templates.size();
}

FileContentElement * XSLFileContentParser::element( int index ) {
	if( index < d->m_imports.size() ) 
		return d->m_imports.at( index );
	else
	if( ( index - d->m_imports.size() ) < d->m_variables.size() ) 
		return d->m_variables.at( index - d->m_imports.size() );
	else
	if( ( index - d->m_imports.size() - d->m_variables.size() ) < d->m_templates.size() ) 
		return d->m_templates.at( index - d->m_imports.size() - d->m_variables.size() );
	else
		return NULL;
}

/* XSLModelData */

XSLModelData::XSLModelData( XSLModelData * orig ) : QObject( orig ), m_parent( orig ) { 
	m_name         = QString();
	m_value        = QString();
	m_type         = etVariable;
	m_fileName     = QString();
	m_documentLine = -1;

	if( m_parent ) { 
		m_fileName = m_parent->m_fileName; 
	}
}

bool XSLModelData::contains( XSLModelData * data ) {
	for( int i = 0; i < m_child.count(); i++ ) {
		if( *(m_child.at( i )) == *data ) 
			return true;
	}
	return false;
}

bool XSLModelData::operator< ( const XSLModelData & cmp ) const { 
	return m_name < cmp.m_name; 
}

bool XSLModelData::operator== ( const XSLModelData & cmp ) const { 
	return m_name == cmp.m_name; 
}

XSLModelData * XSLModelData::addXSLModelData( XSLModelData * data ) {
	m_child.append( data );
	return data;
}

XSLModelData * XSLModelData::createVariable( const QDomElement & element ) {
	XSLModelData * data = new XSLModelData( this );
	data->setType( etVariable );
	data->setName( element.attribute( "name" ) );
	data->setValue( element.attribute( "select", element.text() ) );
	data->setLine( element.lineNumber() );
	return data;
}

void XSLModelData::loadFromElement( const QDomElement& element ) {
	QDomNodeList list = element.elementsByTagName( "script" );
	for( int i = 0 ; i < list.count(); i++ ) {
		QDomElement child = list.at( i ).toElement();
		if( child.attribute( "type" ).toLower().contains( "javascript" ) && (!child.attribute( "src" ).isEmpty()) ) {
			XSLModelData * data = new XSLModelData( this );
			data->setType( etJavascript );   				
			data->setName( child.attribute( "src" ) );
			data->setLine( child.lineNumber() );
			addXSLModelData( data );
		}
	}
	
	list = element.elementsByTagName( "variable" );
	for( int i = 0 ; i < list.count(); i++ ) 
		addXSLModelData( createVariable( list.at( i ).toElement() ) );

	list = element.elementsByTagName( "param" );
	for( int i = 0 ; i < list.count(); i++ ) 
		addXSLModelData( createVariable( list.at( i ).toElement() ) );
}

void XSLModelData::loadFromXML( const QDomElement& element ) {
	qDeleteAll( m_child );
	m_child.clear();		

	QDomElement child = element.firstChildElement();
  
	while (! child.isNull()) {
		if( child.prefix() != "xsl" ) continue;
		
		if( ( child.tagName() == "import" ) || ( child.tagName() == "include" ) ) {
			XSLModelData * data = new XSLModelData( this );
			data->setType( etImport );
  			data->setName( child.attribute( "href" ) );
			data->setLine( child.lineNumber() );
			addXSLModelData( data );
		} else
		if( ( child.tagName() == "variable" ) || ( child.tagName() == "param" ) )  {
			addXSLModelData( createVariable( child ) );
		} else
		if( child.tagName() == "template" ) {
			QStringList list = child.attribute( "name", child.attribute( "match" ) ).split( "|", QString::SkipEmptyParts );

			foreach( QString template_name, list ) {
				XSLModelData * data = new XSLModelData( this );
				data->setType( etTemplate );   				
	  			data->setName( template_name.trimmed() );
	  			data->setValue( child.attribute( "mode" ) );
				data->setLine( child.lineNumber() );
				data = addXSLModelData( data );
				data->loadFromElement( child );
			}
		}
		
		child = child.nextSiblingElement();
	} 
}

void XSLModelData::loadFromFile( const QString & filename ) {
	QFile file( filename );
	QDomDocument xsl;
	m_fileName = filename;
	
	// Open the file
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		throw XMLParserException( QObject::tr("Cannot read file %1:\n%2.").arg(filename).arg(file.errorString()), 0, 0 );
	    return;
	}
	QTextStream text( &file );
	QString content = text.readAll();
	loadFromContent( content );
}

void XSLModelData::loadFromContent( const QString & content ) {
	if( content.trimmed().isEmpty() ) {
		qDeleteAll( m_child );
		m_child.clear();		
		return;		
	}
	
	QDomDocument xsl;
	//m_fileName = QString();

	// Load XML Document
	QString errorStr;
	int errorLine = 0;
	int errorColumn = 0;  
	if (xsl.setContent(content, true, &errorStr, &errorLine, &errorColumn)) {
		QDomElement root = xsl.documentElement();
		if( root.prefix() == "xsl" && root.tagName() == "stylesheet" )	
			loadFromXML( root );
	} else {
		qDeleteAll( m_child );
		m_child.clear();		
		throw XMLParserException( errorStr, errorLine, errorColumn );
	}
}

int XSLModelData::childCount() { 
	try {
		if( global.m_project && ( m_child.size() == 0 ) && ( ( m_type == etImport ) || ( m_type == etJavascript ) ) ) {
			foreach( QString path, global.m_project->processedSearchPathList() ) {
				if( QFile::exists( QDir( path ).absoluteFilePath( m_name ) ) ) {
					loadFromFile( QDir( path ).absoluteFilePath( m_name ) );
					break;
				}
			}
		}
	} catch( XMLParserException e ) {
	}
	return m_child.size();
};

/* XSLItemModel */

XSLItemModel::XSLItemModel( QObject *parent ) : FileContentItemModel( parent ) {
	rootItem = new XSLModelData( NULL );
	toDelete = true;
}

XSLItemModel::XSLItemModel( XSLModelData * data, QObject *parent ) : FileContentItemModel( parent ) {
	rootItem = data;
	toDelete = false;
}

XSLItemModel::~XSLItemModel() {
	if( toDelete )
		delete rootItem;
}
	
QVariant XSLItemModel::data( const QModelIndex &index, int role ) const {
	if (!index.isValid()) return QVariant();

	XSLModelData * data = static_cast<XSLModelData*>(index.internalPointer());
	
	if(role == Qt::DecorationRole && index.column() == 0) {
		switch( data->type() ) {
		case XSLModelData::etImport:
			return QIcon(":/images/import.png");
			break;
		case XSLModelData::etVariable:
			return QIcon(":/images/variable.png");
			break;
		case XSLModelData::etTemplate:
			return QIcon(":/images/template.png");
			break;
		case XSLModelData::etJavascript:
			return QIcon(":/images/typejs.png");
			break;
		default:
			return QVariant();
		}
	} 
	
	if ( role == Qt::UserRole ) {
		struct FileContentItemModel::struct_file_content ud;
		ud.line = data->line();
		ud.filename = data->filename();
		return QVariant::fromValue(ud);	
	}
	
	if ( role == Qt::DisplayRole ) 
		switch(index.column()) {
		case 0: 
			return data->name();
			break;
		case 1:
			return data->value();
			break;
		default:
			return QVariant();
		}
	
	return QVariant();
}

Qt::ItemFlags XSLItemModel::flags(const QModelIndex &index) const {
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant XSLItemModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		switch(section) {
		case 0: 
			return "Name";
			break;
		case 1:
			return "Value";
			break;
		default:
			return QVariant();
		}
		
	return QVariant();
}

QModelIndex XSLItemModel::index(int row, int column, const QModelIndex &parent) const {
	XSLModelData *parentItem;

	if (!parent.isValid())
		parentItem = rootItem;
	else
		parentItem = static_cast<XSLModelData*>(parent.internalPointer());

	XSLModelData *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

QModelIndex XSLItemModel::parent(const QModelIndex &index) const {
	if (!index.isValid()) return QModelIndex();

	XSLModelData * childItem = static_cast<XSLModelData*>(index.internalPointer());
	XSLModelData * parentItem = childItem->parent();

	if (parentItem == rootItem)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

int XSLItemModel::rowCount(const QModelIndex &parent) const {
	XSLModelData *parentItem;

	if (!parent.isValid())
		parentItem = rootItem;
	else
		parentItem = static_cast<XSLModelData*>(parent.internalPointer());

	return parentItem->childCount();
}

int XSLItemModel::columnCount(const QModelIndex &parent) const {
	Q_UNUSED(parent);
	
	return 2; // Name & Value
}

