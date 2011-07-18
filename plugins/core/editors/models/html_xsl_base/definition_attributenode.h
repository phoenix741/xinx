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


#ifndef CORE_DEFINITION_ATTRIBUTENODE_H
#define CORE_DEFINITION_ATTRIBUTENODE_H

// Xinx header
#include <contentview3/node.h>
#include "definition_balisenode.h"

// Qt header
#include <QString>
#include <QHash>
#include <QApplication>

namespace Core
{

namespace BaliseDefinition
{

class ValueNode;

class AttributeNode : public ContentView3::Node
{
	Q_DECLARE_TR_FUNCTIONS(AttributeNode)
public:
	static QSharedPointer<AttributeNode> create(const QString & name, QSharedPointer<BaliseNode> parent = QSharedPointer<BaliseNode>());
	virtual ~AttributeNode();

	bool isDefault() const;
	void setDefault(bool value);

	void setOrd(int value);
	int ord() const;

	QList< QSharedPointer<ValueNode> > defaultValues() const;
	QList< QSharedPointer<ValueNode> > values() const;
	void addValue(QSharedPointer<ValueNode> value);
	virtual void clearChilds();

	QSharedPointer<ValueNode> value(const QString & value);
protected:
	AttributeNode(const QString& name);
private:
	int  _ord;
	bool _is_default;
	QHash< QString, QSharedPointer<ValueNode> > _values;
};

}

}

#endif // CORE_DEFINITION_ATTRIBUTENODE_H
