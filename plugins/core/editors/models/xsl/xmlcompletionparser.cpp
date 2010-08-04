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
#include <contentview2/contentview2node.h>
#include <plugins/plugininterfaces.h>
#include <plugins/xinxpluginsloader.h>
#include <core/xinxcore.h>
#include <contentview2/contentview2node.h>
#include <contentview2/contentview2cache.h>
#include <contentview2/contentview2manager.h>
#include <project/xinxproject.h>

// Qt header
#include <QIcon>
#include <QVariant>
#include <QQueue>
#include <QBuffer>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>

/* Static member */

XmlCompletionParser * XmlCompletionParser::s_self = 0;

QHash<QString,uint> XmlCompletionParser::m_balisesName;
QHash<uint,XmlCompletionParser::InternalBalise> XmlCompletionParser::m_balisesId;
QHash<uint,XmlCompletionParser::InternalAttribute> XmlCompletionParser::m_attributesId;

/* XmlCompletionParser */

XmlCompletionParser::XmlCompletionParser() : ContentView2::Parser()
{
	s_self = this;
}

XmlCompletionParser::~XmlCompletionParser()
{
	if (s_self == this)
		s_self  = 0;
}

XmlCompletionParser * XmlCompletionParser::self()
{
	if (! s_self)
	{
		s_self = new XmlCompletionParser();
	}
	return s_self;
}

XmlCompletionParser * XmlCompletionParser::clone()
{
	XmlCompletionParser * parser = new XmlCompletionParser;

	return parser;
}

void XmlCompletionParser::openFilename(const QString & filename)
{
	if(filename == "XmlCompletion")
	{
		ContentView2::Parser::openFilename("datas:baseplugin_xml.xml");
	}
	else
	{
		ContentView2::Parser::openFilename(filename);
	}
}

void XmlCompletionParser::load()
{
	m_index = 0;
	inputDevice()->reset();
	setDevice(inputDevice());

	clearNodes(rootNode());
	m_parentNode.clear();

	while (! atEnd())
	{
		readNext();

		if (isStartElement())
		{
			if (name() == "xml")
				readRootTag();
			else
				raiseError(tr("The file is can't be used to do completion."));
		}
	}

	if (error())    // Else completion can be more difficulte
		throw ContentView2::ParserException(errorString(), lineNumber(), columnNumber());
}

QString XmlCompletionParser::readElementText()
{
	Q_ASSERT(isStartElement());

	QString result;

	while (! atEnd())
	{
		QXmlStreamReader::TokenType type = readNext();

		if (isEndElement()) break;

		switch (type)
		{
		case Characters:
		case EntityReference:
			result += text().toString().trimmed();
			break;
		case StartElement:
			result += readElementText();
		default:
			continue;
		}
	}

	return result;
}

void XmlCompletionParser::readRootTag()
{
	Q_ASSERT(isStartElement() && QXmlStreamReader::name() == "xml");

	m_parentNode.push(rootNode());

	while (!atEnd())
	{
		readNext();

		if (isEndElement())
			break;

		if (isStartElement())
		{
			if (QXmlStreamReader::name() == "type")
				readTypeTag();
			else
				raiseError(tr("Wrong node name \"%1\" in root element").arg(QXmlStreamReader::name().toString()));
		}
	}

	m_parentNode.pop();
}

void XmlCompletionParser::readTypeTag()
{
	Q_ASSERT(isStartElement() && QXmlStreamReader::name() == "type");

	m_type = attributes().value("name").toString();

	while (!atEnd())
	{
		readNext();

		if (isEndElement())
			break;

		if (isStartElement())
		{
			if (QXmlStreamReader::name() == "balise")
				readBaliseTag();
			else
				raiseError(tr("Wrong node name \"%1\" in type element").arg(QXmlStreamReader::name().toString()));
		}
	}
}

