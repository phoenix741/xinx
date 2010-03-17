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
#include "xslcontentviewparser.h"
#include <contentview2/contentview2node.h>
#include <plugins/plugininterfaces.h>
#include <plugins/xinxpluginsloader.h>
#include <core/xinxcore.h>
#include <contentview2/contentview2node.h>
#include <contentview2/contentview2cache.h>
#include <contentview2/contentview2parserfactory.h>
#include <project/xinxproject.h>

// Qt header
#include <QTextCodec>
#include <QIcon>
#include <QVariant>
#include <QQueue>
#include <QBuffer>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>

/* XslContentView2Parser */

XslContentView2Parser::XslContentView2Parser() : ContentView2::Parser(), m_codec(0)
{
}

XslContentView2Parser::~XslContentView2Parser()
{
}

void XslContentView2Parser::load()
{
	setDevice(inputDevice());

	if (rootNode().isValid())
	{
		rootNode().setData(":/images/typexsl.png", ContentView2::Node::NODE_ICON);
		rootNode().update(database());
	}

	loadAttachedNode(rootNode());

	while (! atEnd())
	{
		readNext();

		if (isStartDocument())
		{
			m_codec = QTextCodec::codecForName(documentEncoding().toString().toLatin1());
		}

		if (isStartElement())
		{
			if (name() == "stylesheet")
				readStyleSheet();
			else
				raiseError(tr("The file is not an Stylesheet file."));
		}
	}

	if (!error())    // Else completion can be more difficulte
	{
		detachAttachedNode();
	}
	else
	{
		throw ContentView2::ParserException(errorString(), lineNumber(), columnNumber());
	}
}

QString XslContentView2Parser::readElementText()
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

void XslContentView2Parser::readStyleSheet()
{
	Q_ASSERT(isStartElement() && QXmlStreamReader::name() == "stylesheet");

	while (!atEnd())
	{
		readNext();

		if (isEndElement())
			break;

		if (isStartElement())
		{
			if (QXmlStreamReader::name() == "param")
				readVariable();
			else if (QXmlStreamReader::name() == "variable")
				readVariable();
			else if (QXmlStreamReader::name() == "template")
				readTemplate();
			else if ((QXmlStreamReader::name() == "import") || (QXmlStreamReader::name() == "include"))
			{
				const QString import = attributes().value("href").toString();
				const QString calculateImport = addImport(import);

				ContentView2::Node node;
				node.setLine(lineNumber());
				node.setFileId(rootNode().fileId());
				node.setData(calculateImport, ContentView2::Node::NODE_NAME);
				node.setData(QFileInfo(import).fileName(), ContentView2::Node::NODE_DISPLAY_NAME);
				node.setData("Import", ContentView2::Node::NODE_TYPE);

				IFileTypePlugin * fileType = XinxPluginsLoader::self()->matchedFileType(import);
				if( fileType )
				{
					node.setData(fileType->icon(), ContentView2::Node::NODE_ICON);
				}
				else
				{
					node.setData(":/images/import.png", ContentView2::Node::NODE_ICON);
				}
				attachNode(rootNode(), node);

				readElementText();
			}
			else
				readUnknownElement();
		}
	}
}

void XslContentView2Parser::readVariable()
{
	Q_ASSERT(isStartElement() && ((QXmlStreamReader::name() == "param") || (QXmlStreamReader::name() == "variable")));

	int ln = lineNumber();
	QString name  = attributes().value("name").toString();
	QString value;
	if (attributes().value("select").isEmpty())
	{
		value = readElementText();
	}
	else
	{
		value = attributes().value("select").toString();
		readUnknownElement();
	}

	if (rootNode().isValid())
	{
		if (QXmlStreamReader::name() == "param")
			attacheNewParamsNode(rootNode(), name.trimmed(), value, ln);
		else
			attacheNewVariableNode(rootNode(), name.trimmed(), value, ln);
	}
}

void XslContentView2Parser::readTemplate(QList<struct_xsl_variable> & variables, QList<struct_script> & scripts)
{
	Q_ASSERT(isStartElement());

	while (!atEnd())
	{
		readNext();

		if (isEndElement())
			break;

		if (isStartElement())
		{
			if ((QXmlStreamReader::name() == "param") || (QXmlStreamReader::name() == "variable"))
			{
				struct struct_xsl_variable v;
				v.isParam = QXmlStreamReader::name() == "param";
				v.line  = lineNumber();
				v.name  = attributes().value("name").toString();
				if (attributes().value("select").isEmpty())
				{
					v.value = readElementText();
				}
				else
				{
					v.value = attributes().value("select").toString();
					readUnknownElement();
				}
				variables += v;
			}
			else if (QXmlStreamReader::name() == "script")
			{
				struct struct_script s;
				s.line = lineNumber();
				s.title = "JavaScript";
				s.content = readElementText();
				s.src = "script.js";
				scripts += s;
			}
			else if (QXmlStreamReader::name() == "style")
			{
				struct struct_script s;
				s.line = lineNumber();
				s.title = "Cascading StyleSheet";
				s.content = readElementText();
				s.src = "script.css";
				scripts += s;
			}
			else
				readTemplate(variables, scripts);
		}
	}
}

