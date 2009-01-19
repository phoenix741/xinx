/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
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

#ifndef JSTEXTEDITOR_H_
#define JSTEXTEDITOR_H_

// Xinx header
#include "xinxcodeedit.h"

class JSTextEditor : public XinxCodeEdit {
	Q_OBJECT
public:
	JSTextEditor( QWidget * parent = 0 );
	virtual ~JSTextEditor();

	virtual QCompleter * completer();
public slots:
	virtual void commentSelectedText( bool uncomment = false );
protected:
	enum cursorPosition {
		cpEditLongComment, // in /* ... */
			cpEditSimpleComment, // in // ...
			cpEditFunction, // in function ....() { .... }
			cpEditParams, // in function .... ( .... )
			cpEditGlobal
	};

	static cursorPosition editPosition( const XinxCodeEdit * textEdit, const QDocumentCursor & cursor, QString & functionName );
	cursorPosition editPosition( const QDocumentCursor & cursor );

	QString m_functionName;
};

#endif /*JSTEXTEDITOR_H_*/
