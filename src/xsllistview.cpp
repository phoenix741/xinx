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
 
#include "xsllistview.h"
#include "xslproject.h"

#include <QDomElement>
#include <QFile>
#include <QMessageBox>
#include <QApplication>
#include <QIcon>

void XSLModelData::loadFromXML( const QDomElement& element, XSLProject * project ) {
	qDeleteAll( m_child );
	m_child.clear();
	
	QDomElement child = element.firstChildElement();
  
	while (! child.isNull()) {
		if( child.prefix() == "xsl" && ( child.tagName() == "import" || child.tagName() == "variable" || child.tagName() == "template" ) ) {
			XSLModelData * data = new XSLModelData( this );
			
  			if( child.tagName() == "import" ) {
				data->setType( etImport );
	  			data->setName( child.attribute( "href" ) );
			} else
  			if( child.tagName() == "variable" )  {
  				data->setType( etVariable );
	  			data->setName( child.attribute( "name" ) );
	  			data->setValue( child.attribute( "select", child.text() ) );
			} else
  			if( child.tagName() == "template" ) {
				data->setType( etTemplate );   				
	  			data->setName( child.attribute( "name", child.attribute( "match" ) ) );
	  			data->setValue( child.attribute( "mode" ) );
			}
			data->setLine( child.lineNumber() );
  			
  			m_child.append( data );
 		}
		child = child.nextSiblingElement();
	} 
	
	m_project = project;
}

void XSLModelData::loadFromFile( const QString& filename, XSLProject * project ) {
	QFile file(filename);
	QDomDocument xsl;
	m_fileName = filename;
	
	// Open the file
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		QMessageBox::warning(qApp->activeWindow(), QObject::tr("Completion"), QObject::tr("Cannot read file %1:\n%2.")
                                                        			 		 .arg(filename)
																	 		 .arg(file.errorString()));
	    return;
	}
	
	// Load XML Document
	QString errorStr;
	int errorLine;
	int errorColumn;  
	if (xsl.setContent(&file, true, &errorStr, &errorLine, &errorColumn)) {
		QDomElement root = xsl.documentElement();
		if( root.prefix() == "xsl" && root.tagName() == "stylesheet" )	
			loadFromXML( root, project );
	} else {
		qDeleteAll( m_child );
		m_child.clear();		
	}
}

void XSLModelData::loadFromContent( const QString& content, XSLProject * project ) {
	QDomDocument xsl;
	m_fileName = QString();

	// Load XML Document
	QString errorStr;
	int errorLine;
	int errorColumn;  
	if (xsl.setContent(content, true, &errorStr, &errorLine, &errorColumn)) {
		QDomElement root = xsl.documentElement();
		if( root.prefix() == "xsl" && root.tagName() == "stylesheet" )	
			loadFromXML( root, project );
	} else {
		qDeleteAll( m_child );
		m_child.clear();		
	}  
}

int XSLModelData::childCount() { 
	if( m_project && ( m_child.size() == 0 ) && ( m_type == etImport ) ) {
		if( QFile::exists( m_project->specifPath() + m_name ) ) {
			loadFromFile( m_project->specifPath() + m_name, m_project );
		} else
		if( QFile::exists( m_project->navPath() + m_name ) ) {
			loadFromFile( m_project->navPath() + m_name, m_project );
		} else
		if( QFile::exists( m_project->projectPath() + m_name ) ) {
			loadFromFile( m_project->projectPath() + m_name, m_project );
		} else 
		if( QFile::exists( m_project->languePath() + m_name ) ) {
			loadFromFile( m_project->languePath() + m_name, m_project );
		} 
	}
	return m_child.size();
};



XSLItemModel::XSLItemModel( QObject *parent ) : QAbstractItemModel(parent) {
	rootItem = new XSLModelData();
}

XSLItemModel::~XSLItemModel() {
	delete rootItem;
}
	
QVariant XSLItemModel::data(const QModelIndex &index, int role) const {
	if (!index.isValid()) return QVariant();

	XSLModelData * data = static_cast<XSLModelData*>(index.internalPointer());
	
	if(role == Qt::DecorationRole && index.column() == 0) {
		switch( data->type() ) {
		case XSLModelData::etImport:
			return QIcon(":/images/doc.png");
			break;
		case XSLModelData::etVariable:
			return QIcon(":/images/CVpublic_var.png");
			break;
		case XSLModelData::etTemplate:
			return QIcon(":/images/CVpublic_meth.png");
			break;
		default:
			return QVariant();
		}
	} 
	
	if ( role == Qt::UserRole ) {
		struct user_data ud;
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

void XSLItemModel::updateModel(const QString & filename, XSLProject * project ) {
	rootItem->loadFromContent( filename, project );
	reset();
}

