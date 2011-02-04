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
#ifndef _VERSIONCONTROL_INTERFACES_H_
#define _VERSIONCONTROL_INTERFACES_H_

// Qt Plugin
#include <QtPlugin>

class RCS;

/*!
 * This plugins is used to add a new revision control system to XINX. For this purpose,
 * the plugin give a list of managed rcs and a description (show in the project page).
 *
 * When XINX must create a RCS object, he call the IRCSPlugin::createRCS() method with
 * the parameter used in the constructor, and the revision control system to create (one from
 * the rcs() method).
 * The method will create a derivated object of RCS with the required implementation for the
 * revision control system.
 */
class IRCSPlugin
{
public:
	/// Destroy the plugin
	virtual ~IRCSPlugin() {}

	/// List of revision control system proposed by the plugin
	virtual QStringList rcs() = 0;
	/// Description of each revision control system
	virtual QString descriptionOfRCS(const QString & rcs) = 0;
	/*!
	 * \brief Create a revision control system for the revision control system \e rcs
	 * and the path \e basePath.
	 */
	virtual RCS * createRCS(const QString & rcs, const QString & basePath = QString()) = 0;
};

Q_DECLARE_INTERFACE(IRCSPlugin, "org.shadoware.xinx.IRCSPlugin/1.0");

#endif /*_VERSIONCONTROL_INTERFACES_H_*/
