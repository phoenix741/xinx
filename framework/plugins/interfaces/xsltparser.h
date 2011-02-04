/*
 XINX
 Copyright (C) 2007-2011 by Ulrich Van Den Hekke
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

#pragma once
#ifndef _XSLTPARSER_INTERFACES_H_
#define _XSLTPARSER_INTERFACES_H_

// Qt Plugin
#include <QtPlugin>

class XsltParser;
class AbstractEditor;

/*!
 * This interface is used to create new XsltParser for XINX if necessary.
 */
class IXinxXsltParser
{
public:
	//! Destroy the plugin
	virtual ~IXinxXsltParser() {}

	/*!
	 * Create a parser of Xslt Stylesheet.
	 * FIXME: Made a parser extendable instead of a replacement.
	 */
	virtual XsltParser * createXsltParser(AbstractEditor * editor) = 0;
};

Q_DECLARE_INTERFACE(IXinxXsltParser, "org.shadoware.xinx.IXinxXsltParser/1.0");

#endif /*_XSLTPARSER_INTERFACES_H_*/
