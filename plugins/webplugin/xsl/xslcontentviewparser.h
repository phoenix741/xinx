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

#ifndef _XSLCONTENTVIEWPARSER_H_
#define _XSLCONTENTVIEWPARSER_H_
#pragma once

// Xinx header
#include <contentview/contentviewparser.h>

// Qt header
#include <QApplication>
#include <QXmlStreamReader>
#include <QStack>

class ContentViewNode;
class QTextCodec;

/* XslContentViewParser */

class XslContentViewParser : public ContentViewParser, private QXmlStreamReader {
	Q_DECLARE_TR_FUNCTIONS(XslContentViewParser)
public:
	XslContentViewParser( bool autoDelete = false );
	virtual ~XslContentViewParser();

	QTextCodec * codec() { return m_codec; };
protected:
	virtual void loadFromDeviceImpl();
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

	ContentViewNode * attacheNewTemplateNode( ContentViewNode * parent, const QString & name, const QString & mode, int line );
	ContentViewNode * attacheNewParamsNode( ContentViewNode * parent, const QString & name, const QString & value,  int line );
	ContentViewNode * attacheNewVariableNode( ContentViewNode * parent, const QString & filename, const QString & value, int line );

	QTextCodec * m_codec;
};

/* XmlCompletionParser */

class XmlCompletionParser : public QObject, private ContentViewParser, private QXmlStreamReader {
	Q_OBJECT
public:
	enum XmlCompletionRoleIndex {
		NODE_XML_TYPE      = 34,
		NODE_XML_ISDEFAULT = 35
	};

	XmlCompletionParser();
	virtual ~XmlCompletionParser();

	static XmlCompletionParser * self();

	ContentViewNode * balise( const QString & name ) const;
	QList<ContentViewNode*> balises() const;
	ContentViewNode * baliseAttribute( const QString & name, const QString & attribute ) const;
	ContentViewNode * baliseOfBalise( const QString & name, const QString & baliseName ) const;
	QList<ContentViewNode*> baliseAttributes( const QString & name ) const;
	ContentViewNode * baliseAttributeValue( const QString & name, const QString & attribute, const QString & value ) const;
	QList<ContentViewNode*> baliseAttributeValues( const QString & name, const QString & attribute ) const;

	ContentViewNode * defaultValue( ContentViewNode * node );
	QList<ContentViewNode*> defaultAttributes( ContentViewNode * node );
	QList<ContentViewNode*> defaultBalises( ContentViewNode * node );

	ContentViewNode * rootNode() const;
	QTextCodec * codec() { return m_codec; };
protected:
	virtual void loadFromDeviceImpl();
private:
	void readRootTag();
	void readTypeTag();
	void readBaliseTag();
	void readAttributeTag();
	void readValueTag();
	QString readElementText();

	QStack<ContentViewNode*> m_parentNode;
	QTextCodec * m_codec;
	QString m_type;

	static XmlCompletionParser * s_self;
};

#endif /* _XSLCONTENTVIEWPARSER_H_ */
