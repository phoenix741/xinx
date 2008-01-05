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

// Xinx header
#include "jscompleter.h"
#include "javascriptmodelcompleter.h"

// Qt header
#include <QCompleter>
#include <QTextEdit>

/* JsCompleter */

JsCompleter::JsCompleter( IXinxExtendedEditor * editor ) : m_editor( editor ) {
	m_modelCompleter = NULL;
	m_completer 	 = NULL;

	m_completer = new QCompleter( this );
	m_completer->setWidget( editor->qTextEdit() );
	m_completer->setCompletionMode( QCompleter::PopupCompletion );
	m_completer->setCaseSensitivity( Qt::CaseInsensitive );
	m_completer->setCompletionRole( Qt::DisplayRole );
	connect( m_completer, SIGNAL(activated(const QModelIndex&)), this, SLOT(insertCompletion(const QModelIndex&)) );

	m_modelCompleter = new JavascriptModelCompleter( editor->modelData(), this );
	m_completer->setModel( m_modelCompleter );
}

JsCompleter::~JsCompleter() {
	delete m_completer;
	delete m_modelCompleter;
}

void JsCompleter::commentSelectedText( IXinxExtendedEditor * editor, bool uncomment ) {
	Q_UNUSED( editor );
	Q_UNUSED( uncomment );
	// TODO: Comment JavaScript
}

bool JsCompleter::keyPressEvent( QKeyEvent *e ) {
	Q_UNUSED( e );
	QString fct = currentFunction();
	if( m_modelCompleter )
		m_modelCompleter->setFilter( fct );
	return false;
}

QCompleter * JsCompleter::currentCompleter( const QTextCursor & cursor ) {
	Q_UNUSED( cursor );
	return m_completer;
}

void JsCompleter::insertCompletion( const QModelIndex& index ) {
	QTextCursor tc = m_editor->qTextEdit()->textCursor();
	QCompleter * c = m_completer;
	
	QString completion = c->completionModel()->data( index ).toString(),
			prefix     = c->completionPrefix();
	
	m_editor->textUnderCursor( tc, true );
	tc.insertText( completion );

	m_editor->qTextEdit()->setTextCursor( tc );
}

QString JsCompleter::currentFunction() {
	QTextCursor tc = m_editor->qTextEdit()->textCursor();
	QString name;
	int bloc = 0;
	bool finded = false;
	// Recherche le debut du document ou le mot function (compte le nombre d'accollade en passant)
	if( ! tc.atStart() )
		while( true ) {
			QTextCursor selectedCursor = tc;
			selectedCursor.select( QTextCursor::WordUnderCursor );
			if( ( selectedCursor.selectedText() == "function" ) && ( name != "{" ) && ( name != "function" ) ) {
				finded = true;
				break;
			} else if( tc.atStart() ) break;
			name = selectedCursor.selectedText();
			if( name == "{" )
				bloc++;
			else if( name == "}" )
				bloc--;
			tc.movePosition( QTextCursor::PreviousWord );
		}

	if( finded && ( bloc > 0 ) )
		return name;
	else
		return QString();
}
