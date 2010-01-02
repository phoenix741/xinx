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

#ifndef SERVICESPLUGIN_H_
#define SERVICESPLUGIN_H_
#pragma once

// Xinx header
#include <plugins/plugininterfaces.h>
#include <actions/actioninterface.h>

class ServicesPlugin : public QObject, public IFilePlugin, public IXinxPluginProjectConfiguration {
	Q_OBJECT
	Q_INTERFACES(IXinxPlugin)
	Q_INTERFACES(IFilePlugin)
	Q_INTERFACES(IXinxPluginProjectConfiguration)
public:
	ServicesPlugin();
	virtual ~ServicesPlugin();

	virtual bool initializePlugin( const QString & lang );
	virtual QVariant getPluginAttribute( const enum IXinxPlugin::PluginAttribute & attr );

	virtual QList<IFileTypePlugin*> fileTypes();

	virtual XinxAction::MenuList actions();

	virtual QList<IXinxPluginProjectConfigurationPage*> createProjectSettingsPage( QWidget * parent );

	virtual QList<IXinxPluginNewProjectConfigurationPage*> createNewProjectSettingsPages();
private:
	QList<IFileTypePlugin*> m_fileTypes;
	XinxAction::MenuList m_menus;
};

#endif /* SERVICESPLUGIN_H_*/
