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
 
#include "globals.h"
#include "xsllistview.h"
#include "xslproject.h"

#include <QDebug>
#include <QDomElement>
#include <QFile>
#include <QMessageBox>
#include <QApplication>
#include <QIcon>
#include <QDir>

/* 	JavaScriptParserException */

XMLParserException::XMLParserException( const QString & message, int line ) : FileContentException( message, line ) {
	qDebug() << QObject::tr("Error %1 at line %2").arg( message ).arg( line ) << endl;
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

void XSLModelData::loadFromElement( const QDomElement& element ) {
	QDomNodeList list = element.elementsByTagName( "script" );
	for( int i = 0 ; i < list.count(); i++ ) {
		QDomElement child = list.at( i ).toElement();
		if( child.attribute( "type" ).toLower().contains( "javascript" ) && (!child.attribute( "src" ).isEmpty()) ) {
			XSLModelData * data = new XSLModelData( this );
			data->setType( etJavascript );   				
			data->setName( child.attribute( "src" ) );
			data->setLine( child.lineNumber() );
			m_child.append( data );
		}
	}
	
	list = element.elementsByTagName( "variable" );
	for( int i = 0 ; i < list.count(); i++ ) {
		QDomElement child = list.at( i ).toElement();

		XSLModelData * data = new XSLModelData( this );
		data->setType( etVariable );
		data->setName( child.attribute( "name" ) );
		data->setValue( child.attribute( "select", child.text() ) );
		data->setLine( child.lineNumber() );
		m_child.append( data );
	}

	list = element.elementsByTagName( "param" );
	for( int i = 0 ; i < list.count(); i++ ) {
		QDomElement child = list.at( i ).toElement();

		XSLModelData * data = new XSLModelData( this );
		data->setType( etVariable );
		data->setName( child.attribute( "name" ) );
		data->setValue( child.attribute( "select", child.text() ) );
		data->setLine( child.lineNumber() );
		m_child.append( data );
	}
}

void XSLModelData::loadFromXML( const QDomElement& element ) {
	emit childAboutToBeReset();
	
	qDeleteAll( m_child );
	m_child.clear();
	
	QDomElement child = element.firstChildElement();
  
	while (! child.isNull()) {
		if( child.prefix() == "xsl" && ( child.tagName() == "import" || child.tagName() == "variable" || child.tagName() == "template" || child.tagName() == "param" ) ) {
  			if( child.tagName() == "import" ) {
				XSLModelData * data = new XSLModelData( this );
				data->setType( etImport );
	  			data->setName( child.attribute( "href" ) );
				data->setLine( child.lineNumber() );
				m_child.append( data );
			} else
  			if( ( child.tagName() == "variable" ) || ( child.tagName() == "param" ) )  {
				XSLModelData * data = new XSLModelData( this );
  				data->setType( etVariable );
	  			data->setName( child.attribute( "name" ) );
	  			data->setValue( child.attribute( "select", child.text() ) );
				data->setLine( child.lineNumber() );
	  			m_child.append( data );
			} else
  			if( child.tagName() == "template" ) {
  				QStringList list = child.attribute( "name", child.attribute( "match" ) ).split( "|", QString::SkipEmptyParts );

				foreach( QString template_name, list ) {
					XSLModelData * data = new XSLModelData( this );
					data->setType( etTemplate );   				
		  			data->setName( template_name.trimmed() );
		  			data->setValue( child.attribute( "mode" ) );
					data->setLine( child.lineNumber() );
					data->loadFromElement( child );
		  			m_child.append( data );
				}
			}
 		}
		child = child.nextSiblingElement();
	} 
	
	emit childReseted();
}

void XSLModelData::loadFromFile( const QString& filename ) {
	QFile file( filename );
	QDomDocument xsl;
	m_fileName = filename;
	
	// Open the file
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		throw XMLParserException( QObject::tr("Cannot read file %1:\n%2.").arg(filename).arg(file.errorString()), 0 );
	    return;
	}
	
	// Load XML Document
	QString errorStr;
	int errorLine;
	int errorColumn;  
	if (xsl.setContent(&file, true, &errorStr, &errorLine, &errorColumn)) {
		QDomElement root = xsl.documentElement();
		if( root.prefix() == "xsl" && root.tagName() == "stylesheet" )	
			loadFromXML( root );
	} else {
		emit childAboutToBeReset();
		qDeleteAll( m_child );
		m_child.clear();		
		emit childReseted();
		throw XMLParserException( tr("Parse error column %1:%2").arg(errorColumn).arg(errorStr), errorLine );
	}
}

