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

#include "filenode.h"

// Qt header
#include <QFileInfo>

namespace ContentView3
{

FileNodePtr FileNode::create(const QString & name, NodePtr parent)
{
	FileNodePtr ptr(new FileNode(name));
	ptr->setThisPtr(ptr.toWeakRef());
	ptr->setParent(parent.toWeakRef());
	return ptr;
}

FileNodePtr FileNode::create(NodePtr parent)
{
	return create(QString(), parent);
}

FileNode::FileNode(const QString& filename): Node(filename)
{
}

QString FileNode::displayName() const
{
    return QFileInfo(name()).fileName();
}

FileNodePtr FileNode::clone() const
{
	FileNodePtr node = FileNode::create (name(), parent ());
	node->setFile (file());
	node->setIcon (icon ());
	node->setLine (line());
	node->setTips (tips());
	node->setType (type());
	return node;
}

}
