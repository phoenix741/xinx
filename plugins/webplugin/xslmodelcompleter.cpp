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
#include "editorcompletion.h"
#include "javascriptparser.h"

// Qt header
#include <QIcon>

/* XSLValueCompletionModel */

XSLValueCompletionModel::XSLValueCompletionModel( FileContentElementList * data, QObject *parent ) : QAbstractListModel( parent ), m_list( data ) {
	connect( m_list, SIGNAL(aboutToAdd(int)), this, SLOT(beginInsertRows(int)) );
	connect( m_list, SIGNAL(added()), this, SLOT(endInsertRows()) );
	connect( m_list, SIGNAL(aboutToRemove(int)), this, SLOT(beginRemoveRows(int)) );
	connect( m_list, SIGNAL(removed()), this, SLOT(endRemoveRows()) );
	connect( m_list, SIGNAL(reset()), this, SLOT(reset()) );
}

XSLValueCompletionModel::~XSLValueCompletionModel() {
	
}

void XSLValueCompletionModel::beginInsertRows( int row ) {
	QAbstractListModel::beginInsertRows( QModelIndex(), row, row );
}

void XSLValueCompletionModel::endInsertRows() {
	QAbstractListModel::endInsertRows();
}

void XSLValueCompletionModel::beginRemoveRows( int row ) {
	QAbstractListModel::beginRemoveRows( QModelIndex(), row, row );
}

void XSLValueCompletionModel::endRemoveRows() {
	QAbstractListModel::endRemoveRows();	
}

void XSLValueCompletionModel::reset() {
	QAbstractListModel::reset();
}

void XSLValueCompletionModel::setBaliseName( const QString & name, const QString & attribute ) { 
	

	int before = m_list->list().count(), after  = m_list->list().count();
	
	if( xmlCompletionContents && xmlCompletionContents->balise( m_baliseName ) && xmlCompletionContents->balise( m_baliseName )->attribute( m_attributeName ) )
		before += xmlCompletionContents->balise( m_baliseName )->attribute( m_attributeName )->values().count();
		
	if( xmlCompletionContents && xmlCompletionContents->balise( name ) && xmlCompletionContents->balise( name )->attribute( attribute ) )
		after += xmlCompletionContents->balise( name )->attribute( attribute )->values().count();
		
	int diff = after - before;

	if( diff > 0 ) 
		QAbstractListModel::beginInsertRows( QModelIndex(), before + 1, after );
	else if( diff < 0 )
		QAbstractListModel::beginRemoveRows( QModelIndex(), after + 1, before );

	m_baliseName = name; 
	m_attributeName = attribute; 

	if( diff > 0 ) {
		emit dataChanged( index( m_list->list().count() ), index( before ) );
		QAbstractListModel::endInsertRows();
	} else  if( diff < 0 ) {
		emit dataChanged( index( m_list->list().count() ), index( after ) );
		QAbstractListModel::endRemoveRows();
	}
}

	
QVariant XSLValueCompletionModel::data( const QModelIndex &index, int role ) const {
	if (!index.isValid()) return QVariant();

	if( index.row() < m_list->list().count() ) {
		FileContentElement * data = m_list->list().at( index.row() );

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
		int value_row = index.row() - m_list->list().count(); 
		if( xmlCompletionContents && xmlCompletionContents->balise( m_baliseName ) && xmlCompletionContents->balise( m_baliseName )->attribute( m_attributeName ) ) {
			if( role == Qt::DecorationRole ) 
				return QIcon(":/images/html_value.png");
			if ( ( role == Qt::DisplayRole ) && ( index.column() == 0 ) ) 
				return xmlCompletionContents->balise( m_baliseName )->attribute( m_attributeName )->values().at( value_row );
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
		int size = m_list->list().count();	
		if( xmlCompletionContents ) {
			if( xmlCompletionContents->balise( m_baliseName ) ) {
				if( xmlCompletionContents->balise( m_baliseName )->attribute( m_attributeName ) )
					size += xmlCompletionContents->balise( m_baliseName )->attribute( m_attributeName )->values().count();
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
	if( xmlCompletionContents && xmlCompletionContents->balise( m_baliseName ) ) {
		if( role == Qt::DecorationRole ) 
			return QIcon(":/images/noeud.png");
		if ( ( role == Qt::DisplayRole ) && ( index.column() == 0 ) ) 
			return xmlCompletionContents->balise( m_baliseName )->attributes().at( value_row )->name();
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
		if( xmlCompletionContents && xmlCompletionContents->balise( m_baliseName ) ) 
			size += xmlCompletionContents->balise( m_baliseName )->attributes().count();
		return size;
	} else
		return 0;
}

/* XSLBaliseCompletionModel */

XSLBaliseCompletionModel::XSLBaliseCompletionModel( QObject *parent, bool onlyHtml ) : QAbstractListModel( parent ), m_onlyHtml( onlyHtml ) {
}

XSLBaliseCompletionModel::~XSLBaliseCompletionModel() {
}

QVariant XSLBaliseCompletionModel::data( const QModelIndex &index, int role ) const {
	if( !( index.isValid() && index.column() == 0 )) return QVariant();

	int value_row = index.row(); 
	if( xmlCompletionContents ) {
		if( role == Qt::DecorationRole ) 
			return QIcon(":/images/balise.png");
		if ( ( role == Qt::DisplayRole ) && ( index.column() == 0 ) )  {
			if( m_onlyHtml )
				return xmlCompletionContents->htmlBalises().at( value_row )->name();
			else
				return xmlCompletionContents->xmlBalises().at( value_row )->name();
		}
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
		if( xmlCompletionContents ) {
			if( m_onlyHtml )
				size += xmlCompletionContents->htmlBalises().count();
			else
				size += xmlCompletionContents->xmlBalises().count();
		}
		return size;
	} else
		return 0;
}

