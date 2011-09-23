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

#ifndef CODECOMPLETION_ITEMMODELFACTORY_H
#define CODECOMPLETION_ITEMMODELFACTORY_H

#include <codecompletion/context.h>
#include <codecompletion/iteminterface.h>

namespace CodeCompletion
{

class PrivateItemModelFactory;
class ItemInterface;

/*
 * FIXME: Documentation
 * FIXME: Créer une factory de Snipet
 */
class LIBEXPORT ItemModelFactory
{
public:
	ItemModelFactory();
	virtual ~ItemModelFactory();

	void setContext(Context context);
	Context context() const;

	void setItemInterface(ItemInterface * interface);
	ItemInterface * itemInterface() const;

	virtual void generate() = 0;
protected:
	bool contextChanged();
	void modelUpdated();
private:
	PrivateItemModelFactory * d;
};

}

#endif // CODECOMPLETION_ITEMMODELFACTORY_H
