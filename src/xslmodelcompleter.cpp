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
#include "xslmodelcompleter.h"
#include "xsllistview.h"
#include "globals.h"
#include "editorcompletion.h"

// Qt header
#include <QIcon>

/* XSLValueCompletionModel */

XSLValueCompletionModel::XSLValueCompletionModel( FileContentElement * data, QObject *parent ) : QAbstractListModel( parent ) {
	rootItem = data;
	refreshList();
	connect( rootItem, SIGNAL(aboutToAdd(FileContentElement*,int)), this, SLOT(addElement(FileContentElement*,int)) );
	connect( rootItem, SIGNAL(aboutToRemove(FileContentElement*)), this, SLOT(removeElement(FileContentElement*)) );
}

XSLValueCompletionModel::~XSLValueCompletionModel() {
}

bool XSLValueCompletionModel::contains( FileContentElement * data ) {
	for( int i = 0; i < m_objList.count(); i++ ) {
		if( m_objList.at( i )->equals( data ) ) 
			return true;
	}
	return false;
}

void XSLValueCompletionModel::refreshRecursive( FileContentElement * data ) {
	for( int i = 0; i < data->rowCount(); i++ ) {
		FileContentElement * e = data->element( i );
		if( dynamic_cast<XSLFileContentImport*>( e ) ) {
			QString name = e->name();
			if( ! m_files.contains( name ) ) { 
				m_files.append( name );
				refreshRecursive( e );
			}
		} else {
			if( ( dynamic_cast<XSLFileContentParams*>( e ) || dynamic_cast<XSLFileContentTemplate*>( e ) ) && ( ! contains( e ) ) )
				addElement( e );
		}
	}
}

void XSLValueCompletionModel::refreshList() {
	m_objList.clear();
	refreshRecursive( rootItem );
	qSort( m_objList.begin(), m_objList.end(), FileContentElementModelObjListSort );
	reset();
}

void XSLValueCompletionModel::addElement( FileContentElement* element ) {
	QList<FileContentElement*>::iterator i = qLowerBound( m_objList.begin(), m_objList.end(), element, FileContentElementModelObjListSort );
	int index = i - m_objList.begin();
	beginInsertRows( QModelIndex(), index, index );
	m_objList.insert( i, element );
	endInsertRows();
}

void XSLValueCompletionModel::addElement( FileContentElement* element, int row ) {
	Q_UNUSED( row );
	if( ( dynamic_cast<XSLFileContentParams*>( element ) || dynamic_cast<XSLFileContentTemplate*>( element ) ) && ( ! contains( element ) ) ) {
		addElement( element );
	}
	if( dynamic_cast<XSLFileContentImport*>( element ) ) {
		refreshRecursive( element );
	}
}

void XSLValueCompletionModel::removeElement( FileContentElement* element ) {
	int index = m_objList.indexOf( element );
	if( index >= 0 ) {
		beginRemoveRows( QModelIndex(), index, index );
		m_objList.removeAt( index );
		endRemoveRows();
	}
}

void XSLValueCompletionModel::setBaliseName( const QString & name, const QString & attribute ) { 
	int before = m_objList.count(), after  = m_objList.count();
	
	if( global.m_completionContents && global.m_completionContents->balise( m_baliseName ) && global.m_completionContents->balise( m_baliseName )->attribute( m_attributeName ) )
		before += global.m_completionContents->balise( m_baliseName )->attribute( m_attributeName )->values().count();
		
	if( global.m_completionContents && global.m_completionContents->balise( name ) && global.m_completionContents->balise( name )->attribute( attribute ) )
		after += global.m_completionContents->balise( name )->attribute( attribute )->values().count();
		
	int diff = after - before;

	if( diff > 0 ) 
		beginInsertRows( QModelIndex(), before + 1, after );
	else if( diff < 0 )
		beginRemoveRows( QModelIndex(), after + 1, before );

	m_baliseName = name; 
	m_attributeName = attribute; 

	if( diff > 0 ) {
		emit dataChanged( index( m_objList.count() ), index( before ) );
		endInsertRows();
	} else  if( diff < 0 ) {
		emit dataChanged( index( m_objList.count() ), index( after ) );
		endRemoveRows();
	}
}

	
QVariant XSLValueCompletionModel::data( const QModelIndex &index, int role ) const {
	if (!index.isValid()) return QVariant();

	if( index.row() < m_objList.count() ) {
		FileContentElement * data = m_objList[ index.row() ];
	
		if( role == Qt::DecorationRole ) {
			return data->icon();
		} else
		if ( ( role == Qt::DisplayRole ) && ( index.column() == 0 ) ) 
			return data->displayName();
		else
		if( role == Qt::UserRole ) {
			return data->metaObject()->className();
		}
	} else {
		int value_row = index.row() - m_objList.count(); 
		if( global.m_completionContents && global.m_completionContents->balise( m_baliseName ) && global.m_completionContents->balise( m_baliseName )->attribute( m_attributeName ) ) {
			if( role == Qt::DecorationRole ) 
				return QIcon(":/images/html_value.png");
			if ( ( role == Qt::DisplayRole ) && ( index.column() == 0 ) ) 
				return global.m_completionContents->balise( m_baliseName )->attribute( m_attributeName )->values().at( value_row );
			if( role == Qt::UserRole ) {
				return -1;
			}
		}
	}
	
	return QVariant();
}

