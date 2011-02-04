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
#ifndef _PLUGIN_INTERFACES_H_
#define _PLUGIN_INTERFACES_H_

// Qt header
#include <QtPlugin>
#include <QPair>

/*!
 * This intereface is used to create a plugin used by XINX.
 * At start, XINX load all plugins and call the initializePlugin method from the plugin.
 *
 * The plugin must define some attribute used to show informations to user (in about box and
 * in the list of Plugin).
 *
 * A plugin can return a list of tools (with a default value). This tool will be show in the
 * customize dialog of XINX.
 */
class IXinxPlugin
{
public:
	/*! Information that can be asked to plugin */
	enum PluginAttribute
	{
		PLG_NAME        = 1001, //!< Name of the plugin
		PLG_DESCRIPTION = 1002, //!< Long description of the plugin
		PLG_ICON        = 1003, //!< An icon that represent the plugin
		PLG_AUTHOR      = 1004, //!< The author (or authors) of the plugin
		PLG_EMAIL       = 1005, //!< Email to contact the author (or team)
		PLG_WEBSITE     = 1006, //!< The site where we can find the plugin
		PLG_VERSION     = 1007, //!< The Version of the plugin
		PLG_LICENCE     = 1008  //!< The Licence of the plugin
	};

	//! Destroy the plugin
	virtual ~IXinxPlugin() {}

	/*!
	 * Called when the plugin is loaded.
	 * \param lang The lang in which the plugin must be load.
	 */
	virtual bool initializePlugin(const QString & lang) = 0;
	/*! Get an information from the attribute. List of informations can be found in the \e PluginAttribute enum. */
	virtual QVariant getPluginAttribute(const enum IXinxPlugin::PluginAttribute & attr) = 0;

	/*! List of tools with the default value where find the tool */
	//! FIXME: This must be used in another interface and should be pure virtual.
	virtual QList< QPair<QString,QString> > pluginTools()
	{
		return QList< QPair<QString,QString> >();
	}

	//! Return a list of action (order in menu) used for dynamic action
	//! FIXME: This must be used in another interface and this must be return action.
	virtual void generateActionMenu() {}
};

Q_DECLARE_INTERFACE(IXinxPlugin, "org.shadoware.xinx.IXinxPlugin/1.0");


#endif /* _PLUGIN_INTERFACES_H_ */
