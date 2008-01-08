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

#ifndef CSSMODELDATA_H_
#define CSSMODELDATA_H_

// Xinx header
#include <exceptions.h>
#include <filecontentstructure.h>

class IXinxExtendedEditor;

/*!
 * This exception is launch when the file can't be parsed.
 */
class CssParserException : public FileContentException {
public:
	/*!
	 * Create the exception
	 * \param message The message to show.
	 * \param line Line where the exception is throw
	 */
	CssParserException( const QString & message, int line, int column );
};

class CssFileContentProperty : public FileContentElement {
	
};

class CssFileContentIdentifier : public FileContentElement {
	
};

class CssFileContentTag : public CssFileContentIdentifier {
	
};

class CssFileContentId : public CssFileContentIdentifier {
	
};

class CSSFileContentParser : public FileContentElement, public FileContentParser {
	Q_OBJECT
public:
	CSSFileContentParser( IXinxExtendedEditor * editor, FileContentElement * parent = NULL, const QString & filename = QString(), int lineNumber = 0 );
	virtual ~CSSFileContentParser();

	virtual void loadFromContent( const QString & content );
	virtual void loadFromFile( const QString & filename );
	virtual void loadFromFileDelayed( const QString & filename );
	virtual bool isLoaded();
	
	virtual int rowCount();
private:
	enum ParsingState {
		CssDefault,
		CssIdentifier,
		CssValue
	};
	
	IXinxExtendedEditor * m_editor;
	bool m_isLoaded;
	QString m_line;
};

#endif /*CSSMODELDATA_H_*/
