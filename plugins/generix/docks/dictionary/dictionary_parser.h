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
#ifndef GENERIX_DICTIONARY_PARSER_H
#define GENERIX_DICTIONARY_PARSER_H

// Xinx header
#include <contentview3/parser.h>

// Qt header
#include <QApplication>
#include <QXmlStreamReader>

namespace ContentView3
{
	class Node;
}

namespace Generix {

namespace Dictionary {

class Parser : public ContentView3::Parser, private QXmlStreamReader
{
	Q_OBJECT
public:
	Parser();
	virtual ~Parser();

	virtual QString name() const;

	using ContentView3::Parser::setDevice;
protected:
	virtual void parse();

private:
	void readRootNode(ContentView3::NodePtr n);
	void readLabelsNode(ContentView3::NodePtr n);
	void readLabelNode(ContentView3::NodePtr n);
};

}

}

#endif // GENERIX_DICTIONARY_PARSER_H
