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

#ifndef CORE_JAVASCRIPT_JSCONTEXTPARSER_H
#define CORE_JAVASCRIPT_JSCONTEXTPARSER_H

#include <codecompletion/context.h>
#include <codecompletion/contextparser.h>
#include "jscontexttype.h"

// QCodeEdit header
#include <qdocumentcursor.h>

// Qt header
#include <QStringList>

namespace Core
{

namespace JavaScript
{

class JsContextParser : public CodeCompletion::ContextParser
{
public:
	JsContextParser();
	virtual ~JsContextParser();

	virtual void updateContext();

	JsContextType::Position editPosition(const QDocumentCursor & cursor, QString& functionName) const;
private:
};

}

}

#endif // CORE_JAVASCRIPT_JSCONTEXTPARSER_H
