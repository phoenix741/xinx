/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
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

#include "jseditor.h"

#include <QCompleter>
#include <QDomElement>
#include <QKeyEvent>
#include <QTextEdit>
#include <QTextBlock>

/* JSProcessor */

JSProcessor::JSProcessor( QTextEdit * widget, XSLProject * project, QObject * parent ) : TextProcessor( widget, project, parent ) {

}

JSProcessor::~JSProcessor() {
	
}

void JSProcessor::complete() {
}

void JSProcessor::keyPressEvent( QKeyEvent *e ) {
	bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
	if (!isShortcut) // dont process the shortcut when we have a completer
		parentKeyPressEvent( e );

	const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);

	if( ( ! ctrlOrShift ) && ( ( e->key() == Qt::Key_Enter ) || ( e->key() == Qt::Key_Return ) ) ) {
		QTextCursor tc( textEdit()->textCursor() );
		QTextBlock previous = tc.block().previous();	
		QString indent = previous.text(); 
		
		indent = indent.left( indent.indexOf( QRegExp( "\\S" ) ) );
		
		tc.movePosition( QTextCursor::StartOfLine );
		
		tc.insertText( indent );
		
		textEdit()->setTextCursor( tc );
		
		return;
	}
}

void JSProcessor::commentSelectedText( bool uncomment ) {
}

void JSProcessor::updateModel() {
	
}

QAbstractItemModel * JSProcessor::model() {
	return NULL;
}
