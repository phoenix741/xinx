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


#ifndef CORE_DEFINITION_VALUENODE_H
#define CORE_DEFINITION_VALUENODE_H

// Xinx header
#include <contentview3/node.h>

// Qt header
#include <QString>
#include <QApplication>

namespace Core
{

namespace BaliseDefinition
{

class AttributeNode;

class ValueNode : public ContentView3::Node
{
	Q_DECLARE_TR_FUNCTIONS(ValueNode)
public:
	static QSharedPointer<ValueNode> create(const QString & name, QSharedPointer<AttributeNode> parent = QSharedPointer<AttributeNode>());
	virtual ~ValueNode();

	bool isDefault() const;
	void setDefault(bool value);
protected:
	ValueNode(const QString& name);
private:
	bool _is_default;
};

}

}

#endif // CORE_DEFINITION_VALUENODE_H
