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
 
#ifndef _XSLLISTVIEW_H_
#define _XSLLISTVIEW_H_

// Xinx header
#include <exceptions.h>
#include <filecontentstructure.h>

/*!
 * This exception is launch when the file can't be parsed.
 */
class XMLParserException : public FileContentException {
public:
	/*!
	 * Create the exception
	 * \param message The message to show.
	 * \param line Line where the exception is throw
	 */
	XMLParserException( const QString & message, int line, int column );
};

class QDomElement;
class IXinxExtendedEditor;

class XSLFileContentParams : public FileContentElement {
	Q_OBJECT
public:
	XSLFileContentParams( FileContentElement * parent, const QDomElement & node );
	virtual ~XSLFileContentParams();
	
	const QString & value() const;
	virtual QString displayTips() const;

	virtual void copyFrom( FileContentElement * element );

	virtual QIcon icon() const;
private:
	QString m_value;
};

class XSLFileContentVariable : public XSLFileContentParams {
	Q_OBJECT
public:
	XSLFileContentVariable( FileContentElement * parent, const QDomElement & node );
	virtual ~XSLFileContentVariable();

	virtual QIcon icon() const;
};

class XSLFileContentTemplate : public FileContentElement {
	Q_OBJECT
public:
	XSLFileContentTemplate( IXinxExtendedEditor * editor, FileContentElement * parent, const QDomElement & node, const QString & name );
	virtual ~XSLFileContentTemplate();

	const QString & mode() const;
	
	virtual QString displayName() const;

	virtual bool equals( FileContentElement * element );
	virtual void copyFrom( FileContentElement * element );

	virtual QIcon icon() const;

	void loadFromXML( const QDomElement& node );
private:
	QString m_mode;
	IXinxExtendedEditor * m_editor;
};

class XSLFileContentParser : public FileContentElement, public FileContentParser {
	Q_OBJECT
public:
	XSLFileContentParser( IXinxExtendedEditor * editor, FileContentElement * parent, const QString & filename, int lineNumber );
	XSLFileContentParser( IXinxExtendedEditor * editor );
	virtual ~XSLFileContentParser();

	virtual void loadFromContent( const QString & content );
	virtual void loadFromFile( const QString & filename );
	virtual void loadFromFileDelayed( const QString & filename );
	virtual bool isLoaded();
	
	virtual int rowCount();
	virtual QIcon icon() const;
protected:
	void loadFromXML( const QDomElement& element );
private:
	IXinxExtendedEditor * m_editor;
	bool m_isLoaded;
};

#endif
