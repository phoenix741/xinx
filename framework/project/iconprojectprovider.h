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

#ifndef __ICONPROJECTPROVIDER_H__
#define __ICONPROJECTPROVIDER_H__
#pragma once

// Xinx header
#include <core/lib-config.h>

// Qt header
#include <QFileIconProvider>
#include <QIcon>
#include <QFileInfo>

/*!
 * Provide special icon for XSL, XML, JS extentions.
 * This Icon provider is used by \e ProjectDirectoryDockWidget to show the tree
 * project with custom icons.
 */
class LIBEXPORT IconProjectProvider : public QFileIconProvider
{
public:
	IconProjectProvider();
	~IconProjectProvider();

	QIcon icon(const QFileInfo & info) const;
};



#endif // __ICONPROJECTPROVIDER_H__