void XmlCompletionParser::readBaliseTag()
{
	Q_ASSERT(isStartElement() && QXmlStreamReader::name() == "balise");

	QString
	name = attributes().value("name").toString(),
	       defaultValue = attributes().value("default").toString();

	ContentView2::Node node;
	node.setLine(lineNumber());
	node.setFileId(rootNode().fileId());
	node.setData("XmlBalise", ContentView2::Node::NODE_TYPE);
	node.setData(":/images/balise.png", ContentView2::Node::NODE_ICON);
	node.setData(name, ContentView2::Node::NODE_NAME);
	node.setData(name, ContentView2::Node::NODE_DISPLAY_NAME);
	node.setData(m_type, XmlCompletionParser::NODE_XML_TYPE);
	node.setData(defaultValue == "true" ? true : false, XmlCompletionParser::NODE_XML_ISDEFAULT);
	node.setData((uint)++m_index, XmlCompletionParser::NODE_XML_SORT_INDEX);

	ContentView2::Node parentNode = m_parentNode.top();
	attachNode(parentNode, node);

	m_balisesName.insert(name, node.nodeId());
	if (parentNode.nodeId() != rootNode().nodeId())
	{
		m_balisesId[ parentNode.nodeId()].balises.insert(name, node.nodeId());
	}

	m_parentNode.push(node);
	while (!atEnd())
	{
		readNext();

		if (isEndElement())
			break;

		if (isStartElement())
		{
			if (QXmlStreamReader::name() == "balise")
				readBaliseTag();
			else if (QXmlStreamReader::name() == "attribute")
				readAttributeTag();
			else
				raiseError(tr("Wrong node name \"%1\" in balise element").arg(QXmlStreamReader::name().toString()));
		}
	}
	m_parentNode.pop();
}

void XmlCompletionParser::readAttributeTag()
{
	Q_ASSERT(isStartElement() && QXmlStreamReader::name() == "attribute");

	QString
	name = attributes().value("name").toString(),
	       defaultValue = attributes().value("default").toString();

	ContentView2::Node node;
	node.setLine(lineNumber());
	node.setFileId(rootNode().fileId());
	node.setData("XmlAttribute", ContentView2::Node::NODE_TYPE);
	node.setData(":/images/noeud.png", ContentView2::Node::NODE_ICON);
	node.setData(name, ContentView2::Node::NODE_DISPLAY_NAME);
	node.setData(name, ContentView2::Node::NODE_NAME);
	node.setData(m_type, XmlCompletionParser::NODE_XML_TYPE);
	node.setData(defaultValue == "true" ? true : false, XmlCompletionParser::NODE_XML_ISDEFAULT);
	node.setData((uint)++m_index, XmlCompletionParser::NODE_XML_SORT_INDEX);

	ContentView2::Node parentNode = m_parentNode.top();
	attachNode(parentNode, node);

	m_balisesId[ parentNode.nodeId()].attributes.insert(name, node.nodeId());

	m_parentNode.push(node);
	while (!atEnd())
	{
		readNext();

		if (isEndElement())
			break;

		if (isStartElement())
		{
			if (QXmlStreamReader::name() == "value")
				readValueTag();
			else
				raiseError(tr("Wrong node name \"%1\" in attribute element").arg(QXmlStreamReader::name().toString()));
		}
	}
	m_parentNode.pop();
}

void XmlCompletionParser::readValueTag()
{
	Q_ASSERT(isStartElement() && QXmlStreamReader::name() == "value");

	QString name, defaultValue = attributes().value("default").toString();
	name = readElementText();

	ContentView2::Node node;
	node.setLine(lineNumber());
	node.setFileId(rootNode().fileId());
	node.setData(name, ContentView2::Node::NODE_NAME);
	node.setData("XmlValue", ContentView2::Node::NODE_TYPE);
	node.setData(":/images/html_value.png", ContentView2::Node::NODE_ICON);
	node.setData(name, ContentView2::Node::NODE_DISPLAY_NAME);
	node.setData(m_type, XmlCompletionParser::NODE_XML_TYPE);
	node.setData(defaultValue == "true" ? true : false, XmlCompletionParser::NODE_XML_ISDEFAULT);

	ContentView2::Node parentNode = m_parentNode.top();
	attachNode(parentNode, node);

	m_attributesId[ parentNode.nodeId()].values.insert(name, node.nodeId());
}

QSqlDatabase XmlCompletionParser::db() const
{
	return ContentView2::Manager::self()->database();
}

static bool XmlCompletionBaliseSort(ContentView2::Node node1, ContentView2::Node node2)
{
	return node1.data(XmlCompletionParser::NODE_XML_SORT_INDEX).toInt() < node2.data(XmlCompletionParser::NODE_XML_SORT_INDEX).toInt();
}

ContentView2::Node XmlCompletionParser::balise(const QString & name) const
{
	if (m_balisesName.value(name) == 0)
		return ContentView2::Node();

	return ContentView2::Node(db(), m_balisesName.value(name));
}

