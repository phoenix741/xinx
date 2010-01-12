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

#ifndef __JAVASCRIPTMODELCOMPETER_H__
#define __JAVASCRIPTMODELCOMPETER_H__
#pragma once

// Xinx header
#include <snipets/snipetcompletionnodemodel.h>

// Qt header
#include <QAbstractListModel>

class ContentViewNode;

/* JavascriptModelCompleter */

/*!
 * Class provide completion on JavaScript editor.
 * This class based on a parser show all variables, params, and function name,
 * that could be used in the JavaScript file.
 */
class JavascriptModelCompleter : public SnipetCompletionNodeModel {
	Q_OBJECT
public:
	/*!
	 * Create a Javascript model for completion, with the help of a javascript parser.
	 * \param parser The parser used to parse the javascript text
	 * \param parent Parent object used to create this object.
	 */
	JavascriptModelCompleter( ContentViewNode * root, QObject *parent = 0 );
	/*!
	 * Destroy the Javascript model.
	 */
	virtual ~JavascriptModelCompleter();
public slots:
	/*!
	 * Set the filter used to define the scope of the function.
	 * If the function is NULL, the scope is global.
	 */
	void setFilter( const QString functionName = QString() );

protected:
	virtual bool mustElementBeShowed( ContentViewNode * node );

private:
	QString m_functionFiltre;
};

#endif // __JAVASCRIPTMODELCOMPETER_H__
