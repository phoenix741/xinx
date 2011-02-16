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


#ifndef CORE_DEFINITION_ROOTNODE_H
#define CORE_DEFINITION_ROOTNODE_H

// Xinx header
#include <contentview3/filenode.h>
#include <contentview3/definitions.h>

// Qt header
#include <QString>
#include <QHash>

namespace Core
{

namespace BaliseDefinition
{

class BaliseNode;
class AttributeNode;

class RootNode : public ContentView3::FileNode
{
public:
	static QSharedPointer<RootNode> create(const QString & name, ContentView3::NodePtr parent = ContentView3::NodePtr());
	virtual ~RootNode();

	QList< QSharedPointer<BaliseNode> > balises() const;
	void addBalise(QSharedPointer<BaliseNode> node);

	QSharedPointer<BaliseNode> balise(const QString & name);

	virtual void clearChilds();

	virtual ContentView3::FileNodePtr clone() const;
protected:
	RootNode(const QString& name);
private:
	QHash< QString,QSharedPointer<BaliseNode> > _balises;

	friend class BaliseNode;
};

}

}

#endif // CORE_DEFINITION_ROOTNODE_H
