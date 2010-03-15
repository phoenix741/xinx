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

#ifndef __FLATTREEVIEW_H__
#define __FLATTREEVIEW_H__
#pragma once

// Xinx header
#include <core/lib-config.h>

// Qt header
#include <QAbstractItemModel>
#include <QStringList>

class QDirModel;

/*!
 * Item model transform a tree Item model (QDirModel) on a Flat model. A flat model is a list.
 * Node in the tree is show as title, and leaf is show as it.
 */
class LIBEXPORT FlatModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	/*!
	 * Create a flat model based on a QDirModel and a root index (if we don't want show all the model).
	 * \param model Model on which the object is based.
	 * \param root  First index on which the model must start.
	 */
	FlatModel(QDirModel * model, QModelIndex root);
	/*!
	 * Destructor of the flat model.
	 */
	virtual ~FlatModel();

	virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	virtual Qt::ItemFlags flags(const QModelIndex & index) const;
	virtual QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex & index) const;
	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;

	/*!
	 * Return the index on QDirModel, for an index in the flat model.
	 * \param index Index in the flat model
	 * \return Index in the QDirModel
	 */
	QModelIndex mappingToSource(const QModelIndex & index);

private slots:
	void recalcPathList();
	void rowsInserted(const QModelIndex & parent, int start, int end);
	void rowsRemoved(const QModelIndex & parent, int start, int end);
private:
	void insertIntoPathList(QModelIndex index);

	QDirModel * m_model;
	QPersistentModelIndex m_root;
	QStringList m_pathList;
};

#endif // __FLATTREEVIEW_H__
