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

#ifndef CORE_BALISEDEFINITION_XMLCONTEXTPARSER_H
#define CORE_BALISEDEFINITION_XMLCONTEXTPARSER_H

#include <codecompletion/context.h>
#include <codecompletion/contextparser.h>
#include "xmlcontexttype.h"

// QCodeEdit header
#include <qdocumentcursor.h>

// Qt header
#include <QStringList>

namespace Core {

namespace BaliseDefinition {

class XmlBalise;

class XmlContextParser : public CodeCompletion::ContextParser
{
public:
	XmlContextParser();
	virtual ~XmlContextParser();

	virtual void updateContext();

	static QString xpathToString(const QList<XmlBalise> & xp);
	QStack<XmlBalise> xpath(const QDocumentCursor & cursor, const QStringList& includeOnly = QStringList(), const QString& prefix = QString(), const QStringList& attributeName = QStringList()) const;
	XmlContextType::Position editPosition(const QDocumentCursor & cursor, QString & nodeName, QString & paramName) const;
private:
	XmlBalise createBalise(const QString& text, const QStringList& attributeName = QStringList()) const;
	XmlBalise currentBalise(const QDocumentCursor & cursor) const;
};

}

}

#endif // CORE_BALISEDEFINITION_XMLCONTEXTPARSER_H
