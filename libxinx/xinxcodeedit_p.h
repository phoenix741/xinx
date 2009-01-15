/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@shadoware.org                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
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

#ifndef _XINXCODEEDIT_P_
#define _XINXCODEEDIT_P_

// Xinx header
#include "xinxcodeedit.h"

// QCodeEdit header
#include <qcodeedit.h>


/* XinxEditor */

class XinxEditor : public QEditor {
	Q_OBJECT
public:
	XinxEditor( QWidget *p = 0 ) : QEditor( p ) {};
	XinxEditor( const QString& s, QWidget *p = 0 ) : QEditor( s, p ) { };
	virtual ~XinxEditor() {};

	virtual void setContentModified(bool y) { setContentModified( y ); };
};

#endif /* _XINXCODEEDIT_P_ */