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

#ifndef _XINXPLUGINELEMENT_H_
#define _XINXPLUGINELEMENT_H_

// Xinx header
#include <pluginelement.h>
#include "plugininterfaces.h"

/*!
 * A XinxPluginElement is a plugin (a library, PluginElement can be a script :).
 * It's used with PluginSelector.
 */
class XinxPluginElement : public PluginElement {
public:
	/*!
	 * Create an object based on a \e plugin. If the plugin \e isStatic
	 * \e isModifiable() will return false and \e isActivated() will
	 * return true.
	 */
	XinxPluginElement( QObject * plugin, bool isStatic = false );

	/*! Return true if the plugin is modifiable (no static) */
	virtual bool isModifiable() const;
	/*!
	 * Return true if the plugin is configurable. A plugin is configurable
	 * if \e plugin() is \e IXinxPluginConfiguration.
	 */
	virtual bool isConfigurable() const;

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
