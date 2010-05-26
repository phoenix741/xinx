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

#ifndef DICTIONARYPARSER_H
#define DICTIONARYPARSER_H
#pragma once

// Xinx header
#include <contentview2/contentview2parser.h>

// Qt header
#include <QXmlStreamReader>

class DictionaryParser : public ContentView2::Parser, private QXmlStreamReader
{
	Q_DECLARE_TR_FUNCTIONS(DictionaryParser)
public:
	enum XmlCompletionRoleIndex
	{
		NODE_DICO_LANG  = ContentView2::Node::NODE_USER_VALUE,
		NODE_DICO_CTX   = ContentView2::Node::NODE_USER_VALUE + 1,
		NODE_DICO_VALUE = ContentView2::Node::NODE_USER_VALUE + 2,
	};

	DictionaryParser();
	virtual ~DictionaryParser();

	virtual void load();
private:
	void readRootNode();
	void readLabelsNode();
	void readLabelNode(ContentView2::Node parent);
};

#endif // DICTIONARYPARSER_H
