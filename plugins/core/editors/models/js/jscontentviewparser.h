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
#include <contentview2/contentview2parser.h>

// Qt header
#include <QApplication>

class QIODevice;

class JsContentViewParser : public ContentView2::Parser
{
	Q_DECLARE_TR_FUNCTIONS(JsContentViewParser)
public:
	JsContentViewParser();
	virtual ~JsContentViewParser();

	virtual void load();
private:
	enum JAVASCRIPT_TOKEN { TOKEN_UNKNOWN, TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER, TOKEN_PONCTUATION, TOKEN_EOF };

	void nextIdentifier(QIODevice * device, enum JAVASCRIPT_TOKEN & symbType, QString & symbName);
	void loadVariables(ContentView2::Node parent, QIODevice * device);
	ContentView2::Node loadFunction(ContentView2::Node parent, QIODevice * device);
	void loadInstruction(QIODevice * buffer, QString & name, JAVASCRIPT_TOKEN & type);

	void attacheNewParamNode(ContentView2::Node parent, const QString & name, int line);
	void attacheNewVariableNode(ContentView2::Node parent, const QString & name, int line);
	ContentView2::Node attacheNewFunctionNode(ContentView2::Node parent, const QString & name, int line);

	int m_line;
};

#endif /* _JSCONTENTVIEWPARSER_H_ */
