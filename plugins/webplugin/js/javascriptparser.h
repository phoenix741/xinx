/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
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

// Xinx header
#include "filecontentstructure.h"

// Qt header
#include <QStringList>
#include <QList>

class JavaScriptParser;

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
	virtual void loadFromFile( const QString & filename );
	virtual void loadFromFileDelayed( const QString & filename );
	virtual void loadFromDevice( QIODevice * device );
	
	virtual bool isLoaded();
	
	virtual int rowCount();
	virtual QIcon icon() const;
private:
	enum JAVASCRIPT_TOKEN { TOKEN_UNKNOWN, TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER, TOKEN_PONCTUATION, TOKEN_EOF };

	void nextIdentifier( QIODevice * device, enum JAVASCRIPT_TOKEN & symbType, QString & symbName );
	QList<JavaScriptVariables*> loadVariables( QIODevice * device );
	JavaScriptFunction * loadFunction( QIODevice * device, QList<JavaScriptParams*> * params );
	void loadInstruction( QIODevice * buffer, JavaScriptFunction * function, QString & name, JAVASCRIPT_TOKEN & type );

	int m_line;
	bool m_isLoaded;
};

#endif // __JAVASCRIPTPARSER_H__
