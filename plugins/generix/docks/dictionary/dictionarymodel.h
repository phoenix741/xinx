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

#ifndef DICTIONARYMODEL_H
#define DICTIONARYMODEL_H
#pragma once

// Qt header
#include <QAbstractItemModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlDatabase>

class DictionaryModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	DictionaryModel(QString uuid, QObject * parent = 0);
	virtual ~DictionaryModel();

	virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	virtual QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex & index) const;
	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;

	void select( QSqlDatabase db, const QString & filter, const QString & order );
private:
	mutable QSqlQuery m_query;
	int m_size;

	QString m_uuid;
};

#endif // DICTIONARYMODEL_H
