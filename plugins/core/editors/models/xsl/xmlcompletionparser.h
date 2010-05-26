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

#ifndef _XMLCOMPLETIONPARSER_H_
#define _XMLCOMPLETIONPARSER_H_
#pragma once

// Xinx header
#include <contentview2/contentview2parser.h>

// Qt header
#include <QApplication>
#include <QXmlStreamReader>
#include <QStack>

/* XmlCompletionParser */

class XmlCompletionParser : public QObject, public ContentView2::Parser, private QXmlStreamReader
{
	Q_OBJECT
public:
	enum XmlCompletionRoleIndex
	{
		NODE_XML_TYPE       = ContentView2::Node::NODE_USER_VALUE,
		NODE_XML_ISDEFAULT  = ContentView2::Node::NODE_USER_VALUE + 1,
		NODE_XML_SORT_INDEX = ContentView2::Node::NODE_USER_VALUE + 2,
		NODE_XML_LAST       = ContentView2::Node::NODE_USER_VALUE + 3  //!< Used for derivation
	};

	~XmlCompletionParser();

	virtual void load();

	static XmlCompletionParser * self();

	ContentView2::Node          balise(const QString & name) const;
	QList<ContentView2::Node>   balises() const;
	ContentView2::Node          baliseAttribute(const QString & name, const QString & attribute) const;
	ContentView2::Node          baliseOfBalise(const QString & name, const QString & baliseName) const;
	QList<ContentView2::Node>   baliseAttributes(const QString & name) const;
	ContentView2::Node          baliseAttributeValue(const QString & name, const QString & attribute, const QString & value) const;
	QList<ContentView2::Node>   baliseAttributeValues(const QString & name, const QString & attribute) const;

	ContentView2::Node          defaultValue(ContentView2::Node node) const;
	QList<ContentView2::Node>   defaultAttributes(ContentView2::Node node) const;
	QList<ContentView2::Node>   defaultBalises(ContentView2::Node node) const;

	XmlCompletionParser * clone();
private:
	XmlCompletionParser();
	QSqlDatabase db() const;

	void readRootTag();
	void readTypeTag();
	void readBaliseTag();
	void readAttributeTag();
	void readValueTag();
	QString readElementText();

	QStack<ContentView2::Node> m_parentNode;
	QString m_type;

	struct InternalAttribute
	{
		QHash<QString,uint> values;
	};
	struct InternalBalise
	{
		QHash<QString,uint> attributes;
		QHash<QString,uint> balises;
	};

	static QHash<QString,uint> m_balisesName;
	static QHash<uint,InternalBalise> m_balisesId;
	static QHash<uint,InternalAttribute> m_attributesId;

	unsigned long m_index;
	static XmlCompletionParser * s_self;
};

#endif /* _XMLCOMPLETIONPARSER_H_ */
