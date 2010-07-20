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

// Xinx header
#include "javascriptmodelcompleter.h"

// Qt header
#include <QIcon>

/* JavascriptModelCompleter */

JavascriptModelCompleter::JavascriptModelCompleter(QSqlDatabase db, ContentView2::FileContainer file, QObject *parent) : ContentView2::CompletionModel(db, file, parent), m_functionFiltre(QString())
{

}

JavascriptModelCompleter::~JavascriptModelCompleter()
{

}

void JavascriptModelCompleter::setFilter(const QString functionName)
{
	if (m_functionFiltre != functionName)
	{
		m_functionFiltre = functionName;
		select();
	}
}

QString JavascriptModelCompleter::whereClause(QList<QVariant> & parameters) const
{
	QString clause = ContentView2::CompletionModel::whereClause(parameters);
	parameters << m_functionFiltre;

	clause += " AND ( ( cv_node.type = 'Snipet' ) ";

	clause +=   " OR ( cv_node.type like 'Js%' AND EXISTS ( "
				"	SELECT 1 "
				"	FROM cv_node nodeParent "
				"	WHERE cv_node.parent_id=nodeParent.id "
				"	  AND (nodeParent.type <> 'JsFunction' OR nodeParent.name=? ))) ";

	clause += ")";

	return clause;
}
