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

#ifndef _XMLCOMPLETIONPARSER_H_
#define _XMLCOMPLETIONPARSER_H_
#pragma once

// Xinx header
#include <contentview3/parser.h>

// Qt header
#include <QXmlStreamReader>

namespace Core
{

namespace BaliseDefinition
{

class BaliseNode;
class AttributeNode;
class RootNode;


/* XmlCompletionParser */

class XmlCompletionParser : public ContentView3::Parser, private QXmlStreamReader
{
	Q_OBJECT
public:
	XmlCompletionParser();
	~XmlCompletionParser();

	virtual QString name() const;
protected:
	virtual void parse();

private:
	void readRootTag(const QSharedPointer<RootNode> & rootNode);
	void readBaliseTag(const ContentView3::NodePtr & rootNode);
	void readAttributeTag(const QSharedPointer<BaliseNode> & parent);
	void readValueTag(const QSharedPointer<AttributeNode> & parent);
};

}

}

#endif /* _XMLCOMPLETIONPARSER_H_ */
