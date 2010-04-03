/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2007-2010 by Ulrich Van Den Hekke                         *
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

#pragma once
#ifndef RECURSIVESORTFILTERPROXYMODEL_H
#define RECURSIVESORTFILTERPROXYMODEL_H

// Xinx header
#include <core/lib-config.h>

// Qt header
#include <QSortFilterProxyModel>
#include <QStringList>
#include <QHash>

class LIBEXPORT RecursiveSortFilterProxyModel : public QSortFilterProxyModel
{
public:
	RecursiveSortFilterProxyModel(QObject * parent = 0);

	bool showAllChild() const;
	void setShowAllChild(bool value);
	void setFilterRegExp(const QString & regExp);
	void setHidePath(const QStringList & hidePath);
	void setIncludeIndex(const QModelIndexList & indexes);

	void setDisabledVisible(bool value);
	bool disabledVisible() const;
protected:
	virtual bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;
	bool canBeShow(const QModelIndex & index) const;
	bool mustBeShow(const QModelIndex & index) const;   // true if a parent is equals
private:
	bool m_showAllChild, m_disabledVisible;
	QStringList m_hidePath;
	QList<QPersistentModelIndex> m_indexes;

	mutable QHash<QPersistentModelIndex,bool> m_indexCache;
};

#endif // RECURSIVESORTFILTERPROXYMODEL_H