void XSLModelData::loadFromContent( const QString& content ) {
	if( content.trimmed().isEmpty() ) {
		emit childAboutToBeReset();
		qDeleteAll( m_child );
		m_child.clear();		
		emit childReseted();
		return;		
	}
	
	QDomDocument xsl;
	m_fileName = QString();

	// Load XML Document
	QString errorStr;
	int errorLine = 0;
	int errorColumn = 0;  
	if (xsl.setContent(content, true, &errorStr, &errorLine, &errorColumn)) {
		QDomElement root = xsl.documentElement();
		if( root.prefix() == "xsl" && root.tagName() == "stylesheet" )	
			loadFromXML( root );
		emit hasError( "" );
	} else {
		emit childAboutToBeReset();
		qDeleteAll( m_child );
		m_child.clear();		
		emit childReseted();
		throw XMLParserException( tr("Parse error column %1:%2").arg(errorColumn).arg(errorStr), errorLine );
	}  
}

int XSLModelData::childCount() { 
	if( global.m_project && ( m_child.size() == 0 ) && ( ( m_type == etImport ) || ( m_type == etJavascript ) ) ) {
		if( QFile::exists( QDir( global.m_project->specifPath() ).absoluteFilePath( m_name ) ) ) {
			loadFromFile( QDir( global.m_project->specifPath() ).absoluteFilePath( m_name ) );
		} else
		if( QFile::exists( QDir( global.m_project->navPath() ).absoluteFilePath( m_name ) ) ) {
			loadFromFile( QDir( global.m_project->navPath() ).absoluteFilePath( m_name ) );
		} else
		if( QFile::exists( QDir( global.m_project->projectPath() ).absoluteFilePath( m_name ) ) ) {
			loadFromFile( QDir( global.m_project->projectPath() ).absoluteFilePath( m_name ) );
		} else 
		if( QFile::exists( QDir( global.m_project->languePath() ).absoluteFilePath( m_name ) ) ) {
			loadFromFile( QDir( global.m_project->languePath() ).absoluteFilePath( m_name ) );
		} else 
		if( QFile::exists( QDir( global.m_project->languesPath() ).absoluteFilePath( m_name ) ) ) {
			loadFromFile( QDir( global.m_project->languesPath() ).absoluteFilePath( m_name ) );
		} 
	}
	return m_child.size();
};



XSLItemModel::XSLItemModel( QObject *parent ) : FileContentItemModel( parent ) {
	rootItem = new XSLModelData( NULL );
	connect( rootItem, SIGNAL( childAboutToBeReset() ), this, SIGNAL( modelAboutToBeReset() ) );
	connect( rootItem, SIGNAL( childReseted() ), this, SLOT( slotReset() ) );
	toDelete = true;
}

XSLItemModel::XSLItemModel( XSLModelData * data, QObject *parent ) : FileContentItemModel( parent ) {
	rootItem = data;
	connect( rootItem, SIGNAL( childAboutToBeReset() ), this, SIGNAL( modelAboutToBeReset() ) );
	connect( rootItem, SIGNAL( childReseted() ), this, SLOT( slotReset() ) );
	toDelete = false;
}

XSLItemModel::~XSLItemModel() {
	disconnect( rootItem, SIGNAL( childAboutToBeReset() ), this, SIGNAL( modelAboutToBeReset() ) );
	disconnect( rootItem, SIGNAL( childReseted() ), this, SLOT( slotReset() ) );
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
