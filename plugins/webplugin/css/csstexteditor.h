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

#ifndef CSSTEXTEDITOR_H_
#define CSSTEXTEDITOR_H_

// Xinx header
#include "xinxcodeedit.h"

class CSSTextEditor : public XinxCodeEdit {
	Q_OBJECT
public:
	CSSTextEditor( QWidget * parent = 0 );
	virtual ~CSSTextEditor();
public slots:
	virtual void commentSelectedText( bool uncomment = false );
protected:
	enum cursorPosition {
		cpEditComment, // in /* ... */
		cpEditTag, // ... { ... }
		cpEditGlobal
	};

	static cursorPosition editPosition( const XinxCodeEdit * textEdit, const QDocumentCursor & cursor );
	cursorPosition editPosition( const QDocumentCursor & cursor );
};

#endif /*CSSTEXTEDITOR_H_*/
