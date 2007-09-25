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

// Xinx header
#include "exceptions.h"

// Qt header
#include <QDebug>

/* XinxException */

XinxException::XinxException( QString message ) {
	m_message = message;
	qDebug() << message << endl;
}

const QString & XinxException::getMessage() const {
	return m_message;
}

/* XinxAssertException */

XinxAssertException::XinxAssertException( const char *assertion, const char *file, int line ) : XinxException( QString( "ASSERT: \"%1\" in file %2, line %3" ).arg( assertion ).arg( file ).arg( line ) ) {
	
}

