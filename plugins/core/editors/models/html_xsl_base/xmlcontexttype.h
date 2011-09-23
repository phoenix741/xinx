/*
 XINX
 Copyright (C) 2007-2010 by Ulrich Van Den Hekke
 xinx@shadoware.org

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful.
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CORE_BALISEDEFINITION_CONTEXT_H
#define CORE_BALISEDEFINITION_CONTEXT_H

#include <codecompletion/context.h>
#include <codecompletion/contexttype.h>
#include <QHash>
#include <QStack>

#define XML_CONTEXT_TYPE "XML"

namespace Core
{

namespace BaliseDefinition
{

class XmlBalise
{
public:
	XmlBalise();
	~XmlBalise();

	const QString & nameSpacePrefix() const;
	void setNameSpacePrefix(const QString & value);

	const QString & baliseName() const;
	void setBaliseName(const QString & value);

	const QHash<QString,QString> & attributes() const;
	void setAttributes(const QHash<QString,QString> & value);

	bool isClosing() const;
	void setIsClosing(bool value);

	bool operator==(const XmlBalise & other) const;
private:
	QString _balise_name, _namespace_prefix;
	QHash< QString, QString > _attributes;
	bool _is_closing;
};

class XmlContextType : public CodeCompletion::ContextType
{
public:
	enum Position
	{
		NONE				= 0,
		COMMENT				= 1,
		BALISE_NAME			= 2,
		ATTRIBUTE_NAME		= 3,
		ATTRIBUTE_CONTENT	= 4
	};

	XmlContextType();
	virtual ~XmlContextType();

	bool isHtml();
	void setIsHtml(bool value);

	bool isXsl();
	void setIsXsl(bool value);

	Position position();
	void setPosition(Position value);

	/* ATTRIBUTE_NAME, ATTRIBUTE_CONTENT */
	const XmlBalise & balise() const;
	void setBalise(const XmlBalise & balise);

	/* ATTRIBUTE_CONTENT */
	const QString & attributeName() const;
	void setAttributeName(const QString & value);

	const QStack<XmlBalise> & xpathLocation();
	void setXpathLocation(const QStack<XmlBalise> & value);

	/* NAMESPACE */
	const QHash<QString,QString> & xmlnsList() const;
	void setXmlnsList(const QHash<QString,QString> & value);

	QString xslNamespace() const;

	virtual QString type() const;
	virtual bool operator==(const ContextType & other) const;
private:
	Position _position;
	XmlBalise _balise;
	QString _attribute_name;
	bool _is_html;
	bool _is_xsl;
	QStack< XmlBalise > _xpath_location;
	QHash<QString,QString> _xmlns_list;
};

}

}

#endif // CORE_BALISEDEFINITION_CONTEXT_H
