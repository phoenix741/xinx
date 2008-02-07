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

#ifndef IEXTENDEDEDITOR_H_
#define IEXTENDEDEDITOR_H_

// Xinx header
#include <filecontentstructure.h>

class QTextEdit;
class QObject;
class QTextCursor;

class IXinxExtendedEditor {
public:
	virtual ~IXinxExtendedEditor() {}; 
	
	virtual FileContentElement * importModelData( FileContentElement * parent, QString & filename, int line ) = 0;
	virtual FileContentElement * modelData() const = 0; 

	virtual int level() const = 0;
	virtual QString textUnderCursor( const QTextCursor & cursor, bool deleteWord = false ) = 0;
	
	virtual QTextEdit * qTextEdit() const = 0;
	virtual QObject * object() const = 0;
	virtual void setObject( QObject * object ) = 0;
};

#endif /*IEXTENDEDEDITOR_H_*/
