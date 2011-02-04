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

#pragma once
#ifndef CSSMODELDATA_H_
#define CSSMODELDATA_H_

// Xinx header
#include <contentview3/parser.h>

// Qt header
#include <QApplication>

namespace Core
{

namespace CascadingStyleSheet
{

class IdNode;
class TagNode;
class ClassNode;
class IdentifierNode;
class PropertyNode;

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
	enum ParsingState
	{
		CssDefault,
		CssIdentifier
	};

	int m_line;

	QSharedPointer<PropertyNode> attacheNewPropertyNode(const ContentView3::NodePtr & parent, const QString & name, int line);
	QSharedPointer<IdentifierNode> attacheNewIdentifierNode(const ContentView3::NodePtr & parent, const QString & name, int line);
	QSharedPointer<ClassNode> attacheNewClassNode(const ContentView3::NodePtr & parent, const QString & name, int line);
	QSharedPointer<TagNode> attacheNewTagNode(const ContentView3::NodePtr & parent, const QString & name, int line);
	QSharedPointer<IdNode> attacheNewIdNode(const ContentView3::NodePtr & parent, const QString & name, int line);
};

}

}

#endif /*CSSMODELDATA_H_*/
