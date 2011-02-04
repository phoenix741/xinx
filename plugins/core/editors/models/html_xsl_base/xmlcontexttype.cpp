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

#include "xmlcontexttype.h"

namespace Core
{
	
namespace BaliseDefinition
{
	
/* XmlBalise */

void XmlBalise::setBaliseName(const QString& value)
{
	_balise_name = value;
}

const QString& XmlBalise::baliseName() const
{
	return _balise_name;
}

const QString & XmlBalise::nameSpacePrefix() const
{
	return _namespace_prefix;
}

void XmlBalise::setNameSpacePrefix(const QString & value)
{
	_namespace_prefix = value;
}

void XmlBalise::setAttributes(const QHash< QString, QString >& value)
{
	_attributes = value;
}

const QHash< QString, QString >& XmlBalise::attributes() const
{
	return _attributes;
}

bool XmlBalise::operator==(const XmlBalise & other) const
{
	return (_balise_name == other._balise_name) && (_attributes == other._attributes);
}

/* XmlContextType */

XmlContextType::XmlContextType()
{

}

XmlContextType::~XmlContextType()
{

}

QString XmlContextType::type() const
{
	return XML_CONTEXT_TYPE;
}

void XmlContextType::setIsHtml(bool value)
{
	_is_html = value;
}

bool XmlContextType::isHtml()
{
	return _is_html;
}

void XmlContextType::setIsXsl(bool value)
{
	_is_xsl = value;
}

bool XmlContextType::isXsl()
{
	return _is_xsl;
}

void XmlContextType::setPosition(XmlContextType::Position value)
{
	_position = value;
}

XmlContextType::Position XmlContextType::position()
{
	return _position;
}

void XmlContextType::setBalise(const Core::BaliseDefinition::XmlBalise& balise)
{
	_balise = balise;
}

const Core::BaliseDefinition::XmlBalise& XmlContextType::balise() const
{
	return _balise;
}

void XmlContextType::setAttributeName(const QString& value)
{
	_attribute_name = value;
}

const QString& XmlContextType::attributeName() const
{
	return _attribute_name;
}

void XmlContextType::setXpathLocation(const QStack< XmlBalise >& value)
{
	_xpath_location = value;
}

const QStack< XmlBalise >& XmlContextType::xpathLocation()
{
	return _xpath_location;
}

const QHash<QString,QString> & XmlContextType::xmlnsList() const
{
	return _xmlns_list;
}

void XmlContextType::setXmlnsList(const QHash<QString,QString> & value)
{
	_xmlns_list = value;
}

QString XmlContextType::xslNamespace() const
{
	return _xmlns_list.key ("http://www.w3.org/1999/XSL/Transform");
}

bool XmlContextType::operator==(const CodeCompletion::ContextType& other) const
{
    if (CodeCompletion::ContextType::operator==(other))
	{
		const XmlContextType & t = dynamic_cast<const XmlContextType &>(other);
		return (t._is_html == _is_html) && (t._is_xsl == _is_xsl) && (t._attribute_name == _attribute_name) && (t._balise == _balise) && (t._position == _position) && (t._xpath_location == _xpath_location) && (t._xmlns_list == _xmlns_list);
	}
	else
	{
		return false;
	}
}

}

}

