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

#ifndef XMLTEXTEDITOR_H_
#define XMLTEXTEDITOR_H_

// Xinx header
#include "texteditor.h"

// Qt header
#include <QStringList>

class XmlTextEditor : public TextEditor {
	Q_OBJECT
public:
	XmlTextEditor( QWidget * parent = 0 );
	virtual ~XmlTextEditor();

	virtual QCompleter * completer();
	
	QStringList paramOfNode( const QTextCursor & cursor );
public slots:
	virtual void commentSelectedText( bool uncomment = false );
protected slots:
	virtual void insertCompletion( const QModelIndex& index );
protected:
	virtual void localKeyPressExecute( QKeyEvent * e );
	virtual bool processKeyPress( QKeyEvent * );

private:
	void key_shenter( bool back );

	enum cursorPosition {
		cpEditComment, // <!-- XXXXX  -->
		cpEditNodeName, // <XXXXX>
		cpEditParamName, // <..... XXXXX=".." XXXX=.. XXXX/>
		cpEditParamValue, // <..... ....=XXXXX ....="XXXXX XXXXX=XXXX"
		cpNone
	};

	static cursorPosition editPosition( const QTextEdit * textEdit, const QTextCursor & cursor, QString & nodeName, QString & paramName );
	cursorPosition editPosition( const QTextCursor & cursor );

	void insertCompletionValue( QTextCursor & tc, QString node, QString param );
	int insertCompletionParam( QTextCursor & tc, QString node, bool movePosition = true );
	int insertCompletionBalises( QTextCursor & tc, QString node );
	void insertCompletionAccolade( QTextCursor & tc, QString node, QString param, QString value, QString type );

	QString m_nodeName, m_paramName;
};

#endif /*XMLTEXTEDITOR_H_*/
