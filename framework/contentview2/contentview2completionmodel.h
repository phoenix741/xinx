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

#ifndef CONTENTVIEW2COMPLETIONMODEL_H
#define CONTENTVIEW2COMPLETIONMODEL_H

// Xinx header
#include <contentview2/contentview2node.h>

// Qt header
#include <QSqlQueryModel>

namespace ContentView2 {

class LIBEXPORT CompletionModel : public QSqlQueryModel {
	Q_OBJECT
public:
	enum CompletionModelRole { CompletionIdRole = 0x77 };

	CompletionModel( QSqlDatabase db, QObject * parent = 0 );
	virtual ~CompletionModel();

	virtual QVariant data( const QModelIndex &index, int role ) const;
	virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const;
	virtual bool setHeaderData( int section, Qt::Orientation orientation, const QVariant & value, int role = Qt::EditRole );

	void addWhereClause( const QString & whereClause );
	void addFile( uint file );
public slots:
	//! Launch the selection in the base
	virtual void select();

private:
	QSqlDatabase m_db;

	QList<int> m_filesId;
	QStringList m_whereClauses;
};

} // namespace ContentView2

#endif // CONTENTVIEW2COMPLETIONMODEL_H
