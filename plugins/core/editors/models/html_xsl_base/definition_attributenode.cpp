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
#include "definition_attributenode.h"
#include "definition_balisenode.h"
#include "definition_valuenode.h"

namespace Core
{

namespace BaliseDefinition
{

QSharedPointer<AttributeNode> AttributeNode::create(const QString & name, QSharedPointer<BaliseNode> parent)
{
	QSharedPointer<AttributeNode> ptr(new AttributeNode(name));
	initalisation(ptr, parent);
	if (parent)
	{
		parent->addAttribute(ptr);
	}
	return ptr;
}

AttributeNode::AttributeNode(const QString& name): ContentView3::Node(name.trimmed())
{
	setIcon(":/images/noeud.png");
	setType(tr("Attribute"));
}

AttributeNode::~AttributeNode()
{

}

bool AttributeNode::isDefault() const
{
	return _is_default;
}

void AttributeNode::setDefault(bool value)
{
	_is_default = value;
}

QList< QSharedPointer<ValueNode> > AttributeNode::defaultValues() const
{
	QList< QSharedPointer<ValueNode> > result;

	foreach(QSharedPointer<ValueNode> node, _values.values())
	{
		if (node->isDefault())
		{
			result << node;
		}
	}

	return result;
}

QList< QSharedPointer<ValueNode>  > AttributeNode::values() const
{
	return _values.values();
}

void AttributeNode::addValue(QSharedPointer<ValueNode>  value)
{
	_values.insert(value->name(), value);
	addChild(value);
}

void AttributeNode::clearChilds()
{
	_values.clear();
	ContentView3::Node::clearChilds();
}

QSharedPointer<ValueNode>  AttributeNode::value(const QString & value)
{
	return _values.value(value);
}

}

}
