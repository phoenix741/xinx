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
 
#include "javascriptmodelcompeter.h"

XSLValueCompletionModel::XSLValueCompletionModel( XSLModelData * data, QObject *parent ) : QAbstractListModel( parent ) {
	rootItem = data;
	refreshList();
	
	connect( rootItem, SIGNAL( childAboutToBeReset() ), this, SIGNAL( modelAboutToBeReset() ) );
	connect( rootItem, SIGNAL( childReseted() ), this, SLOT( refreshList() ) );
}

XSLValueCompletionModel::~XSLValueCompletionModel() {
	disconnect( rootItem, SIGNAL( childAboutToBeReset() ), this, SIGNAL( modelAboutToBeReset() ) );
	disconnect( rootItem, SIGNAL( childReseted() ), this, SLOT( refreshList() ) );
}

bool XSLValueCompletionModel::contains( XSLModelData * data ) {
	for( int i = 0; i < m_objList.count(); i++ ) {
		if( *(m_objList.at( i )) == *data ) 
			return true;
	}
	return false;
}

void XSLValueCompletionModel::refreshRecursive(XSLModelData * data) {
	for( int i = 0; i < data->childCount(); i++ ) {
		if( data->child( i )->type() != XSLModelData::etImport ) {
			if( ! contains( data->child( i ) ) )
			m_objList.append( data->child( i ) );
		} else {
			refreshRecursive( data->child( i ) );
		}
	}
}

bool XSLValueCompletionModelObjListSort( XSLModelData * d1, XSLModelData * d2 ) {
	return d1->name() < d2->name();
}

void XSLValueCompletionModel::refreshList() {
	m_objList.clear();
	refreshRecursive( rootItem );
	qSort( m_objList.begin(), m_objList.end(), XSLValueCompletionModelObjListSort );
	reset();
}

void XSLValueCompletionModel::setBaliseName( const QString & name, const QString & attribute ) { 
	int before = m_objList.count(), after  = m_objList.count();
	
	if( completionContents && completionContents->balise( m_baliseName ) && completionContents->balise( m_baliseName )->attribute( m_attributeName ) )
		before += completionContents->balise( m_baliseName )->attribute( m_attributeName )->values().count();
		
	if( completionContents && completionContents->balise( name ) && completionContents->balise( name )->attribute( attribute ) )
		after += completionContents->balise( name )->attribute( attribute )->values().count();
		
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
		XSLModelData * data = m_objList[ index.row() ];
	
		if( role == Qt::DecorationRole ) {
			switch( data->type() ) {
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
	
		if ( ( role == Qt::DisplayRole ) && ( index.column() == 0 ) ) 
			return data->name();
		
		if( role == Qt::UserRole ) {
			return (int)data->type();
		}
	} else {
		int value_row = index.row() - m_objList.count(); 
		if( completionContents && completionContents->balise( m_baliseName ) && completionContents->balise( m_baliseName )->attribute( m_attributeName ) ) {
			if( role == Qt::DecorationRole ) 
				return QIcon(":/images/html_value.png");
			if ( ( role == Qt::DisplayRole ) && ( index.column() == 0 ) ) 
				return completionContents->balise( m_baliseName )->attribute( m_attributeName )->values().at( value_row );
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
		if( completionContents ) {
			if( completionContents->balise( m_baliseName ) ) {
				if( completionContents->balise( m_baliseName )->attribute( m_attributeName ) )
					size += completionContents->balise( m_baliseName )->attribute( m_attributeName )->values().count();
			}
		}
		return size;
	} else
		return 0;
	}
