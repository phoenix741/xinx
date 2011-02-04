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


#ifndef CORE_JS_VARIABLENODE_H
#define CORE_JS_VARIABLENODE_H

// Xinx header
#include <contentview3/node.h>

// Qt header
#include <QString>

namespace Core {

namespace JavaScript {

class VariableNode : public ContentView3::Node
{
public:
	static QSharedPointer<VariableNode> create(const QString & name, ContentView3::NodePtr parent = ContentView3::NodePtr());

	virtual ~VariableNode();

protected:
	VariableNode(const QString& name);
private:
};

}

}

#endif // CORE_JS_VARIABLENODE_H
