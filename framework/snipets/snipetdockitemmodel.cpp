/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

// Xinx header
#include "snipetdockitemmodel.h"
#include "snipetmanager.h"
#include "editors/editormanager.h"
#include "editors/abstracteditor.h"

// // Qt header
#include <QIcon>
#include <QColor>
#include <QFont>
#include <QVariant>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QDebug>
#include <QMimeData>

/* SnipetDockItemModel */

SnipetDockItemModel::SnipetDockItemModel( QSqlDatabase db, QObject * parent ) : BaseSnipetItemModel( db, parent ) {

}

SnipetDockItemModel::~SnipetDockItemModel() {

}

int SnipetDockItemModel::proxyColumnToSource( int proxyColumn ) const {
	switch( proxyColumn ) {
	case 0 :
		return list_name;
	default:
		return -1;
	}
}

int SnipetDockItemModel::sourceColumnToProxy( int sourceColumn ) const {
	switch( sourceColumn ) {
	case list_name :
		return 0;
	default:
		return -1;
	}
}

/// For the given source index, this method return the corresponding index in the proxy
QModelIndex SnipetDockItemModel::mapFromSource ( const QModelIndex & sourceIndex ) const {
	QModelIndex index = mapFromSource( sourceIndex );
	int column = sourceColumnToProxy( index.column() );
	if( column == -1 ) return QModelIndex();

	return BaseSnipetItemModel::createIndex( index.row(), column, index.internalPointer() );
}

QModelIndex SnipetDockItemModel::mapToSource ( const QModelIndex & proxyIndex ) const {
	QModelIndex index = proxyIndex;
	int column = proxyColumnToSource( index.column() );
	if( column == -1 ) return QModelIndex();

	return BaseSnipetItemModel::mapToSource( createIndex( index.row(), column, index.internalPointer() ) );
}

int SnipetDockItemModel::columnCount( const QModelIndex & index ) const {
	Q_UNUSED( index );

	return 1;
}

Qt::ItemFlags SnipetDockItemModel::flags( const QModelIndex & index ) const {
	if( index.isValid() ) {
		QModelIndex sourceIndex = mapToSource( index );
		QSqlRecord record = sourceModel()->record( sourceIndex.row() );

		// Enable the snipet according to the script stored on the snipet or category
		QString availableScript = record.value( list_availablejs ).toString();
		if( ! SnipetManager::self()->isAvailable( availableScript, record.value( list_type ).toString(), record.value( list_id ).toInt() ) )
			return Qt::ItemIsSelectable;

		// The script is the only restriction for category
		if( record.value( list_type ).toString() == "CATEGORY" ) {
			if( index.parent().isValid() )
				return index.parent().flags();
			else
				return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
		}

		// If no editor open, we can't call snipet
		if( EditorManager::self() && EditorManager::self()->currentEditor() ) {
			// Get the filename in the editor
			QString filename = EditorManager::self()->currentEditor()->getTitle();

			if( SnipetManager::self()->isSnipetMatch( filename, record.value( list_id ).toInt() ) ) {
				if( index.parent().flags().testFlag( Qt::ItemIsEnabled ) )
					return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled;
			}
		}

		// The snipet can't be called
		return Qt::ItemIsSelectable;
	}
	return 0;
}

QStringList SnipetDockItemModel::mimeTypes() const {
	QStringList types = BaseSnipetItemModel::mimeTypes();
	types << "application/snipet.id.list";
	types << "text/plain";
	return types;
}

Qt::DropActions SnipetDockItemModel::supportedDropActions() const {
	return Qt::MoveAction;
}

QMimeData * SnipetDockItemModel::mimeData( const QModelIndexList &indexes ) const {
	QMimeData * mimeData = BaseSnipetItemModel::mimeData( indexes );
	QByteArray encodedData;
	QStringList names;
	QDataStream stream( &encodedData, QIODevice::WriteOnly );

	foreach( QModelIndex index, indexes ) {
		if( index.isValid() ) {
			QString type = data( index, SnipetTypeRole ).toString();
			QString name = data( index, Qt::DisplayRole ).toString();
			int id = data( index, SnipetIdRole ).toInt();
			stream << id << type << name;
			names += name;
		}
	}

	mimeData->setData( "application/snipet.id.list", encodedData );
	mimeData->setText( names.join( "," ) );
	return mimeData;
}

QVariant SnipetDockItemModel::data( const QModelIndex & index, int role ) const {
	if( ! index.isValid() ) return QVariant();


	if( role == Qt::DecorationRole ) {
		if( index.column() == 0 ) {
			QModelIndex sourceIndex = mapToSource( index );
			QSqlRecord record = sourceModel()->record( sourceIndex.row() );
			return QIcon( record.value( list_icon ).toString() );
		}
	} else if( role == Qt::BackgroundRole ) {
		QModelIndex sourceIndex = mapToSource( index );
		if( ! sourceIndex.isValid() ) return QVariant();

		QSqlRecord record = sourceModel()->record( sourceIndex.row() );
		if( record.value( list_type ).toString() == "CATEGORY" ) {
			return QColor( 0xFF, 0xFF, 0xCC );
		}
	} else if( role == Qt::FontRole ) {
		QModelIndex sourceIndex = mapToSource( index );
		if( ! sourceIndex.isValid() ) return QVariant();

		QSqlRecord record = sourceModel()->record( sourceIndex.row() );
		if( record.value( list_type ).toString() == "CATEGORY" ) {
			QFont currentFont;
			currentFont.setBold( true );
			return currentFont;
		}
	} else if( role == Qt::StatusTipRole ) {
		QModelIndex sourceIndex = mapToSource( index );
		if( ! sourceIndex.isValid() ) return QVariant();

		QSqlRecord record = sourceModel()->record( sourceIndex.row() );
		return record.value( list_description );
	}

	return BaseSnipetItemModel::data( index, role );
}
