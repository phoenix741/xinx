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
 
#include "globals.h"
#include "xsllistview.h"
#include "xslproject.h"

#include <QDomElement>
#include <QFile>
#include <QMessageBox>
#include <QApplication>
#include <QIcon>
#include <QDir>

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
		emit hasError( QObject::tr("Cannot read file %1:\n%2.").arg(filename).arg(file.errorString()) );
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
		emit hasError( "" );
	} else {
		emit childAboutToBeReset();
		qDeleteAll( m_child );
		m_child.clear();		
		emit childReseted();
		emit hasError( tr("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr) );
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
		emit hasError( tr("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr) );
	}  
}

int XSLModelData::childCount() { 
	if( global.m_project && ( m_child.size() == 0 ) && ( m_type == etImport ) ) {
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
