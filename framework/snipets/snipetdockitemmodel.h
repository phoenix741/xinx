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

#ifndef SNIPETDOCKITEMMODEL_H
#define SNIPETDOCKITEMMODEL_H
#pragma once

// Xinx header
#include "snipets/basesnipetitemmodel.h"
#include "snipets/snipetlist.h"

// Qt header
#include <QString>
#include <QSqlDatabase>
#include <QSqlQueryModel>

/* SnipetDockItemModel */

class SnipetDockItemModel : public BaseSnipetItemModel {
	Q_OBJECT
public:
	virtual ~SnipetDockItemModel();

	virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const;
	virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
	virtual Qt::ItemFlags flags( const QModelIndex & index ) const;

	virtual Qt::DropActions supportedDropActions() const;
	virtual QStringList mimeTypes() const;
	virtual QMimeData * mimeData(const QModelIndexList &indexes) const;

	virtual QModelIndex mapFromSource ( const QModelIndex & sourceIndex ) const;
	virtual QModelIndex mapToSource ( const QModelIndex & proxyIndex ) const;

protected:
	friend class SnipetManager;
	SnipetDockItemModel( QSqlDatabase db, QObject * parent = 0 );

private:
	int proxyColumnToSource( int proxyColumn ) const;
	int sourceColumnToProxy( int sourceColumn ) const;
};

#endif // SNIPETDOCKMODELINDEX_H
