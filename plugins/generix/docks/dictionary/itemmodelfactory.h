/*
 XINX
 Copyright (C) 2007-2010 by Ulrich Van Den Hekke
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

#ifndef GENERIX_DICTIONARY_ITEMMODELFACTORY_H
#define GENERIX_DICTIONARY_ITEMMODELFACTORY_H

#include <codecompletion/itemmodelfactory.h>
#include <QApplication>
#include <contentview3/definitions.h>
#include <codecompletion/model.h>

namespace Core
{
namespace BaliseDefinition
{
class XmlContextType;
}
};

namespace Generix
{

namespace Dictionary
{

class ItemModelFactory : public CodeCompletion::ItemModelFactory
{
	Q_DECLARE_TR_FUNCTIONS(ItemModelFactory)
public:
	ItemModelFactory();
	virtual ~ItemModelFactory();

	virtual void generate();
private:
	void addNode(const QString & contextType, ContentView3::NodePtr node);
};

}

}

#endif // GENERIX_DICTIONARY_ITEMMODELFACTORY_H
