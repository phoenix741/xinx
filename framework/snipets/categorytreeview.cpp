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

// Xinx header
#include "snipets/categorytreeview.h"
#include "snipets/categoryitemmodel.h"

/* CategoryTreeView */

/*!
 * \ingroup Snipets
 * \class CategoryTreeView
 * \since 0.9.0.0
 *
 * \brief This tree view is used to choose a new category in the model.
 *
 * This dialog box add to method to the QTreeView object : categoryId() and
 * setCategoryId() to get te selected category and select a new category with
 * the help of the id.
 */

//! Create the tree view
CategoryTreeView::CategoryTreeView(QWidget * parent) : QTreeView(parent)
{
}

//! Destroy the tree view
CategoryTreeView::~CategoryTreeView()
{
}

//! Get the selected category id in the tree view
int CategoryTreeView::categoryId()
{
	QModelIndexList indexes = selectedIndexes();
	if (indexes.count() == 0) return -1;

	return indexes.at(0).data(CategoryItemModel::CategoryIdRole).toInt();
}

//! Set the selected category id to \p id
void CategoryTreeView::setCategoryId(int id)
{
	CategoryItemModel * categoryModel = qobject_cast<CategoryItemModel*>(model());
	Q_ASSERT(categoryModel);
	selectionModel()->select(QModelIndex(), QItemSelectionModel::Clear);
	selectionModel()->select(categoryModel->index(id), QItemSelectionModel::Select);
}

