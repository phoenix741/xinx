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

#pragma once
#ifndef CORE_STYLESHEET_PARSER_H
#define CORE_STYLESHEET_PARSER_H

#include <contentview3/parser.h>
#include <QApplication>
#include <QXmlStreamReader>
#include <QAbstractMessageHandler>

namespace ContentView3
{
class Node;
}

namespace Core
{

namespace Stylesheet
{

class Parser : public ContentView3::Parser, private QXmlStreamReader
{
	Q_OBJECT
public:
	Parser();
	virtual ~Parser();

	virtual QString name() const;
private:
	virtual void parse();

	struct struct_xsl_variable
	{
		bool isParam;
		int line;
		QString name;
		QString value;
	};
	struct struct_internal_script
	{
		int line;
		QString src;
		QString content;
		QString title;
	};
	struct struct_import
	{
		int line;
		QString src;
	};

	void readStyleSheet(ContentView3::NodePtr & n);
	void readVariable(ContentView3::NodePtr & n);
	void readTemplate(QList<struct_xsl_variable> & variables, QList<struct_internal_script> & scripts, QList<struct_import> & imports);
	void readTemplate(ContentView3::NodePtr & n);

	QTextCodec * m_codec;
	QAbstractMessageHandler * _handler;
};

}

}

#endif // CORE_STYLESHEET_PARSER_H
