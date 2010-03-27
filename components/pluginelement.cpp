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

// Xinx header
#include "pluginelement.h"

/* PluginElement */

/*!
 * \class PluginElement
 * The PluginElement is used by the PluginSelector to show information in the list.
 */

/*! Initialise the plugin element */
PluginElement::PluginElement() : m_isActivated(true)
{

}

/*! Destroy the plugin element */
PluginElement::~PluginElement() {
}


/*!
 * \fn virtual bool PluginElement::isModifiable() const = 0
 * If the plugin element is modifiable a checkbox is showed at the right
 */

/*!
 * \fn virtual bool PluginElement::isConfigurable() const = 0;
 * If the plugin is configurable a button "Configured ..." is showed.
 */

/*!
 * \fn virtual QString PluginElement::name() const = 0;
 * Return the name of the plugin
 */

/*!
 * \fn virtual QString PluginElement::description() const = 0;
 * Return the description of the plugin
 */

/*! If the plugin is activated the checkbox is checked */
bool PluginElement::isActivated() const
{
	return m_isActivated || (! isModifiable());
}

/*!
 * Activate the plugin if \e activated is true
 * \sa setDesactivated()
 */
void PluginElement::setActivated(bool activated)
{
	if (m_isActivated != activated)
	{
		m_isActivated = activated;
	}
}

/*!
 * Desactivate the plugin if \e desactivated is true.
 * \sa setActivated()
 */
void PluginElement::setDesactivated(bool desactivated)
{
	setActivated(! desactivated);
}

/*! Return the pixmap of the plugin */
QPixmap PluginElement::pixmap() const
{
	return QPixmap(":/images/unknown.png");
}
