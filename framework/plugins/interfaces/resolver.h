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
#ifndef _RESOLVER_INTERFACES_H_
#define _RESOLVER_INTERFACES_H_

// Xinx header
#include <project/xinxprojectproject.h>

// Qt Plugin
#include <QtPlugin>

/*!
 * Define a filename resolver  object used to find the import, and find file in parsing of stylesheet
 */
class IFileResolverPlugin
{
public:
	//! Destroy the interface. Used to hide warning when using the interface
	virtual ~IFileResolverPlugin() {}

	//! Return the name of the resolver
	virtual QString name() = 0;
	//! Resolver id
	virtual QString id() = 0;
	//! Resolve the file \e nameToResolve. The \e currentPath, can be used to find the file relatively of the current open editor.
	virtual bool resolveFileName(const QString & nameToResolve, QString & resolvedName, const QString & currentPath = QString(), XinxProject::ProjectPtr project = XinxProject::ProjectPtr()) = 0;
};

/*!
 * Return the list of resolver than can be used in the application.
 * Actually, only resolver of imported file exists.
 */
class IResolverPlugin
{
public:
	//! Destroy the interface. Used to hide warning when using the interface
	virtual ~IResolverPlugin() {}

	//! Return the list of resolver used to find import
	virtual QList<IFileResolverPlugin*> fileResolvers() = 0;
};

Q_DECLARE_INTERFACE(IResolverPlugin, "org.shadoware.xinx.IResolverPlugin/1.0");

#endif /*_RESOLVER_INTERFACES_H_*/
