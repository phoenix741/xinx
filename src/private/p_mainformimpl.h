/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
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

#ifndef __P_MAINFORMIMPL_H__
#define __P_MAINFORMIMPL_H__

// Xinx header
#include "../mainformimpl.h"

// Qt header
#include <QObject>

#define MAXRECENTFILES 10

class PrivateMainformImpl : public QObject {
	Q_OBJECT
public:
	PrivateMainformImpl( MainformImpl * parent );
	~PrivateMainformImpl();
	
	void updateShortcut();
	void createSubMenu();
	void createStatusBar();
	
	// Label text
	QLabel * m_editorPosition;

	// Recent action
	QAction * m_recentProjectActs[ MAXRECENTFILES ]; 
	QAction * m_recentSeparator;
	QAction * m_recentFileActs[ MAXRECENTFILES ]; 
	QAction * m_recentFileSeparator;

	void setupRecentMenu( QMenu * menu, QAction * & seperator, QAction * recentActions[ MAXRECENTFILES ] );
private:	
	MainformImpl * m_parent;
};

#endif // __P_MAINFORMIMPL_H__
