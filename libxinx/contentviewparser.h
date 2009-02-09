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

/*!
 * \file contentviewparser.h
 * \brief Contains the element for manage the parser.
 */

#ifndef __CONTENTVIEWPARSER_H__
#define __CONTENTVIEWPARSER_H__

// Xinx header
#include "exceptions.h"

class ContentViewNode;

/*!
 * \class ContentViewException
 * \brief Exception throw when the model can't be updated.
 */
class  ContentViewException : public XinxException {
public:
	/*!
	 * Create the exception with a message and a line.
	 * \param message Error of the exception.
	 * \param line Line where the error is.
	 */
	ContentViewException( QString message, int line, int column );

	/*!
	 * Return the line where the error is.
	 * \return The line of the error.
	 */
	int getLine() const;

	/*!
	 * Return the column where the error is.
	 * \return the column of the error.
	 */
	int getColumn() const;
private:
	int m_line, m_column;
};


/*!
 * \class ContentViewParser
 * \brief The content view parser create the content view tree.
 *
 * The parser read the content view tree and return the root element :
 * a content view node. The parser must be sub-classed and must redefine
 * the method \e loadFromDevice().
 *
 */
class ContentViewParser {
	virtual ~ContentViewParser();

	/*! Loed the content of the device. and return the root element of the content view */
	virtual void loadFromDevice( ContentViewNode * rootNode, QIODevice * device ) = 0;
	/*! Construct elements from \e content. */
	virtual void loadFromContent( ContentViewNode * rootNode, const QString & content );
	/*! Construct elements from \e filename. */
	virtual void loadFromFile( ContentViewNode * rootNode, const QString & filename );
};

#endif /* __CONTENTVIEWPARSER_H__ */
