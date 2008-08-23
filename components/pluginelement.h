/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef _PLUGINELEMENT_H_
#define _PLUGINELEMENT_H_

// Qt header
#include <QPixmap>
#include <QString>

/*!
 * The \e PluginElement is used by the \e PluginSelector to show information in
 * the list.
 */
class PluginElement {
public:
	/*! Initialise the plugin element */
	PluginElement();
	/*! Destroy the plugin element */
	virtual ~PluginElement() {};

	/*! If the plugin element is modifiable a checkbox is showed at the right */
	virtual bool isModifiable() const = 0;
	/*! If the plugin is activated the checkbox is checked */
	bool isActivated() const;

	/*!
	 * Activate the plugin if \e activated is true
	 * \sa setDesactivated()
	 */
	void setActivated( bool activated );
	/*!
	 * Desactivate the plugin if \e desactivated is true.
	 * \sa setActivated()
	 */
	void setDesactivated( bool desactivated );

	/*! If the plugin is configurable a button "Configured ..." is showed. */
	virtual bool isConfigurable() const = 0;

	/*! Return the pixmap of the plugin */
	virtual QPixmap pixmap() const;
	/*! Return the name of the plugin */
	virtual QString name() const = 0;
	/*! Return the description of the plugin */
	virtual QString description() const = 0;
private:
	bool m_isActivated;
};

#endif // _PLUGINELEMENT_H_
