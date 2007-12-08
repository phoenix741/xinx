/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
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

#include "filecontentitemmodel.h"
#include "filecontentstructure.h"
#include "javascriptparser.h"

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
class XSLProject;

class XSLFileContentImport;
class PrivateXSLFileContentParser;
class PrivateXSLFileContentImportJavaScript;
class PrivateXSLFileContentParams;
class PrivateXSLFileContentTemplate;

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
	PrivateXSLFileContentParams * d;
	friend class PrivateXSLFileContentParams;
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
	XSLFileContentTemplate( FileContentElement * parent, const QDomElement & node, const QString & name );
	virtual ~XSLFileContentTemplate();

	const QString & mode() const;
	
	virtual QString displayName() const;

	virtual bool equals( FileContentElement * element );
	virtual void copyFrom( FileContentElement * element );

	virtual QIcon icon() const;

	void loadFromXML( const QDomElement& node );
private:
	PrivateXSLFileContentTemplate * d;
	friend class PrivateXSLFileContentTemplate;
};

class XSLFileContentParser : public FileContentElement {
	Q_OBJECT
public:
	XSLFileContentParser();
	virtual ~XSLFileContentParser();

	void loadFromContent( const QString & content );

	virtual QIcon icon() const;
protected:
	XSLFileContentParser( FileContentElement * parent, const QString & filename, int lineNumber );
	
	void loadFromFile( const QString & filename );
	void loadFromXML( const QDomElement& element );
};

class XSLFileContentImport : public XSLFileContentParser {
	Q_OBJECT
public:
	virtual ~XSLFileContentImport();
	
	static XSLFileContentImport * createImportFromLocation( FileContentElement * parent, const QDomElement & node );
	virtual int rowCount();
protected:
	XSLFileContentImport( FileContentElement * parent, const QString & filename, const QDomElement & node );
private:
	bool m_loadFlag;
};

class XSLFileContentImportJS : public JavaScriptParser {
	Q_OBJECT
public:
	virtual ~XSLFileContentImportJS();
	
	static XSLFileContentImportJS * createImportFromLocation( FileContentElement * parent, const QDomElement & node );
	virtual int rowCount();
protected:
	XSLFileContentImportJS( FileContentElement * parent, const QString & filename, const QDomElement & node );
};

#endif
