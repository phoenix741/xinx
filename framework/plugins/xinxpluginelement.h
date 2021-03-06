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
#ifndef _XINXPLUGINELEMENT_H_
#define _XINXPLUGINELEMENT_H_

// Xinx header
#include <core/lib-config.h>
#include <pluginelement.h>
#include <plugins/interfaces/plugin.h>

/*!
 * A XinxPluginElement is a plugin (a library, PluginElement can be a script :).
 * It's used with PluginSelector.
 */
class LIBEXPORT XinxPluginElement : public PluginElement
{
public:
	/*!
	 * Create an object based on a \e plugin. If the plugin \e isStatic
	 * \e isModifiable() will return false and \e isActivated() will
	 * return true.
	 */
	explicit XinxPluginElement(QObject * plugin, bool isStatic = false);

	/*! Return true if the plugin is modifiable (no static) */
	virtual bool isModifiable() const;
	/*!
	 * Return always false even if the plugin inherits from IXinxPluginConfiguration.
	 * This hide the configuration button. Now the plugin have it's pages in the
	 * configuration dialog.
	 */
	virtual bool isConfigurable() const;

	virtual QString objectName() const;
	virtual QPixmap pixmap() const;
	virtual QString name() const;
	virtual QString description() const;

	/*! Return the plugin of the element  */
	QObject * plugin() const;
	/*! Return true if the plugin is static */
	bool isStatic() const;
private:
	QObject * m_plugin;
	bool m_isStatic;
};

#endif // _XINXPLUGINELEMENT_H_
