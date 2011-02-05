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
#include "definition_rootnode.h"
#include "definition_balisenode.h"
#include <QDebug>

namespace Core
{

namespace BaliseDefinition
{

QSharedPointer<RootNode> RootNode::create(const QString & name, ContentView3::NodePtr parent)
{
	QSharedPointer<RootNode> ptr(new RootNode(name));
	initalisation(ptr, parent);
	return ptr;
}

RootNode::RootNode(const QString& name): ContentView3::FileNode(name.trimmed())
{
	setIcon(":/images/template.png");
}

RootNode::~RootNode()
{

}

QList< QSharedPointer<BaliseNode> > RootNode::balises() const
{
	return _balises.values();
}

void RootNode::addBalise(QSharedPointer<BaliseNode> node)
{
	_balises.insert(node->name(), node);
	addChild(node);
}

QSharedPointer<BaliseNode> RootNode::balise(const QString & name)
{
	return _balises.value(name);
}

void RootNode::clearChilds()
{
	_balises.clear();
	ContentView3::Node::clearChilds();
}

ContentView3::FileNodePtr RootNode::clone() const
{
	QSharedPointer<RootNode> node = RootNode::create (name(), parent ());
	node->setFile (file());
	node->setIcon (icon ());
	node->setLine (line());
	node->setTips (tips());
	node->setType (type());
	return node;
}

}

}
