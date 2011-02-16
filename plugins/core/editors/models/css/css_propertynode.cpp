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
#include "css_propertynode.h"

namespace Core
{

namespace CascadingStyleSheet
{

QSharedPointer<PropertyNode> PropertyNode::create(const QString & name, ContentView3::NodePtr parent)
{
	QSharedPointer<PropertyNode> ptr(new PropertyNode(name));
	initalisation(ptr, parent);
	return ptr;
}

PropertyNode::PropertyNode(const QString& name): ContentView3::Node(name.trimmed())
{
	setIcon(":/images/html_value.png");
}

PropertyNode::~PropertyNode()
{

}

void PropertyNode::setValue(const QString& value)
{
	_value = value;
	setTips(tr("Element at line : %1\nValue = %2").arg(line()).arg(_value));
}

const QString& PropertyNode::value() const
{
	return _value;
}

void PropertyNode::setLine(int line)
{
	ContentView3::Node::setLine(line);
	setTips(tr("Element at line : %1\nValue = %2").arg(line).arg(_value));
}

}

}
