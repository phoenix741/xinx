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
#include "completerdirmodel.h"

// Qt header
#include <QDir>

/* CompleterDirModel */

/*!
 * \ingroup Components
 * \class CompleterDirModel
 * \since 0.7.0.0
 *
 * \brief Class used by DirectoryEdit to complete on path.
 *
 * \bc
 *
 * Completer model class based on a QDirModel which permit to propose a completion on
 * the path.
 * The CompleterDirModel convert path on native separtor. If the path end with a separator,
 * the separator is deleted.
 *
 * To use this modele, we can use a QCompleter on a QLineEdit.
 *
 * \code
 *   QLineEdit * lineEdit = new QLineEdit;
 *
 *   QCompleter * completer = new QCompleter(lineEdit);
 *   lineEdit->setCompleter(completer);
 *
 *   CompleterDirModel * model = new CompleterDirModel(completer);
 *   completer->setModel(model);
 * \endcode
 */

/*!
 * \brief Construct a CompleterDirModel.
 * \param parent The parent of the object.
 */
CompleterDirModel::CompleterDirModel(QObject *parent) : QDirModel(parent)
{
}

/*!
 * \brief Return the data to change in the completion dir model.
 *
 * This model show the good separator (with the \p QDir::toNativeSepartors() function).
 * \param index The model index, represents a path in the tree.
 * \param role Only the role Qt::DisplayRole is modified in this object.
 */
QVariant CompleterDirModel::data(const QModelIndex &index, int role) const
{
	if (role == Qt::DisplayRole && index.column() == 0)
	{
		QString path = QDir::toNativeSeparators(filePath(index));
		if (path.endsWith(QDir::separator()))
			path.chop(1);
		return path;
	}

	return QDirModel::data(index, role);
}
