/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef __JAVASCRIPTPARSER_H__
#define __JAVASCRIPTPARSER_H__

#include "filecontentitemmodel.h"
#include "filecontentstructure.h"

#include <QStringList>
#include <QList>

class PrivateJavaScriptParser;

/*!
 * This exception is launch when the file can't be parsed.
 */
class JavaScriptParserException : public FileContentException {
public:
	/*!
	 * Create the exception
	 * \param line Line where the exception is throw
	 */
	JavaScriptParserException( const QString & message, int line, int column = 0 );
};

/*!
 * Class represent params in a Javascript's function.
 */
class JavaScriptParams : public FileContentElement {
	Q_OBJECT
public:
	/*!
	 * Create a param of a JavaScript Function
	 * \param name Name of the parameter.
	 * \param line Line of the parameter.
	 */
	JavaScriptParams( FileContentElement * parent, const QString & name, int line );

	virtual QIcon icon() const;
};

/*!
 * Class represent a variable in a Javascript's function or in a global definition.
 */
class JavaScriptVariables : public JavaScriptParams {
	Q_OBJECT
public:
	/*!
	 * Create a variables.
	 * \param name Name of the variable
	 * \param line Line of the parameter.
	 */
	JavaScriptVariables( FileContentElement * parent, const QString & name, int line );

	virtual QIcon icon() const;
};

/*!
 * Class represent a function in the Javascript file.
 * A class can have parameter and variables.
 */
class JavaScriptFunction : public FileContentElement {
	Q_OBJECT
public:
	/*!
	 * Create a function.
	 * \param name Name of the function.
	 * \param line Line of the function.
	 */
	JavaScriptFunction( FileContentElement * parent, const QString & name, int line );

	virtual QIcon icon() const;
private:
	friend class PrivateJavaScriptParser;
	friend class JavaScriptParser;
};

/*!
 * This is a simple Javascript parser who create a tree with variables, functions, ...
 * \todo Detect non declared variable.
 * \todo Make the parser more complexe ;)
 */
class JavaScriptParser : public FileContentElement, public FileContentParser {
	Q_OBJECT
public:
	/*!
	 * Create the parser with the content of a Javascript file.
	 * \param content Content of the Javascript file.
	 */
	JavaScriptParser();
	JavaScriptParser( FileContentElement * parent, const QString & filename, int lineNumber );
	/*!
	 * Delete the parser
	 */
	virtual ~JavaScriptParser();
	
	/*!
	 * Load and parse the content of the file.
	 * \param content Content of the file
	 */
	virtual void loadFromContent( const QString & content );
	virtual void loadFromFile( const QString & filename );

	virtual QIcon icon() const;
private:
	PrivateJavaScriptParser * d;
	friend class PrivateJavaScriptParser;
	friend class PrivateJavaScriptFunction;
	friend class JavaScriptFunction;
};

#endif // __JAVASCRIPTPARSER_H__
