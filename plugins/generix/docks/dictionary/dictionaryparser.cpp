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
#include "dictionaryparser.h"
#include <contentview2/contentview2cache.h>
#include <contentview2/contentview2manager.h>

// Qt header
#include <QTextCodec>
#include <QVariant>
#include <QFile>

DictionaryParser::DictionaryParser() : ContentView2::Parser(), m_codec(0)
{
}

DictionaryParser::~DictionaryParser()
{
}

/*
 * <?xml version = '1.0' encoding = 'ISO-8859-1'?>
 * <root default="FRA" xsi:noNamespaceSchemaLocation="trad.xsd" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
 *   <labels code="LBL.A">
 *      <label lang="FRA" ctx="a_client_dossier.xsl" value="a"/>
 *      <label lang="FRA" ctx="f_balance_agee_criteres.xsl" value="a"/>
 *   </labels>
 * </root>
 */
void DictionaryParser::load()
{
	if (rootNode().isValid())
	{
		rootNode().setData(":/images/typexml.png", ContentView2::Node::NODE_ICON);
		rootNode().update(database());
	}

	inputDevice()->reset();
	setDevice(inputDevice());

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
			if (name() == "root")
				readRootNode();
			else
				raiseError(tr("The file is not an dictionary file."));
		}
	}

	if (error())
	{
		throw ContentView2::ParserException(errorString(), lineNumber(), columnNumber());
	}

	detachAttachedNode();
}


void DictionaryParser::readRootNode()
{
	Q_ASSERT(isStartElement() && QXmlStreamReader::name() == "root");

	while (!atEnd())
	{
		readNext();

		if (isEndElement())
			break;

		if (isStartElement())
		{
			if (QXmlStreamReader::name() == "labels")
				readLabelsNode();
			else
				raiseError(tr("Labels node expected. %1 found").arg(QXmlStreamReader::name().toString()));
		}
	}
}

void DictionaryParser::readLabelsNode()
{
	Q_ASSERT(isStartElement() && QXmlStreamReader::name() == "labels");

	QString code  = attributes().value("code").toString();

	ContentView2::Node labels;
	labels.setFileId(rootNode().fileId());
	labels.setLine(lineNumber());
	labels.setData(code, ContentView2::Node::NODE_NAME);
	labels.setData(code, ContentView2::Node::NODE_DISPLAY_NAME);
	labels.setData("XslVariable", ContentView2::Node::NODE_TYPE);
	labels.setData(":/generix/images/dictionary16.png", ContentView2::Node::NODE_ICON);
	labels.setData("gnx:trad($LANGUAGE,'%1',$CTXTRD)", ContentView2::Node::NODE_COMPLETE_FORM);
	if (rootNode().isValid())
	{
		attachNode(rootNode(), labels);
	}

	while (!atEnd())
	{
		readNext();

		if (isEndElement())
			break;

		if (isStartElement())
		{
			if (QXmlStreamReader::name() == "label")
				readLabelNode(labels);
			else
				raiseError(tr("Labels node expected. %1 found").arg(QXmlStreamReader::name().toString()));
		}
	}
}

void DictionaryParser::readLabelNode(ContentView2::Node parent)
{
	Q_ASSERT(isStartElement() && QXmlStreamReader::name() == "label");

	QString lang  = attributes().value("lang").toString();
	QString ctx   = attributes().value("ctx").toString();
	QString value = attributes().value("value").toString();

	ContentView2::Node label;
	label.setFileId(rootNode().fileId());
	label.setLine(lineNumber());
	label.setData(value, ContentView2::Node::NODE_NAME);
	label.setData(QString("%1 [Langue=%2] [Ctx=%3]").arg(value).arg(lang).arg(ctx), ContentView2::Node::NODE_DISPLAY_NAME);
	label.setData("DICTIONARY_LABEL", ContentView2::Node::NODE_TYPE);
	label.setData(":/generix/images/label.png", ContentView2::Node::NODE_ICON);
	label.setData(lang, NODE_DICO_LANG);
	label.setData(ctx, NODE_DICO_CTX);
	label.setData(value, NODE_DICO_VALUE);
	if (rootNode().isValid())
	{
		attachNode(parent, label);
	}

	while (!atEnd())
	{
		readNext();

		if (isEndElement())
			break;

		if (isStartElement())
		{
			raiseError(tr("Label node expected. %1 found").arg(QXmlStreamReader::name().toString()));
		}
	}
}

