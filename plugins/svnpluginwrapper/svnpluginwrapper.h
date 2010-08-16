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

#ifndef SVNPLUGIN_H_
#define SVNPLUGIN_H_
#pragma once

// Xinx header
#include <plugins/plugininterfaces.h>

class SVNPluginWrapper : public QObject, public IRCSPlugin
{
	Q_OBJECT
	Q_INTERFACES(IXinxPlugin)
	Q_INTERFACES(IRCSPlugin)
public:
	SVNPluginWrapper();

	virtual bool initializePlugin(const QString & lang);
	virtual QVariant getPluginAttribute(const enum IXinxPlugin::PluginAttribute & attr);

	virtual QList< QPair<QString,QString> > pluginTools();

	virtual QStringList rcs();
	virtual QString descriptionOfRCS(const QString & rcs);
	virtual RCS * createRCS(const QString & rcs, const QString & basePath);
};

#endif /* SVNPLUGIN_H_*/
