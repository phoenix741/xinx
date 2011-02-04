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

#ifndef _JSCONTENTVIEWPARSER_H_
#define _JSCONTENTVIEWPARSER_H_
#pragma once

// Xinx header
#include <contentview3/parser.h>

// Qt header
#include <QApplication>

class QIODevice;

namespace Core
{

namespace JavaScript
{

class Parser : public ContentView3::Parser
{
	Q_OBJECT
public:
	Parser();
	virtual ~Parser();

	virtual QString name() const;
protected:
	virtual void parse();
private:
	enum JAVASCRIPT_TOKEN { TOKEN_UNKNOWN, TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER, TOKEN_PONCTUATION, TOKEN_EOF };

	void nextIdentifier(QIODevice * device, enum JAVASCRIPT_TOKEN & symbType, QString & symbName);
	void loadVariables(const ContentView3::NodePtr & parent, QIODevice * device);
	ContentView3::NodePtr loadFunction(const ContentView3::NodePtr & parent, QIODevice * device);
	void loadInstruction(QIODevice * buffer, QString & name, JAVASCRIPT_TOKEN & type);

	void attacheNewParamNode(const ContentView3::NodePtr & parent, const QString & name, int line);
	void attacheNewVariableNode(const ContentView3::NodePtr & parent, const QString & name, int line);
	ContentView3::NodePtr attacheNewFunctionNode(const ContentView3::NodePtr & parent, const QString & name, int line);

	int m_line;
};

}

}

#endif /* _JSCONTENTVIEWPARSER_H_ */
