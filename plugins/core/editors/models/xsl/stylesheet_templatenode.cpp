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
#include "stylesheet_templatenode.h"

namespace Core
{

namespace Stylesheet
{

QSharedPointer<TemplateNode> TemplateNode::create(const QString & name, ContentView3::NodePtr parent)
{
	QSharedPointer<TemplateNode> ptr(new TemplateNode(name));
	initalisation(ptr, parent);
	return ptr;
}

TemplateNode::TemplateNode(const QString& name): ContentView3::Node(name.trimmed()), _isNamedTemplate(false)
{
	setIcon(":/images/template.png");
	setType(tr("Xsl Template"));
}

TemplateNode::~TemplateNode()
{

}

void TemplateNode::setLine(int value)
{
    ContentView3::Node::setLine(value);
	setTips(tr("Element at line : %1\nMode = %2").arg(line()).arg(_mode));
}

void TemplateNode::setMode(const QString & mode)
{
	_mode = mode;
	setTips(tr("Element at line : %1\nMode = %2").arg(line()).arg(_mode));
}

const QString & TemplateNode::mode() const
{
	return _mode;
}

void TemplateNode::setIsNamedTemplate(bool value)
{
	_isNamedTemplate = value;
	if (_isNamedTemplate)
	{
		setType(tr("Xsl named Template"));
	}
	else
	{
		setType(tr("Xsl Template"));
	}
}

bool TemplateNode::isNamedTemplate() const
{
	return _isNamedTemplate;
}

QString TemplateNode::displayName() const
{
	return name() + (!_mode.isEmpty() ? QString(" [%1]").arg(_mode) : "");
}

}

}
