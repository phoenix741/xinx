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


#ifndef CORE_DEFINITION_BALISENODE_H
#define CORE_DEFINITION_BALISENODE_H

// Xinx header
#include <contentview3/node.h>

// Qt header
#include <QString>
#include <QHash>
#include <QApplication>

namespace Core {

namespace BaliseDefinition {

class RootNode;
class AttributeNode;

class BaliseNode : public ContentView3::Node
{
	Q_DECLARE_TR_FUNCTIONS(BaliseNode)
public:
	static QSharedPointer<BaliseNode> create(const QString & name, ContentView3::NodePtr parent = ContentView3::NodePtr());
	virtual ~BaliseNode();

	bool isDefault() const;
	void setDefault(bool value);

	QList< QSharedPointer<BaliseNode> > defaultBalises() const;
	QList< QSharedPointer<BaliseNode> > balises() const;
	void addBalise(QSharedPointer<BaliseNode> node);

	QList< QSharedPointer<AttributeNode> > defaultAttributes() const;
	QList< QSharedPointer<AttributeNode> > attributes() const;
	void addAttribute(QSharedPointer<AttributeNode> node);

	virtual void clearChilds();

	QSharedPointer<AttributeNode> attribute(const QString & name);
protected:
	BaliseNode(const QString& name);

private:
	QWeakPointer<RootNode> _rootNode;
	bool _is_default;
	QHash<QString,QSharedPointer<AttributeNode> > _attributes;
	QHash<QString,QSharedPointer<BaliseNode> > _balises;
};

}

}

#endif // CORE_STYLESHEET_TEMPLATENODE_H
