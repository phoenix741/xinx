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

// Qt header
#include <QXmlStreamReader>
#include <QMultiHash>

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
	XSLFileContentParams( FileContentElement * parent, const QString & name, const QString & value, int line );
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
	XSLFileContentVariable( FileContentElement * parent, const QString & name, const QString & value, int line );
	XSLFileContentVariable( FileContentElement * parent, const QDomElement & node );
	virtual ~XSLFileContentVariable();

	virtual QIcon icon() const;
};

class XSLFileContentTemplate : public FileContentElement {
	Q_OBJECT
public:
	XSLFileContentTemplate( FileContentElement * parent, const QString & name, const QString & mode, int line );
	XSLFileContentTemplate( FileContentElement * parent, const QDomElement & node, const QString & name );
	virtual ~XSLFileContentTemplate();

	const QString & mode() const;

	virtual QString displayName() const;

	virtual bool equals( FileContentElement * element );
	virtual void copyFrom( FileContentElement * element );

	virtual QIcon icon() const;
private:
	QString m_mode;
	IXinxExtendedEditor * m_editor;

	friend class XSLFileContentParser;
};

class XSLFileContentParser : public FileContentElement, public FileContentParser {
	Q_OBJECT
public:
	XSLFileContentParser( FileContentElement * parent, const QString & filename, int lineNumber );
	XSLFileContentParser();
	virtual ~XSLFileContentParser();

	virtual void loadFromDevice( QIODevice * device );
	virtual void loadFromContent( const QString & content );
	virtual void loadFromFile( const QString & filename );
	virtual void loadFromFileDelayed( const QString & filename );
	virtual bool isLoaded();

	virtual int rowCount();
	virtual QIcon icon() const;

	QTextCodec * codec() { return m_codec; };
protected:
private:
	bool m_isLoaded;
	QTextCodec * m_codec;

	class Parser : public QXmlStreamReader {
	public:
		Parser( XSLFileContentParser * parent ) : m_parent( parent ) {};

		bool loadFromDevice( QIODevice * device );
	private:
		struct struct_xsl_variable {
			bool isParam;
			int line;
			QString name;
			QString value;
		};
		struct struct_script {
			bool isSrc;
			int line;
			QString content;
			QString src;
			QString title;
		};

		void readStyleSheet();
		void readUnknownElement();
		void readVariable();
		void readTemplate( QList<struct_xsl_variable> & t, QList<struct_script> & s );
		void readTemplate();
		QString readElementText();

		XSLFileContentParser * m_parent;
	};
};

class XslContentElementList : public FileContentElementList {
	Q_OBJECT
public:
	XslContentElementList( FileContentElement * root );
	virtual ~XslContentElementList();

	QStringList modes( QString templateName ) const;
	QStringList params( QString templateName ) const;
private slots:
	void slotAboutToAdd( int row );
	void slotAdded();
	void slotAboutToRemove( int row );
	void slotReset();
private:
	void addElementList( XSLFileContentTemplate * templ );
	
	QMultiHash<QString,QString> m_modes, m_params;
	int m_line;
};

#endif
