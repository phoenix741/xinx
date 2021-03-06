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

// Xinx header
#include "definition_valuenode.h"
#include "definition_attributenode.h"

namespace Core
{

namespace BaliseDefinition
{

QSharedPointer<ValueNode> ValueNode::create(const QString & name, QSharedPointer<AttributeNode> parent)
{
	QSharedPointer<ValueNode> ptr(new ValueNode(name));
	initalisation(ptr, parent);

	if (parent)
	{
		parent->addValue(ptr);
	}
	return ptr;
}

ValueNode::ValueNode(const QString& name): ContentView3::Node(name.trimmed())
{
	setIcon(":/images/html_value.png");
	setType(tr("Value"));
}

ValueNode::~ValueNode()
{

}

bool ValueNode::isDefault() const
{
	return _is_default;
}

void ValueNode::setDefault(bool value)
{
	_is_default = value;
}

}

}