void XslContentView2Parser::readTemplate()
{
	Q_ASSERT(isStartElement() && QXmlStreamReader::name() == "template");

	QStringList templates = (attributes().value("name").isEmpty() ? attributes().value("match").toString() : attributes().value("name").toString()).split("|", QString::SkipEmptyParts);
	QString mode = attributes().value("mode").toString();
	int line = lineNumber();

	QList<struct_xsl_variable> variables;
	QList<struct_script> scripts;
	readTemplate(variables, scripts);

	foreach(const QString & name, templates)
	{
		ContentView2::Node t;
		if (rootNode().isValid())
			t = attacheNewTemplateNode(rootNode(), name.trimmed(), mode, line);

		if (! t.isValid()) continue;

		loadAttachedNode(t);
		/* Chargement des scripts */
		foreach(struct_script s, scripts)
		{
			ContentView2::Node node;
			node.setData(s.title, ContentView2::Node::NODE_NAME);
			node.setData(s.title, ContentView2::Node::NODE_DISPLAY_NAME);
			node.setData("internal", ContentView2::Node::NODE_TYPE);
			node.setData(tr("Element at line : %1").arg(s.line), ContentView2::Node::NODE_DISPLAY_TIPS);
			node.setData(":/images/import.png", ContentView2::Node::NODE_ICON);
			node.setLine(s.line);
			node.setFileId(rootNode().fileId());

			// Il nous faut un parser JavaScript
			IFileTypePlugin * ft = XinxPluginsLoader::self()->matchedFileType(s.src);
			ContentView2::Parser * parser = ContentView2::ParserFactory::getParserByFilename(s.src);
			if (ft && parser)
			{
				try
				{
					node.setData(ft->icon(), ContentView2::Node::NODE_ICON);

					attachNode(t, node);

					QByteArray textToParse = qPrintable(s.content);
					if (codec())
						textToParse = codec()->fromUnicode(s.content);
					QBuffer buffer;
					buffer.setData(textToParse);
					buffer.open(QIODevice::ReadOnly);

					parser->setRootNode(node);
					parser->setInputDevice(&buffer);
					parser->setDecalage(s.line);
					parser->setDatabase(database());

					parser->load();
				}
				catch (ContentView2::ParserException e)
				{
				}
				delete parser;
			}
			else
				attachNode(t, node);
		}

		/* Chargement des variables et params */
		foreach(const struct_xsl_variable & v, variables)
		{
			if (v.isParam)
				attacheNewParamsNode(t, v.name.trimmed(), v.value.trimmed(), v.line);
			else
				attacheNewVariableNode(t, v.name.trimmed(), v.value.trimmed(), v.line);
		}
	}
}

void XslContentView2Parser::readUnknownElement()
{
	Q_ASSERT(isStartElement());

	while (!atEnd())
	{
		readNext();

		if (isEndElement())
			break;

		if (isStartElement())
			readUnknownElement();
	}
}

ContentView2::Node XslContentView2Parser::attacheNewTemplateNode(ContentView2::Node parent, const QString & name, const QString & mode, int line)
{
	QString displayName = name;
	if (! mode.isEmpty())
		displayName += " [" + mode + "]";

	ContentView2::Node node;
	node.setLine(line);
	node.setFileId(rootNode().fileId());
	node.setData(name, ContentView2::Node::NODE_NAME);
	node.setData("XslTemplate",  ContentView2::Node::NODE_TYPE);
	node.setData(":/images/template.png",  ContentView2::Node::NODE_ICON);
	node.setData(displayName,  ContentView2::Node::NODE_DISPLAY_NAME);
	node.setData(tr("Element at line : %1\nMode = %2").arg(line).arg(mode),  ContentView2::Node::NODE_DISPLAY_TIPS);
	node.setData(mode,  ContentView2::Node::NODE_USER_VALUE);

	attachNode(parent, node);

	return node;
}

void XslContentView2Parser::attacheNewParamsNode(ContentView2::Node parent, const QString & name, const QString & value,  int line)
{
	QString displayName = name;
	if (! value.isEmpty())
		displayName += "=" + value;

	ContentView2::Node node;
	node.setLine(line);
	node.setFileId(rootNode().fileId());
	node.setData(name, ContentView2::Node::NODE_NAME);
	node.setData("XslParam", ContentView2::Node::NODE_TYPE);
	node.setData(":/images/html_value.png", ContentView2::Node::NODE_ICON);
	node.setData(displayName, ContentView2::Node::NODE_DISPLAY_NAME);
	node.setData(tr("Element at line : %1\nValue = %2").arg(line).arg(value), ContentView2::Node::NODE_DISPLAY_TIPS);
	node.setData(value, ContentView2::Node::NODE_USER_VALUE);
	node.setData("$%1", ContentView2::Node::NODE_COMPLETE_FORM);

	attachNode(parent, node);
}

void XslContentView2Parser::attacheNewVariableNode(ContentView2::Node parent, const QString & name, const QString & value, int line)
{
	QString displayName = name;
	if (! value.isEmpty())
		displayName += "=" + value;

	ContentView2::Node node;
	node.setLine(line);
	node.setFileId(rootNode().fileId());
	node.setData(name, ContentView2::Node::NODE_NAME);
	node.setData("XslVariable", ContentView2::Node::NODE_TYPE);
	node.setData(":/images/variable.png", ContentView2::Node::NODE_ICON);
	node.setData(displayName, ContentView2::Node::NODE_DISPLAY_NAME);
	node.setData(tr("Element at line : %1\nValue = %2").arg(line).arg(value), ContentView2::Node::NODE_DISPLAY_TIPS);
	node.setData(value, ContentView2::Node::NODE_USER_VALUE);
	node.setData("$%1", ContentView2::Node::NODE_COMPLETE_FORM);

	attachNode(parent, node);
}


