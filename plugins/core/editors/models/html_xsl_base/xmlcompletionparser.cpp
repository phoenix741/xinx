/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

// Xinx header
#include "xmlcompletionparser.h"
#include "definition_rootnode.h"
#include "definition_balisenode.h"
#include "definition_attributenode.h"
#include "definition_valuenode.h"

namespace Core
{

namespace BaliseDefinition
{

/* XmlCompletionParser */

XmlCompletionParser::XmlCompletionParser() : ContentView3::Parser()
{

}

XmlCompletionParser::~XmlCompletionParser()
{

}

QString XmlCompletionParser::name() const
{
	return "XSL/HTML";
}

void XmlCompletionParser::parse()
{
	Q_ASSERT_X(ContentView3::Parser::device(), "XmlCompletionParser::parser", "No device to read.");

	ContentView3::Parser::device()->reset();
	QXmlStreamReader::setDevice(ContentView3::Parser::device());

	QSharedPointer<RootNode> node = rootNode().dynamicCast<RootNode>();
	Q_ASSERT_X(node, "XmlCompletionParser::load", "FileNode must be of the type RootNode");

	node->clearChilds();
	node->setIcon(":/images/typexsl.png");

	while (! atEnd())
	{
		readNext();

		if (isStartElement())
		{
			if (QXmlStreamReader::name() == "definition")
				readRootTag(node);
			else
				raiseError(tr("The file is can't be used to do completion."));
		}
	}

	if (error())    // Else completion can be more difficulte
		throw ContentView3::ParserException(errorString(), lineNumber(), columnNumber());
}

void XmlCompletionParser::readRootTag(const QSharedPointer<RootNode> & rootNode)
{
	Q_ASSERT(isStartElement() && QXmlStreamReader::name() == "definition");

	while (!atEnd())
	{
		readNext();

		if (isEndElement())
			break;

		if (isStartElement())
		{
			if (QXmlStreamReader::name() == "balise")
				readBaliseTag(rootNode);
			else
				raiseError(tr("Wrong node name \"%1\" in type element").arg(QXmlStreamReader::name().toString()));
		}
	}
}

void XmlCompletionParser::readBaliseTag(const ContentView3::NodePtr & parent)
{
	Q_ASSERT(isStartElement() && QXmlStreamReader::name() == "balise");

	QString		name 			= attributes().value("name").toString(),
					   defaultValue 	= attributes().value("default").toString();

	QSharedPointer<BaliseNode> node = BaliseNode::create(name, parent);
	node->setLine(decalage() + lineNumber());
	node->setDefault(defaultValue == "true" ? true : false);

	while (!atEnd())
	{
		readNext();

		if (isEndElement())
			break;

		if (isStartElement())
		{
			if (QXmlStreamReader::name() == "balise")
				readBaliseTag(node);
			else if (QXmlStreamReader::name() == "attribute")
				readAttributeTag(node);
			else
				raiseError(tr("Wrong node name \"%1\" in balise element").arg(QXmlStreamReader::name().toString()));
		}
	}
}

void XmlCompletionParser::readAttributeTag(const QSharedPointer<BaliseNode> & parent)
{
	Q_ASSERT(isStartElement() && QXmlStreamReader::name() == "attribute");

	QString		name 			= attributes().value("name").toString(),
					   defaultValue 	= attributes().value("default").toString();

	QSharedPointer<AttributeNode> node = AttributeNode::create(name, parent);
	node->setLine(decalage() + lineNumber());
	node->setDefault(defaultValue == "true" ? true : false);

	while (!atEnd())
	{
		readNext();

		if (isEndElement())
			break;

		if (isStartElement())
		{
			if (QXmlStreamReader::name() == "value")
				readValueTag(node);
			else
				raiseError(tr("Wrong node name \"%1\" in attribute element").arg(QXmlStreamReader::name().toString()));
		}
	}
}

void XmlCompletionParser::readValueTag(const QSharedPointer<AttributeNode> & parent)
{
	Q_ASSERT(isStartElement() && QXmlStreamReader::name() == "value");

	const QXmlStreamAttributes attrs  = attributes();
	const QString 	name 			= readElementText(IncludeChildElements),
							 defaultValue 	= attrs.value("default").toString();

	QSharedPointer<ValueNode> node = ValueNode::create(name, parent);
	node->setLine(decalage() + lineNumber());
	node->setDefault(defaultValue == "true" ? true : false);
}

}

}
