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

#ifndef ABSTRACTMANAGER_H
#define ABSTRACTMANAGER_H

#include <QString>
#include <QObject>

class DTabbedMainWindow;

/**
 * @author David Cuadrado <krawek@gmail.com>
*/

class AbstractManager : public QObject
{
	public:
		AbstractManager(QObject *parent = 0) : QObject(parent), m_initialized(false) {};
		virtual ~AbstractManager() {};
		
		void initialize(DTabbedMainWindow *w )
		{
			if ( !m_initialized)
			{
				m_initialized = true;
				setupWindow(w);
			}
		};
		virtual void openFile(const QString &file) = 0;
		virtual void newFile() = 0;
		
		virtual QString fileFilter() const { return QString(); };
		
	protected:
		virtual void setupWindow(DTabbedMainWindow *w) = 0;
		
		
	private:
		bool m_initialized;
};

#endif
