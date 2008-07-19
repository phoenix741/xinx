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

// Qt header
#include <QIcon>

/* XSLCompletionModel */

XSLCompletionModel::XSLCompletionModel( CompletionTags tags, FileContentElementList * list, QObject * parent ) : QAbstractListModel( parent ), m_list( list ), m_tags( tags ) {
	if( m_list ) {
		connect( m_list, SIGNAL(aboutToAdd(int)), this, SLOT(beginInsertRows(int)) );
		connect( m_list, SIGNAL(added()), this, SLOT(endInsertRows()) );
		connect( m_list, SIGNAL(aboutToRemove(int)), this, SLOT(beginRemoveRows(int)) );
		connect( m_list, SIGNAL(removed()), this, SLOT(endRemoveRows()) );
		connect( m_list, SIGNAL(reset()), this, SLOT(reset()) );
	}
}

XSLCompletionModel::~XSLCompletionModel() {
}

QVariant XSLCompletionModel::data( const QModelIndex &index, int role ) const {
	if( !( index.isValid() && index.column() == 0 )) return QVariant();

	int value_row = index.row();
	if( ! ( m_baliseName.isEmpty() || m_attributeName.isEmpty() ) ) { // Complete value
		if( m_list && ( value_row < m_list->list().count() ) ) {
			FileContentElement * data = m_list->list().at( value_row );

			switch( role ) {
			case Qt::DecorationRole:
				return data->icon();
			case Qt::DisplayRole:
				return data->displayName();
			case Qt::UserRole:
				return data->metaObject()->className();
			}
		} else {
			value_row = index.row();
			if( m_list ) value_row -= m_list->list().count();

			if( xmlCompletionContents && xmlCompletionContents->balise( m_baliseName ) && xmlCompletionContents->balise( m_baliseName )->attribute( m_attributeName ) ) {
				switch( role ) {
				case Qt::DecorationRole:
					return QIcon(":/images/html_value.png");
				case Qt::DisplayRole:
					return xmlCompletionContents->balise( m_baliseName )->attribute( m_attributeName )->values().at( value_row );
				case Qt::UserRole:
					return -1;
				}
			}
		}
	} else if( ! m_baliseName.isEmpty() ) { // Complete attribute
		if( xmlCompletionContents && xmlCompletionContents->balise( m_baliseName ) && ( value_row < xmlCompletionContents->balise( m_baliseName )->attributes().size() ) ) {
			switch( role ) {
			case Qt::DecorationRole:
				return QIcon(":/images/noeud.png");
			case Qt::DisplayRole:
				return xmlCompletionContents->balise( m_baliseName )->attributes().at( value_row )->name();
			}
		}
	} else { // Complete balise
		if( xmlCompletionContents && ( ( m_tags.testFlag( Html ) && ( value_row < xmlCompletionContents->htmlBalises().size() )  ) || ( value_row < xmlCompletionContents->xmlBalises().size() ) ) ) {
			switch( role ) {
			case Qt::DecorationRole:
				return QIcon(":/images/balise.png");
			case Qt::DisplayRole:
				if( m_tags.testFlag( Html ) )
					return xmlCompletionContents->htmlBalises().at( value_row )->name();
				else
					return xmlCompletionContents->xmlBalises().at( value_row )->name();
			}
		}
	}

	return QVariant();
}

Qt::ItemFlags XSLCompletionModel::flags( const QModelIndex &index ) const {
	if ( index.isValid() )
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	return Qt::ItemIsEnabled;
}

int XSLCompletionModel::rowCount( const QModelIndex &parent ) const {
	if( ! parent.isValid() ) {
		int size = 0;
		if( m_list && !( m_baliseName.isEmpty() || m_attributeName.isEmpty() ) ) size += m_list->list().count();
		if( ! ( m_baliseName.isEmpty() || m_attributeName.isEmpty() ) ) { // Complete value
			if( xmlCompletionContents && xmlCompletionContents->balise( m_baliseName ) && xmlCompletionContents->balise( m_baliseName )->attribute( m_attributeName ) ) {
				size += xmlCompletionContents->balise( m_baliseName )->attribute( m_attributeName )->values().count();
			}
		} else if( ! m_baliseName.isEmpty() ) { // Complete attribute
			if( xmlCompletionContents && xmlCompletionContents->balise( m_baliseName ) )
				size += xmlCompletionContents->balise( m_baliseName )->attributes().count();
		} else {
			if( xmlCompletionContents ) { // Complate balise
				if( m_tags.testFlag( Html ) )
					size += xmlCompletionContents->htmlBalises().count();
				else
					size += xmlCompletionContents->xmlBalises().count();
			}
		}
		return size;
	} else
		return 0;
}

void XSLCompletionModel::setFilter( QString baliseName, QString attributeName ) {
	// TODO : Deux cas d'erreur : Si a = blanc..... !!!
	Q_ASSERT( ! baliseName.isEmpty() || attributeName.isEmpty() );
	if( ( m_baliseName != baliseName ) || ( m_attributeName != attributeName ) ) {
		m_baliseName = baliseName;
		m_attributeName = attributeName;
		reset();
	}
}

void XSLCompletionModel::beginInsertRows( int row ) {
	if( m_baliseName.isEmpty() || m_attributeName.isEmpty() ) return;
	QAbstractListModel::beginInsertRows( QModelIndex(), row, row );
}

void XSLCompletionModel::endInsertRows() {
	if( m_baliseName.isEmpty() || m_attributeName.isEmpty() ) return;
	QAbstractListModel::endInsertRows();
}

void XSLCompletionModel::beginRemoveRows( int row ) {
	if( m_baliseName.isEmpty() || m_attributeName.isEmpty() ) return;
	QAbstractListModel::beginRemoveRows( QModelIndex(), row, row );
}

void XSLCompletionModel::endRemoveRows() {
	if( m_baliseName.isEmpty() || m_attributeName.isEmpty() ) return;
	QAbstractListModel::endRemoveRows();
}

void XSLCompletionModel::reset() {
	QAbstractListModel::reset();
}
