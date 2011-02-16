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

#pragma once
#ifndef _CODECOMPLETION_INTERFACES_H_
#define _CODECOMPLETION_INTERFACES_H_

// Qt header
#include <QtPlugin>

namespace CodeCompletion
{
class ContextParser;
class ItemModelFactory;
}

class AbstractEditor;

/*!
 * Interface link to the CodeCompletion part.
 */
class ICodeCompletionPlugin
{
public:
	/*!
	 * \brief Create and return a list of ContextParser.
	 */
	virtual QList<CodeCompletion::ContextParser*> createContextParser() const = 0;
	/*!
	 * \brief Create and return a list of ItemModelFactory
	 */
	virtual QList<CodeCompletion::ItemModelFactory*> createItemModelFactory() const = 0;
};

Q_DECLARE_INTERFACE(ICodeCompletionPlugin, "org.shadoware.xinx.ICodeCompletionPlugin/1.0")

#endif /*_CODECOMPLETION_INTERFACES_H_*/