QList<ContentView2::Node> XmlCompletionParser::balises() const
{
	QList<ContentView2::Node> baliseList;

	foreach(uint id, m_balisesName.values())
	{
		baliseList.append(ContentView2::Node(db(), id));
	}

	qSort(baliseList.begin(), baliseList.end(), XmlCompletionBaliseSort);
	return baliseList;
}

ContentView2::Node XmlCompletionParser::baliseAttribute(const QString & name, const QString & attribute) const
{
	uint baliseId;
	if ((baliseId = m_balisesName.value(name)) == 0)
		return ContentView2::Node();

	if (m_balisesId.value(baliseId).attributes.value(attribute) == 0)
		return ContentView2::Node();

	return ContentView2::Node(db(), m_balisesId.value(baliseId).attributes.value(attribute));
}

QList<ContentView2::Node> XmlCompletionParser::baliseAttributes(const QString & name) const
{
	QList<ContentView2::Node> attributeList;
	uint baliseId;

	if ((baliseId = m_balisesName.value(name)) == 0)
		return attributeList; // Empty list

	foreach(uint id, m_balisesId.value(baliseId).attributes.values())
	{
		attributeList.append(ContentView2::Node(db(), id));
	}

	qSort(attributeList.begin(), attributeList.end(), XmlCompletionBaliseSort);
	return attributeList;
}

ContentView2::Node XmlCompletionParser::baliseOfBalise(const QString & name, const QString & baliseName) const
{
	uint baliseId;
	if ((baliseId = m_balisesName.value(name)) == 0)
		return ContentView2::Node();

	if (m_balisesId.value(baliseId).balises.value(baliseName) == 0)
		return ContentView2::Node();


	return ContentView2::Node(db(), m_balisesId.value(baliseId).balises.value(baliseName));
}

ContentView2::Node XmlCompletionParser::baliseAttributeValue(const QString & name, const QString & attribute, const QString & value) const
{
	uint baliseId, attributeId;
	if ((baliseId = m_balisesName.value(name)) == 0)
		return ContentView2::Node();

	if ((attributeId = m_balisesId.value(baliseId).attributes.value(attribute)) == 0)
		return ContentView2::Node();

	if (m_attributesId.value(attributeId).values.value(value) == 0)
		return ContentView2::Node();

	return ContentView2::Node(db(), m_attributesId.value(attributeId).values.value(value));
}

QList<ContentView2::Node> XmlCompletionParser::baliseAttributeValues(const QString & name, const QString & attribute) const
{
	QList<ContentView2::Node> valueList;
	uint baliseId, attributeId;
	if ((baliseId = m_balisesName.value(name)) == 0)
		return valueList;

	if ((attributeId = m_balisesId.value(baliseId).attributes.value(attribute)) == 0)
		return valueList;

	foreach(uint id, m_attributesId.value(attributeId).values.values())
	{
		valueList.append(ContentView2::Node(db(), id));
	}

	qSort(valueList.begin(), valueList.end(), XmlCompletionBaliseSort);
	return valueList;
}

ContentView2::Node XmlCompletionParser::defaultValue(ContentView2::Node node) const
{
	ContentView2::Node def;
	foreach(uint id, node.childs(db()))
	{
		ContentView2::Node n(db(), id);
		if ((n.data(ContentView2::Node::NODE_TYPE).toString() == "XmlValue") && (n.data(XmlCompletionParser::NODE_XML_ISDEFAULT).toBool()))
		{
			def = n;
		}
	}
	return def;
}

QList<ContentView2::Node> XmlCompletionParser::defaultAttributes(ContentView2::Node node) const
{
	QList<ContentView2::Node> defs;
	foreach(uint id, node.childs(db()))
	{
		ContentView2::Node n(db(), id);
		if ((n.data(ContentView2::Node::NODE_TYPE).toString() == "XmlAttribute") && (n.data(XmlCompletionParser::NODE_XML_ISDEFAULT).toBool()))
			defs += n;
	}
	qSort(defs.begin(), defs.end(), XmlCompletionBaliseSort);
	return defs;
}

QList<ContentView2::Node> XmlCompletionParser::defaultBalises(ContentView2::Node node)  const
{
	QList<ContentView2::Node> defs;
	foreach(uint id, node.childs(db()))
	{
		ContentView2::Node n(db(), id);
		if ((n.data(ContentView2::Node::NODE_TYPE).toString() == "XmlBalise") && (n.data(XmlCompletionParser::NODE_XML_ISDEFAULT).toBool()))
			defs += n;
	}
	qSort(defs.begin(), defs.end(), XmlCompletionBaliseSort);
	return defs;
}
