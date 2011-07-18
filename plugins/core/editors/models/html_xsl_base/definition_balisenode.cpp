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
#include "definition_balisenode.h"
#include "definition_attributenode.h"
#include "definition_rootnode.h"
#include <QDebug>

namespace Core
{

namespace BaliseDefinition
{

template<class T> static bool nodeOrdLessThan(const QSharedPointer<T> & v1, const QSharedPointer<T> & v2)
{
	return v1->ord() < v2->ord();
}

QSharedPointer<BaliseNode> BaliseNode::create(const QString & name, ContentView3::NodePtr parent)
{
	QSharedPointer<BaliseNode> ptr(new BaliseNode(name));
	initalisation(ptr, parent);

	if (parent.dynamicCast<RootNode>())
	{
		ptr->_rootNode = parent.dynamicCast<RootNode>().toWeakRef();

		parent.dynamicCast<RootNode>()->addBalise(ptr);
	}
	else if (parent.dynamicCast<BaliseNode>())
	{
		parent.dynamicCast<BaliseNode>()->addBalise(ptr);
	}
	return ptr;
}

BaliseNode::BaliseNode(const QString& name): ContentView3::Node(name.trimmed()), _ord(0)
{
	setIcon(":/images/template.png");
	setType(tr("Balise"));
}

BaliseNode::~BaliseNode()
{

}

int BaliseNode::ord() const
{
	return _ord;
}

void BaliseNode::setOrd(int value)
{
	_ord = value;
}

bool BaliseNode::isDefault() const
{
	return _is_default;
}

void BaliseNode::setDefault(bool value)
{
	_is_default = value;
}

QList< QSharedPointer<BaliseNode> > BaliseNode::defaultBalises() const
{
	QList< QSharedPointer<BaliseNode> > result;

	foreach(QSharedPointer<BaliseNode> node, _balises.values())
	{
		if (node->isDefault())
		{
			result << node;
		}
	}

	qSort(result.begin(), result.end(), nodeOrdLessThan<BaliseNode>);

	return result;
}

QList< QSharedPointer<BaliseNode> > BaliseNode::balises() const
{
	return _balises.values();
}

void BaliseNode::addBalise(QSharedPointer<BaliseNode> node)
{
	node->setOrd(_balises.size());
	_balises.insert(node->name(), node);
	addChild(node);

	QSharedPointer<RootNode> rootNode = _rootNode.toStrongRef();
	if (rootNode)
	{
		rootNode->_balises.insert(node->name(), node);
	}
}

QList< QSharedPointer<AttributeNode> > BaliseNode::defaultAttributes() const
{
	QList< QSharedPointer<AttributeNode> > result;

	foreach(QSharedPointer<AttributeNode> node, _attributes.values())
	{
		if (node->isDefault())
		{
			result << node;
		}
	}

	qSort(result.begin(), result.end(), nodeOrdLessThan<AttributeNode>);

	return result;
}

QList< QSharedPointer<AttributeNode> > BaliseNode::attributes() const
{
	return _attributes.values();
}

void BaliseNode::addAttribute(QSharedPointer<AttributeNode> node)
{
	node->setOrd(_attributes.size());
	_attributes.insert(node->name(), node);
	addChild(node);
}

QSharedPointer<AttributeNode> BaliseNode::attribute(const QString & name)
{
	return _attributes.value(name);
}

void BaliseNode::clearChilds()
{
	_attributes.clear();
	_balises.clear();
	ContentView3::Node::clearChilds();
}

}

}
