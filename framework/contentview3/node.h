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
#ifndef CONTENTVIEW3_NODE_H
#define CONTENTVIEW3_NODE_H

// Xinx header
#include <core/lib-config.h>
#include <contentview3/definitions.h>

// Qt header
#include <QObject>
#include <QScopedPointer>

namespace ContentView3
{

class PrivateNode;

class LIBEXPORT Node
{
public:
	static NodePtr create(const QString & name, NodePtr parent = NodePtr());
	static NodePtr create(NodePtr parent = NodePtr());
	virtual ~Node();

	virtual QString type() const;
	virtual void setType(const QString & type);

	virtual QString name() const;
	virtual void setName(const QString & name);

	virtual QString tips() const;
	virtual void setTips(const QString & tips);

	virtual QString icon() const;
	virtual void setIcon(const QString & icon);

	const QString & filename() const;
	FilePtrWeak file() const;
	void setFile(FilePtrWeak file);

	virtual int line() const;
	virtual void setLine(int line);

	NodePtrWeak parent() const;
	void setParent(NodePtrWeak parent);

	const QList<NodePtr> & childs() const;
	void addChild(NodePtr child);
	virtual void clearChilds();

	virtual QString displayName() const;
	virtual QString completionString() const;

	QString keyString() const;
protected:
	explicit Node(const QString & name = QString());

	static void initalisation(NodePtr node, NodePtr parent);

	NodePtrWeak thisPtr() const;
	void setThisPtr(NodePtrWeak value);

private:
	QScopedPointer<PrivateNode> d;
};

}

#endif // CONTENTVIEW3_NODE_H
