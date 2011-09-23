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

#include "actionitem.h"

namespace CodeCompletion
{

/* ActionItem */

/*!
 * \class ActionItem
 * \ingroup codecompletion
 * \since 0.10.1
 *
 * \brief Item of a code completion list used for action.
 *
 * Item in a code completion list, that call an action. This can be the case of
 * calling snipet, or other type of action (like adding a new template at the end
 * of the stylesheet ...)
 */

/*!
 * \brief Create a new action item
 *
 * \param libelle The libelle of the action (show in the completion list)
 * \param code The code to write in the editor to show this action.
 */
ActionItem::ActionItem(const QString& libelle, const QString & code)
{
	setCompletionText(code);
	setText(libelle);
	setForeground(Qt::gray);
	setKeyString(code);
	setCompletionType(tr("Action"));
}

//! Destroy the action item
ActionItem::~ActionItem()
{

}

}