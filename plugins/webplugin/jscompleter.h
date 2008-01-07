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

#ifndef JSCOMPLETER_H_
#define JSCOMPLETER_H_

// Xinx header
#include "iextendededitor.h"

// Qt header
#include <QTextCursor>
#include <QString>
#include <QTextCursor>
#include <QModelIndex>

class QCompleter;
class QKeyEvent;
class JavascriptModelCompleter;

class JsCompleter : public QObject {
	Q_OBJECT
public:
	JsCompleter( IXinxExtendedEditor * editor );
	~JsCompleter();
	
	static void commentSelectedText( IXinxExtendedEditor * editor, bool uncomment );

	bool keyPressEvent( QKeyEvent *e );
	QCompleter * currentCompleter( const QTextCursor & cursor );
private slots:
	void insertCompletion( const QModelIndex& index );
private:
	QString currentFunction();

	JavascriptModelCompleter * m_modelCompleter;
	QCompleter * m_completer;
	
	IXinxExtendedEditor * m_editor;
};


#endif /*JSCOMPLETER_H_*/
