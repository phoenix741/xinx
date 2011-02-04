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
#include "css_classnode.h"

namespace Core
{

namespace CascadingStyleSheet
{

QSharedPointer<ClassNode> ClassNode::create(const QString & name, ContentView3::NodePtr parent)
{
	QSharedPointer<ClassNode> ptr(new ClassNode(name));
	initalisation(ptr, parent);
	return ptr;
}

ClassNode::ClassNode(const QString& name): ContentView3::Node(name.trimmed())
{
	setIcon(":/images/noeud.png");
}

ClassNode::~ClassNode()
{

}

}

}
