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
#include "stylesheet_parser.h"
#include "stylesheet_importnode.h"
#include "stylesheet_templatenode.h"
#include "stylesheet_variablenode.h"
#include "stylesheet_paramnode.h"

#include <contentview3/filenode.h>
#include <plugins/xinxpluginsloader.h>
#include <plugins/interfaces/files.h>
#include <contentview3/parserfactory.h>
#include <editors/filetypepool.h>
#include <contentview3/parserexception.h>


// Qt header
#include <QTextCodec>
#include <QBuffer>

namespace Core
{
namespace Stylesheet
{

class PrivateHandler : public QAbstractMessageHandler
{
public:
	PrivateHandler(QObject * o) : QAbstractMessageHandler(o)
	{

	}

	void handleMessage(QtMsgType type, const QString& description, const QUrl& identifier, const QSourceLocation& sourceLocation)
	{
		if (type == QtCriticalMsg || type == QtFatalMsg)
		{
			throw ContentView3::ParserException(description, sourceLocation.line(), sourceLocation.column());
		}
	}
};

/* Parser */

Parser::Parser()
{
	_handler = new PrivateHandler(this);
}

Parser::~Parser()
{

}

QString Parser::name() const
{
	return "XSL";
}

void Parser::parse()
{
	ContentView3::Parser::device()->reset();
	QXmlStreamReader::setDevice(ContentView3::Parser::device());

	ContentView3::NodePtr node = rootNode();
	node->clearChilds();
	node->setIcon(":/images/typexsl.png");


	while (! atEnd())
	{
		readNext();

		if (isStartDocument())
		{
			m_codec = QTextCodec::codecForName(documentEncoding().toString().toLatin1());
		}

		if (isStartElement())
		{
			if (QXmlStreamReader::name() == "stylesheet")
				readStyleSheet(node);
			else
				raiseError(tr("The file is not an Stylesheet file."));
		}
	}

	if (error())    // Else completion can be more difficulte
	{
		throw ContentView3::ParserException(errorString(), lineNumber(), columnNumber());
	}
}

void Parser::readStyleSheet(ContentView3::NodePtr & n)
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
			{
				readVariable(n);
			}
			else if (QXmlStreamReader::name() == "variable")
			{
				readVariable(n);
			}
			else if (QXmlStreamReader::name() == "template")
			{
				readTemplate(n);
			}
			else if ((QXmlStreamReader::name() == "import") || (QXmlStreamReader::name() == "include"))
			{
				const QString import = attributes().value("href").toString();
				const QString calculateImport = addImport(import);

				QSharedPointer<ImportNode> node = ImportNode::create(calculateImport, n);
				node->setLine(decalage() + lineNumber());

				readElementText(IncludeChildElements);
			}
			else
			{
				skipCurrentElement();
			}
		}
	}
}

void Parser::readTemplate(QList<struct_xsl_variable >& variables, QList<struct_internal_script> & scripts, QList<struct_import> & imports)
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
				v.line  = decalage() + lineNumber();
				v.name  = attributes().value("name").toString();
				if (attributes().value("select").isEmpty())
				{
					v.value = readElementText(SkipChildElements);
				}
				else
				{
					v.value = attributes().value("select").toString();
					skipCurrentElement();
				}
				variables += v;
			}
			else if (QXmlStreamReader::name() == "script")
			{
				if (attributes().value("src").isEmpty())
				{
					struct struct_internal_script s;
					s.line = decalage() + lineNumber();
					s.title = "JavaScript";
					s.content = readElementText(SkipChildElements);
					s.src = "script.js";
					scripts += s;
				}
				else
				{
					struct struct_import s;
					s.line = decalage() + lineNumber();
					s.src = attributes().value("src").toString();
					skipCurrentElement();
					imports += s;
				}
			}
			else if (QXmlStreamReader::name() == "link")
			{
				struct struct_import s;
				s.line = decalage() + lineNumber();
				s.src = attributes().value("href").toString();
				skipCurrentElement();
				imports += s;
			}
			else if (QXmlStreamReader::name() == "style")
			{
				struct struct_internal_script s;
				s.line = decalage() + lineNumber();
				s.title = "Cascading StyleSheet";
				s.content = readElementText(SkipChildElements);
				s.src = "script.css";
				scripts += s;
			}
			else
				readTemplate(variables, scripts, imports);
		}
	}
}

