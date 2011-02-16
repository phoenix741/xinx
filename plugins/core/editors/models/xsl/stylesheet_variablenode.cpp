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
#include "stylesheet_variablenode.h"

// Qt header
#include <QFileInfo>
#include <plugins/xinxpluginsloader.h>

namespace Core
{

namespace Stylesheet
{

QSharedPointer<VariableNode> VariableNode::create(const QString & name, ContentView3::NodePtr parent)
{
	QSharedPointer<VariableNode> ptr(new VariableNode(name));
	initalisation(ptr, parent);
	return ptr;
}

VariableNode::VariableNode(const QString& name): Node(name)
{
	setIcon(":/images/variable.png");
	setType(tr("Xsl Variable"));
}

VariableNode::~VariableNode()
{

}

void VariableNode::setLine(int value)
{
	ContentView3::Node::setLine(value);
	setTips(tr("Element at line : %1\nValue = %2").arg(line()).arg(_value));
}

void VariableNode::setValue(const QString& value)
{
	_value = value;
	setTips(tr("Element at line : %1\nValue = %2").arg(line()).arg(_value));
}

const QString& VariableNode::value() const
{
	return _value;
}

QString VariableNode::displayName() const
{
	return name() + (! value().isEmpty() ? " = " + value() : "");
}

QString VariableNode::completionString() const
{
	return "$%1";
}

}

}
