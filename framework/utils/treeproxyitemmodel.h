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

#ifndef TREEPROXYITEMMODEL_H
#define TREEPROXYITEMMODEL_H
#pragma once

// Xinx header
#include <core/lib-config.h>

// Qt header
#include <QString>
#include <QAbstractProxyModel>
#include <QHash>
#include <QVector>

/* TreeProxyItemModel */

class LIBEXPORT TreeProxyItemModel : public QAbstractProxyModel
{
	Q_OBJECT
	Q_PROPERTY(bool resetModel READ resetModel WRITE setResetModel)
public:
	TreeProxyItemModel(QObject * parent = 0);
	virtual ~TreeProxyItemModel();

	virtual QModelIndex index(int id) const;
	virtual QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex & index) const;

	virtual QVariant data(const QModelIndex &proxyIndex, int role) const;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const;

	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;

	virtual QModelIndex mapFromSource(const QModelIndex & sourceIndex) const;
	virtual QModelIndex mapToSource(const QModelIndex & proxyIndex) const;

	bool resetModel() const;
	void setResetModel(bool value);
protected:
	virtual int getUniqueIdentifier(const QModelIndex & sourceIndex) const = 0;
	virtual int getParentUniqueIdentifier(const QModelIndex & sourceIndex) const = 0;

	virtual void setSourceModel(QAbstractItemModel * sourceModel);
	virtual void createMapping();
	void setParentId(int id, int parentId);

protected slots:
#ifndef _XINX_RELEASE_MODE_
	void printMapping(int id = 0, int niveau = 0) const;
#endif
private:
	struct Mapping
	{
		int id;                  //< Define the id of the line. This id have nothing to do with the line in the source model
		int parentId;            //< Define the parent id of this structure
		QVector<int> childs;     //< Define a list of id, child of this element
		QVector<int> new_childs; //< Structure used to know who are the new childs inserted
	};
	/*
	 * Map of id/structure.
	 * This map associate id with a stucture contains id, parentId, and rows.
	 */
	typedef QHash<int,Mapping*> IndexMap;

	// Map of id/structure
	mutable IndexMap m_idMapping;
	QHash<int,int> m_id2IndexMapping;
	QHash<int,int> m_index2IdMapping;

	int m_sourceColumnCount;
	bool m_resetModel;

	Mapping * getMapping(int id) const;
	Mapping * getMapping(const QModelIndex & index) const;
	void unassignId(Mapping * mapping);
	void assignId(Mapping * mapping, int parentId);
	void validAssignement(Mapping * mapping);
	bool isAttachedToRoot(Mapping * mapping);
};

#endif // TREEPROXYITEMMODEL_H
