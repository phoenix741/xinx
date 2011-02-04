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
#ifndef CONTENTVIEW3_FILENODE_H
#define CONTENTVIEW3_FILENODE_H

// Xinx header
#include <core/lib-config.h>
#include <contentview3/node.h>

namespace ContentView3 {

class LIBEXPORT FileNode : public Node
{
public:
	static FileNodePtr create(const QString & name, NodePtr parent = NodePtr());
	static FileNodePtr create(NodePtr parent = NodePtr());

	virtual QString displayName() const;

	virtual FileNodePtr clone() const;
protected:
	explicit FileNode(const QString & filename);
};

}

#endif // CONTENTVIEW3_FILENODE_H
