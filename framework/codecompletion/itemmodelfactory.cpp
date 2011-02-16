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

#include "itemmodelfactory.h"

namespace CodeCompletion
{

/* PrivateItemModelFactory */

class PrivateItemModelFactory
{
public:
	PrivateItemModelFactory();

	bool _contextChanged;
	Context _context;
	ItemInterface* _interface;
};

PrivateItemModelFactory::PrivateItemModelFactory() : _contextChanged(false), _interface(0)
{

}

/* ItemModelFactory */

/*!
 * \class ItemModelFactory
 * \brief Create items that can be used in the CodeCompletion::Model
 */

/*!
 * \fn virtual void ItemModelFactory::generate() = 0
 * \brief This method create the list of item to show in the completion
 *
 * This method must check contextChanged() before launch the update and must
 * stop by calling modelUpdated().
 */

ItemModelFactory::ItemModelFactory()
{
	d = new PrivateItemModelFactory;
}

ItemModelFactory::~ItemModelFactory()
{
	delete d;
}

ItemInterface * ItemModelFactory::itemInterface() const
{
	return d->_interface;
}

void ItemModelFactory::setItemInterface(ItemInterface* interface)
{
	d->_interface = interface;
}

void ItemModelFactory::setContext(Context context)
{
	if (d->_context != context)
	{
		d->_contextChanged = true;
		d->_context = context;
	}
}

Context ItemModelFactory::context() const
{
	return d->_context;
}

bool ItemModelFactory::contextChanged()
{
	return d->_contextChanged;
}

void ItemModelFactory::modelUpdated()
{
	d->_contextChanged = false;
}

}

