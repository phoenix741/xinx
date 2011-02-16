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

#include "dictionary_labelnode.h"

namespace Generix
{

namespace Dictionary
{

/* LabelNode */

QSharedPointer<LabelNode> LabelNode::create(const QString & name, ContentView3::NodePtr parent)
{
	QSharedPointer<LabelNode> ptr(new LabelNode(name));
	initalisation(ptr, parent);
	return ptr;
}

LabelNode::LabelNode(const QString& name): Node(name)
{
	setIcon(":/generix/images/label.png");
	setType("Dictionary Text");
}

LabelNode::~LabelNode()
{

}

QString LabelNode::code() const
{
	ContentView3::NodePtr parentPtr = parent().toStrongRef();
	if (parentPtr)
	{
		return parentPtr->name();
	}
	else
	{
		return QString();
	}
}

const QString& LabelNode::lang() const
{
	return _lang;
}

void LabelNode::setLang(const QString& lang)
{
	_lang = lang;
}

const QString& LabelNode::context() const
{
	return _context;
}

void LabelNode::setContext(const QString& context)
{
	_context = context;
}

const QString& LabelNode::value() const
{
	return _value;
}

void LabelNode::setValue(const QString& value)
{
	_value = value;
}

QString LabelNode::displayName() const
{
	return QString("%1 [Langue=%2] [Ctx=%3]").arg(_value).arg(_lang).arg(_context);
}

}

}
