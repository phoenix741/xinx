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

class PluginElement {
public:
	virtual ~PluginElement() {};

	virtual bool isModifiable() const = 0;
	bool isActivated() const;

	void setActivated( bool activated );
	void setDesactivated( bool desactivated );

	virtual bool isConfigurable() const = 0;

	virtual QPixmap pixmap() const;
	virtual QString name() const = 0;
	virtual QString description() const = 0;
private:
	bool m_isActivated;
};

#endif // _PLUGINELEMENT_H_
