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

#ifndef CSSMODELDATA_H_
#define CSSMODELDATA_H_
#pragma once

// Xinx header
#include <contentview2/contentview2parser.h>

// Qt header
#include <QApplication>

class CSSFileContentParser : public ContentView2::Parser
{
	Q_DECLARE_TR_FUNCTIONS(CSSFileContentParser)
public:
	CSSFileContentParser();
	virtual ~CSSFileContentParser();

	virtual void load();

private:
	enum ParsingState
	{
		CssDefault,
		CssIdentifier
	};

	int m_line;

	ContentView2::Node attacheNewPropertyNode(ContentView2::Node parent, const QString & name, int line);
	ContentView2::Node attacheNewIdentifierNode(ContentView2::Node parent, const QString & name, int line);
	ContentView2::Node attacheNewClassNode(ContentView2::Node parent, const QString & name, int line);
	ContentView2::Node attacheNewTagNode(ContentView2::Node parent, const QString & name, int line);
	ContentView2::Node attacheNewIdNode(ContentView2::Node parent, const QString & name, int line);
};

#endif /*CSSMODELDATA_H_*/