Qt::ItemFlags XSLValueCompletionModel::flags(const QModelIndex &index) const {
	if ( index.isValid() )
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	return Qt::ItemIsEnabled;
}

int XSLValueCompletionModel::rowCount(const QModelIndex &parent) const {
	if ( ! parent.isValid() ) {
		int size = m_objList.count();	
		if( global.m_completionContents ) {
			if( global.m_completionContents->balise( m_baliseName ) ) {
				if( global.m_completionContents->balise( m_baliseName )->attribute( m_attributeName ) )
					size += global.m_completionContents->balise( m_baliseName )->attribute( m_attributeName )->values().count();
			}
		}
		return size;
	} else
		return 0;
	}

/* XSLParamCompletionModel */

XSLParamCompletionModel::XSLParamCompletionModel( QObject *parent ) : QAbstractListModel( parent ) {
}

XSLParamCompletionModel::~XSLParamCompletionModel() {
}

void XSLParamCompletionModel::setBaliseName( const QString & name ) { 
	emit layoutAboutToBeChanged();
	m_baliseName = name; 
	emit layoutChanged();
}

	
QVariant XSLParamCompletionModel::data( const QModelIndex &index, int role ) const {
	if (!index.isValid()) return QVariant();

	int value_row = index.row(); 
	if( global.m_completionContents && global.m_completionContents->balise( m_baliseName ) ) {
		if( role == Qt::DecorationRole ) 
			return QIcon(":/images/noeud.png");
		if ( ( role == Qt::DisplayRole ) && ( index.column() == 0 ) ) 
			return global.m_completionContents->balise( m_baliseName )->attributes().at( value_row )->name();
	}
	
	return QVariant();
}

Qt::ItemFlags XSLParamCompletionModel::flags(const QModelIndex &index) const {
	if ( index.isValid() )
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	return Qt::ItemIsEnabled;
}

int XSLParamCompletionModel::rowCount(const QModelIndex &parent) const {
	if ( ! parent.isValid() ) {
		int size = 0;	
		if( global.m_completionContents && global.m_completionContents->balise( m_baliseName ) ) 
			size += global.m_completionContents->balise( m_baliseName )->attributes().count();
		return size;
	} else
		return 0;
}

/* XSLBaliseCompletionModel */

XSLBaliseCompletionModel::XSLBaliseCompletionModel( QObject *parent ) : QAbstractListModel( parent ) {
}

XSLBaliseCompletionModel::~XSLBaliseCompletionModel() {
}

QVariant XSLBaliseCompletionModel::data( const QModelIndex &index, int role ) const {
	if (!index.isValid()) return QVariant();

	int value_row = index.row(); 
	if( global.m_completionContents ) {
		if( role == Qt::DecorationRole ) 
			return QIcon(":/images/balise.png");
		if ( ( role == Qt::DisplayRole ) && ( index.column() == 0 ) ) 
			return global.m_completionContents->xmlBalises().at( value_row )->name();
	}
	
	return QVariant();
}

Qt::ItemFlags XSLBaliseCompletionModel::flags(const QModelIndex &index) const {
	if ( index.isValid() )
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	return Qt::ItemIsEnabled;
}

int XSLBaliseCompletionModel::rowCount(const QModelIndex &parent) const {
	if ( ! parent.isValid() ) {
		int size = 0;	
		if( global.m_completionContents ) 
			size += global.m_completionContents->xmlBalises().count();
		return size;
	} else
		return 0;
}