void Parser::readTemplate(ContentView3::NodePtr & n)
{
	Q_ASSERT(isStartElement() && QXmlStreamReader::name() == "template");

	bool isNamedTemplate  = ! attributes().value("name").isEmpty();
	QStringList templates = (!isNamedTemplate ? attributes().value("match").toString() : attributes().value("name").toString()).split("|", QString::SkipEmptyParts);
	QString mode = attributes().value("mode").toString();
	int line = decalage() + lineNumber();

	QList<struct_xsl_variable> variables;
	QList<struct_internal_script> scripts;
	QList<struct_import> imports;
	readTemplate(variables, scripts, imports);

	foreach(const QString & name, templates)
	{
		QSharedPointer<TemplateNode> t = TemplateNode::create(name, n);
		t->setLine(line);
		t->setMode(mode);
		t->setIsNamedTemplate(isNamedTemplate);

		/* Chargement des imports */
		foreach(struct_import s, imports)
		{
			const QString calculateImport = addImport(s.src);

			QSharedPointer<ImportNode> node = ImportNode::create(calculateImport, n);
			node->setLine(s.line);
		}

		/* Chargement des scripts */
		foreach(struct_internal_script s, scripts)
		{
			QSharedPointer<ImportNode> import = ImportNode::create(s.title, n);
			import->setLine(line);
			import->setType("internal");
			import->setTips(tr("Element at line : %1").arg(line));

			QList<IFileTypePluginPtr> ft = XinxPluginsLoader::self()->fileTypePool()->matchedFileType(s.src);
			QScopedPointer<ContentView3::Parser> parser(ContentView3::ParserFactory::getParserByFilename(s.src));
			if (ft.size() && parser)
			{
				import->setIcon(ft.at(0)->icon());

				QByteArray textToParse = m_codec ? m_codec->fromUnicode(s.content) : textToParse = qPrintable(s.content);
				QBuffer buffer;

				buffer.setData(textToParse);
				buffer.open(QIODevice::ReadOnly);

				parser->setRootNode(import);
				parser->setFile(file());
				parser->setDevice(&buffer);
				parser->setDecalage(s.line);

				parser->startJob();
			}
		}

		/* Chargement des variables et params */
		foreach(const struct_xsl_variable & v, variables)
		{
			if (v.isParam)
			{
				QSharedPointer<ParamNode> param = ParamNode::create(v.name.trimmed(), t);
				param->setLine(v.line);
				param->setValue(v.value.trimmed());
			}
			else
			{
				QSharedPointer<VariableNode> variable = VariableNode::create(v.name.trimmed(), t);
				variable->setLine(v.line);
				variable->setValue(v.value.trimmed());
			}
		}
	}
}

void Parser::readVariable(ContentView3::NodePtr & n)
{
	Q_ASSERT(isStartElement() && ((QXmlStreamReader::name() == "param") || (QXmlStreamReader::name() == "variable")));

	int ln = lineNumber();
	QString name  = attributes().value("name").toString();
	QString value;
	if (attributes().value("select").isEmpty())
	{
		value = readElementText(SkipChildElements);
	}
	else
	{
		value = attributes().value("select").toString();
		skipCurrentElement();
	}

	if (QXmlStreamReader::name() == "param")
	{
		QSharedPointer<ParamNode> param = ParamNode::create(name.trimmed(), n);
		param->setLine(ln);
		param->setValue(value.trimmed());
	}
	else
	{
		QSharedPointer<VariableNode> variable = VariableNode::create(name.trimmed(), n);
		variable->setLine(ln);
		variable->setValue(value.trimmed());
	}
}

}

}
