/***************************************************************************
 *   Copyright (C) 2006 by David Cuadrado                                *
 *   krawek@gmail.com                                                      *
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

#ifndef DMAINWINDOWABSTRACTSETTINGS_H
#define DMAINWINDOWABSTRACTSETTINGS_H

#include <QObject>
#include <dideality.h>

class DToolView;
class DMainWindow;

/**
 * @author David Cuadrado <krawek@gmail.com>
*/

class D_IDEAL_EXPORT DMainWindowAbstractSettings : public QObject
{
	public:
		DMainWindowAbstractSettings(QObject *parent = 0) : QObject(parent) {}
		~DMainWindowAbstractSettings() {};
		
		virtual void save(DMainWindow *w) = 0;
		virtual void restore(DMainWindow *w) = 0;
		
};

#endif
