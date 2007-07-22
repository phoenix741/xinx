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

#include <QStringList>
#include <QList>

class PrivateJavaScriptElement;
class PrivateJavaScriptFunction;
class PrivateJavaScriptParser;

/*!
 * This exception is launch when the file can't be parsed.
 */
class JavaScriptParserException {
public:
	/*!
	 * Create the exception
	 * \param line Line where the exception is throw
	 */
	JavaScriptParserException( int line );
	
	/*!
	 * Line where the exception is throw.
	 */
	int m_line;
};

/*!
 * Javascript element of the file. It represent the file himself, the functions, the params, the variables,
 * or other element.
 */
class JavaScriptElement {
public:
	/*!
	 * Create an element with a name and the line where the element is.
	 * \param name Name of the element.
	 * \param line Line where the element is.
	 */
	JavaScriptElement( JavaScriptElement * parent, const QString & name, int line );
	/*!
	 * Destroy the element.
	 */
	virtual ~JavaScriptElement();
	
	/*!
	 * Name of the element.
	 * \return Return the name of the element.
	 */
	virtual const QString & name();
	/*!
	 * Return the line where the element is.
	 * \return the line of the element.
	 */
	int line();
	
	/*!
	 * Count the number of child of the element.
	 * \return the number of child.
	 */
	virtual int rowCount();
	/*!
	 * Return the index-th child element of the object. Return null if i can't.
	 * \param index Index of the child element.
	 * \return The child element.
	 */
	virtual JavaScriptElement * element( int index );
	
	/*!
	 * Return the parent of this element.
	 * \return The parent of this element.
	 */
	virtual JavaScriptElement * parent();
	 
	/*!
	 * Return the row index in the parent list.
	 * \return The index of the element
	 */ 
	virtual int row();
protected:
	/*!
	 * Function to change the name of the element. This function is only accessible
	 * by children.
	 * \param name New name of the element.
	 */
	void setName( const QString & name );
	/*!
	 * Function to change the line of the element. This function is only accessible
	 * by childre of the class.
	 * \param line Line of Javascript element.
	 */
	void setLine( int line );
private:
	PrivateJavaScriptElement * d;
	friend class PrivateJavaScriptElement;
};

/*!
 * Class represent params in a Javascript's function.
 */
class JavaScriptParams : public JavaScriptElement {
public:
	/*!
	 * Create a param of a JavaScript Function
	 * \param name Name of the parameter.
	 * \param line Line of the parameter.
	 */
	JavaScriptParams( JavaScriptElement * parent, const QString & name, int line );
};

/*!
 * Class represent a variable in a Javascript's function or in a global definition.
 */
class JavaScriptVariables : public JavaScriptParams {
public:
	/*!
	 * Create a variables.
	 * \param name Name of the variable
	 * \param line Line of the parameter.
	 */
	JavaScriptVariables( JavaScriptElement * parent, const QString & name, int line );
};

/*!
 * Class represent a function in the Javascript file.
 * A class can have parameter and variables.
 */
class JavaScriptFunction : public JavaScriptElement {
public:
	/*!
	 * Create a function.
	 * \param name Name of the function.
	 * \param line Line of the function.
	 */
	JavaScriptFunction( JavaScriptElement * parent, const QString & name, int line );
	/*!
	 * Detele the function
	 */
	virtual ~JavaScriptFunction();

	/*!
	 * List of the params of the function
	 * \return The list of the params.
	 */
	const QList<JavaScriptParams*> & params();
	/*!
	 * List of the variables of the function
	 * \return The list of the variables.
	 */
	const QList<JavaScriptVariables*> & variables();
	
	virtual int rowCount();
	virtual JavaScriptElement * element( int index );
private:
	PrivateJavaScriptFunction * d;
	friend class PrivateJavaScriptFunction;
	friend class PrivateJavaScriptParser;
	friend class JavaScriptParser;
};

/*!
 * This is a simple Javascript parser who create a tree with variables, functions, ...
 * \todo Detect non declared variable.
 * \todo Make the parser more complexe ;)
 */
class JavaScriptParser : public JavaScriptElement {
public:
	/*!
	 * Create the parser with the content of a Javascript file.
	 * \param content Content of the Javascript file.
	 */
	JavaScriptParser( const QString & content );
	/*!
	 * Delete the parser
	 */
	virtual ~JavaScriptParser();
	
	/*!
	 * List all globals variables.
	 * \return list of variables.
	 */
	const QList<JavaScriptVariables*> & variables();
	/*! 
	 * List the function in the file.
	 * \return list of functions.
	 */
	const QList<JavaScriptFunction*> & functions();

	virtual int rowCount();
	virtual JavaScriptElement * element( int index );
protected:
	/*!
	 * Load and parse the content of the file.
	 * \param content Content of the file
	 */
	void load( const QString & content );
private:
	PrivateJavaScriptParser * d;
	friend class PrivateJavaScriptParser;
	friend class PrivateJavaScriptFunction;
	friend class JavaScriptFunction;
};

#endif // __JAVASCRIPTPARSER_H__
