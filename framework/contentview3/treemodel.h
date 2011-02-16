/*
 XINX
 Copyright (C) 2007-2011 by Ulrich Van Den Hekke
 xinx@shadoware.org

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful.
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CONTENTVIEW3_TREEMODEL_H
#define CONTENTVIEW3_TREEMODEL_H

// Xinx header
#include "file.h"
#include "core/lib-config.h"

// Qt header
#include <QStandardItemModel>
#include <QScopedPointer>

namespace ContentView3
{

class PrivateTreeModel;

class LIBEXPORT TreeModel : public QStandardItemModel
{
	Q_OBJECT
public:
	explicit TreeModel(FilePtr file, QObject* parent = 0);
	explicit TreeModel(QObject* parent = 0);
	virtual ~TreeModel();

	void setFile(FilePtr file);
	FilePtr file() const;

	void reload();

	virtual bool hasChildren(const QModelIndex & parent = QModelIndex()) const;
	virtual bool canFetchMore(const QModelIndex & parent) const;
	virtual void fetchMore(const QModelIndex & parent);

	QString filenameOfIndex(const QModelIndex & index) const;
	int lineOfIndex(const QModelIndex & index) const;
private:
	QScopedPointer<PrivateTreeModel> d;
};

}

#endif // CONTENTVIEW3_TREEMODEL_H
