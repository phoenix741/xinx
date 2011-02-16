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

#include "node.h"
#include "file.h"
#include <typeinfo>
#include <QStringBuilder>
#include <QDebug>

namespace ContentView3
{

/* PrivateNode */

class PrivateNode
{
public:
	PrivateNode();

	NodePtrWeak _parent, _this;
	QString _name, _tips, _icon, _type;
	int _line;
	QList<NodePtr> _childs;
	FilePtrWeak _file;
};

PrivateNode::PrivateNode() : _line(0)
{

}

/* Node */

NodePtr Node::create(const QString & name, NodePtr parent)
{
	NodePtr ptr(new Node(name));
	initalisation(ptr, parent);
	return ptr;
}

NodePtr Node::create(NodePtr parent)
{
	return Node::create(QString(), parent);
}

void Node::initalisation(NodePtr node, NodePtr parent)
{
	node->setThisPtr(node.toWeakRef());
	node->setParent(parent.toWeakRef());
	if (parent)
	{
		parent->addChild(node);
	}
}

Node::Node(const QString& name) : d(new PrivateNode)
{
	setName(name);
	setType(typeid(this).name());

	//qDebug() << "Create the node" << this << "with name" << name;
}

Node::~Node()
{
	clearChilds();

	//qDebug() << "Destroy the node" << this << "with name" << d->_name;
}

void Node::setThisPtr(NodePtrWeak value)
{
	d->_this = value;
}

NodePtrWeak Node::thisPtr() const
{
	return d->_this;
}

void Node::addChild(NodePtr child)
{
	d->_childs.append(child);
	child->setFile(d->_file);
	child->setParent(d->_this);
}

const QList< NodePtr >& Node::childs() const
{
	return d->_childs;
}

void Node::clearChilds()
{
	foreach(NodePtr node, d->_childs)
	{
		node->setParent(NodePtrWeak());
	}

	d->_childs.clear();
}

NodePtrWeak Node::parent() const
{
	return d->_parent;
}

void Node::setParent(NodePtrWeak parent)
{
	d->_parent = parent;
}

const QString& Node::filename() const
{
	FilePtr file = d->_file.toStrongRef();
	return file->filename();
}

FilePtrWeak Node::file() const
{
	return d->_file;
}

void Node::setFile(FilePtrWeak file)
{
	d->_file = file;

	foreach(NodePtr node, d->_childs)
	{
		node->setFile(file);
	}
}

QString Node::name() const
{
	return d->_name;
}

void Node::setName(const QString& name)
{
	d->_name= name;
}

QString Node::type() const
{
	return d->_type;
}

void Node::setType(const QString& type)
{
	d->_type = type;
}

int Node::line() const
{
	return d->_line;
}

void Node::setLine(int line)
{
	d->_line = line;
}

QString Node::icon() const
{
	return d->_icon;
}

void Node::setIcon(const QString& icon)
{
	d->_icon = icon;
}

QString Node::tips() const
{
	return d->_tips;
}

void Node::setTips(const QString& tips)
{
	d->_tips = tips;
}

QString Node::displayName() const
{
	return d->_name;
}

QString Node::completionString() const
{
	return "%1";
}

QString Node::keyString() const
{
	const QString sep  = QString::fromUtf8("¤");
	const QString line = QString::number(d->_line);

	return d->_name % sep % d->_type % sep % d->_tips % sep % d->_icon % sep % line;
}

}

