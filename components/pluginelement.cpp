/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2007-2010 by Ulrich Van Den Hekke                         *
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
 * \ingroup Components
 * \class PluginElement
 * \since 0.7.2.0
 *
 * \brief The PluginElement class provides an item to use with the PluginSelector view class.
 *
 * The PluginElement is used by the PluginSelector to show information in the list. This class has
 * \e abstract method. You must redefine this method in your PluginElement's class.
 */

//! Initialise the plugin element
PluginElement::PluginElement() : m_isActivated(true)
{

}

//! Destroy the plugin element
PluginElement::~PluginElement()
{
}


/*!
 * \fn virtual bool PluginElement::isModifiable() const = 0
 * \brief If the plugin element is modifiable a checkbox is shown at the right
 */

/*!
 * \fn virtual bool PluginElement::isConfigurable() const = 0;
 * \brief If the plugin is configurable a button "Configured ..." is showed.
 */

/*!
 * \fn virtual QString PluginElement::name() const = 0;
 * \brief Returns the name of the plugin
 */

/*!
 * \fn virtual QString PluginElement::description() const = 0;
 * \brief Returns the description of the plugin
 */

/*!
 * \brief If the plugin is activated the checkbox is checked.
 * \sa setActivated(), setDesactivated()
 */
bool PluginElement::isActivated() const
{
	return m_isActivated || (! isModifiable());
}

/*!
 * \brief Activates the plugin if \p activated is true
 * \sa setDesactivated(), isActivated()
 */
void PluginElement::setActivated(bool activated)
{
	if (m_isActivated != activated)
	{
		m_isActivated = activated;
	}
}

/*!
 * \brief Desactivate the plugin if \p desactivated is true.
 * \sa setActivated(), isActivated()
 */
void PluginElement::setDesactivated(bool desactivated)
{
	setActivated(! desactivated);
}

//! Return the pixmap of the plugin
QPixmap PluginElement::pixmap() const
{
	return QPixmap(":/images/unknown.png");
}
