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

#include "dictionary_parser.h"
#include <contentview3/filenode.h>
#include <contentview3/file.h>
#include "dictionary_labelnode.h"
#include "dictionary_labelsnode.h"
#include <contentview3/filenode.h>

namespace Generix
{

namespace Dictionary
{

/* Parser */

Parser::Parser()
{

}

Parser::~Parser()
{

}

QString Parser::name() const
{
	return "GNX_DICO";
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
void Parser::parse()
{
	ContentView3::Parser::device()->reset();
	QXmlStreamReader::setDevice(ContentView3::Parser::device());

	ContentView3::NodePtr node = rootNode();
	node->clearChilds();
	node->setIcon(":/images/typexml.png");

	while (! atEnd())
	{
		readNext();

		if (isStartElement())
		{
			if (QXmlStreamReader::name() == "root")
				readRootNode(node);
			else
				raiseError(tr("The file is not an dictionary file."));
		}
	}

	if (error())
	{
		throw ContentView3::ParserException(errorString(), lineNumber(), columnNumber());
	}
}

void Parser::readRootNode(ContentView3::NodePtr n)
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
				readLabelsNode(n);
			else
				raiseError(tr("Labels node expected. %1 found").arg(QXmlStreamReader::name().toString()));
		}
	}
}

void Parser::readLabelsNode(ContentView3::NodePtr n)
{
	Q_ASSERT(isStartElement() && QXmlStreamReader::name() == "labels");

	QString code  = attributes().value("code").toString();

	QSharedPointer<LabelsNode> labels = LabelsNode::create(code, n);
	labels->setLine(lineNumber());

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

void Parser::readLabelNode(ContentView3::NodePtr n)
{
	Q_ASSERT(isStartElement() && QXmlStreamReader::name() == "label");

	QString lang  = attributes().value("lang").toString();
	QString ctx   = attributes().value("ctx").toString();
	QString value = attributes().value("value").toString();

	QSharedPointer<LabelNode> label = LabelNode::create(value, n);
	label->setLine(lineNumber());
	label->setLang(lang);
	label->setContext(ctx);
	label->setValue(value);

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

}

}

