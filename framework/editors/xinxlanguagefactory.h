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
#ifndef _XINXLANGUAGEFACTORY_H_
#define _XINXLANGUAGEFACTORY_H_

// Xinx header
#include <core/lib-config.h>

// QCodeEdit header
#include <qlanguagefactory.h>

class XINXConfig;
class XinxFormatScheme;

/*!
 * This class used to create language definition for text editor in XINX.
 */
class LIBEXPORT XinxLanguageFactory : public QLanguageFactory
{
	Q_OBJECT
public:
	//! Destroy the instance
	virtual ~XinxLanguageFactory();
private:
	//! Create a new instance of XinxLanguageFactory
	XinxLanguageFactory(XinxFormatScheme *fmt, XINXConfig *p = 0);
	friend class XINXConfig;
};

#endif /* _XINXLANGUAGEFACTORY_H_ */
