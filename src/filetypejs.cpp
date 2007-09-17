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
#include "texteditor.h"
#include "filetypejs.h"
#include "private/p_filetypejs.h"
#include "globals.h"
#include "xinxconfig.h"

// Qt header
#include <QRect>
#include <QAbstractItemView>
#include <QScrollBar>

// Define
#define EOW "~!@$#%^&*()+{}|\"<>?,/;'[]\\="

/* PrivateFileTypeJs */

PrivateFileTypeJs::PrivateFileTypeJs( FileTypeJs * parent ) {
	m_parent    = parent;
	m_parser    = NULL;
	m_model     = NULL;
	m_modelCompleter = NULL;
	m_completer = NULL;
}

PrivateFileTypeJs::~PrivateFileTypeJs() {
	delete m_completer;
	delete m_modelCompleter;
	delete m_model;
	delete m_parser;
}

void PrivateFileTypeJs::insertCompletion( const QModelIndex& index ) {
	QTextCursor tc = m_parent->textEdit()->textCursor();
	QCompleter * c = m_completer;
	
	QString completion = c->completionModel()->data( index ).toString(),
			prefix     = c->completionPrefix();
	
	tc.movePosition( QTextCursor::EndOfWord );
	for( int i = 0 ; i < prefix.length() ; i++ ) tc.deletePreviousChar();
	tc.insertText( completion );

	m_parent->textEdit()->setTextCursor( tc );
}

QString PrivateFileTypeJs::currentFunction() {
	QTextCursor tc = m_parent->textEdit()->textCursor();
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
//		tc.movePosition( QTextCursor::PreviousCharacter );
		tc.movePosition( QTextCursor::PreviousWord );
	}
	if( finded && ( bloc >= 0 ) )
		return name;
	else
		return QString();
}

void PrivateFileTypeJs::keyPressEvent( QKeyEvent * e ) {
	if( ( global.m_config->config().editor.completionLevel == 0 ) || (! m_modelCompleter) ) {
		m_parent->textEdit()->keyPressExecute( e );
		return;
	}

	QCompleter * c = m_completer;

	if ( c && c->popup()->isVisible() ) {
		// The following keys are forwarded by the completer to the widget
		switch (e->key()) {
		case Qt::Key_Enter:
		case Qt::Key_Return:
		case Qt::Key_Escape:
		case Qt::Key_Tab:
		case Qt::Key_Backtab:
			e->ignore();
			m_parent->textEdit()->keyPressSkip( e );
			return; // let the completer do default behavior
		default:
			break;
		}
	}

	bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
	if( !c || !isShortcut )
		m_parent->textEdit()->keyPressExecute( e );
	else
		m_parent->textEdit()->keyPressSkip( e );
	
	const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);

	if (!c || (ctrlOrShift && e->text().isEmpty()))
		return;

	static QString eow(EOW); // end of word
	bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
	QString completionPrefix = m_parent->textEdit()->textUnderCursor( m_parent->textEdit()->textCursor() );

	if (!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 2 || eow.contains(e->text().right(1)))) {
		c->popup()->hide();
		return;
	}

	if ( completionPrefix != c->completionPrefix() ) {
		QString fct = currentFunction();
			if( m_modelCompleter )
			m_modelCompleter->setFilter( fct );
		c->setCompletionPrefix( completionPrefix );
		c->popup()->setCurrentIndex( c->completionModel()->index(0, 0) );
	}

	QRect cr = m_parent->textEdit()->cursorRect();
	cr.setWidth(c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width());
	c->complete(cr); // popup it up!
}

/* FileTypeJs */

FileTypeJs::FileTypeJs( TextEditor * parent ) : FileTypeInterface( parent ) {
	d = new PrivateFileTypeJs( this );

	d->m_completer = new QCompleter( this );
	d->m_completer->setWidget( parent );
	d->m_completer->setCompletionMode( QCompleter::PopupCompletion );
	d->m_completer->setCaseSensitivity( Qt::CaseInsensitive );
	d->m_completer->setCompletionRole( Qt::DisplayRole );
	connect( d->m_completer, SIGNAL(activated(const QModelIndex&)), d, SLOT(insertCompletion(const QModelIndex&)) );
	connect( parent, SIGNAL(execKeyPressEvent(QKeyEvent*)), d, SLOT(keyPressEvent(QKeyEvent*)) );
}

FileTypeJs::~FileTypeJs() {
	delete d;
}

void FileTypeJs::commentSelectedText( bool uncomment ) {
	Q_UNUSED( uncomment );
	// TO COMPLETE
}

void FileTypeJs::updateModel() {
	JavaScriptParser * parser = NULL;
	try {
		parser = new JavaScriptParser( textEdit()->toPlainText() );
		emit modelUpdated( NULL );
		d->m_completer->setModel( NULL );
		delete d->m_model; d->m_model = NULL;
		delete d->m_modelCompleter; d->m_modelCompleter = NULL; 
		delete d->m_parser; d->m_parser = NULL;
		d->m_parser = parser;
		d->m_model  = new JavascriptFileContent( d->m_parser, this );
		d->m_modelCompleter = new JavascriptModelCompleter( d->m_parser, this );
		emit modelUpdated( d->m_model );
		d->m_completer->setModel( d->m_modelCompleter );
	} catch( JavaScriptParserException e ) {
		delete parser;
		throw e;
	}
}

QAbstractItemModel * FileTypeJs::model() {
	return d->m_model;
}

void FileTypeJs::complete() {
	if( ! d->m_modelCompleter ) return ;
		
	QTextCursor cursor = textEdit()->textCursor();
	
	QRect cr = textEdit()->cursorRect();
	QString completionPrefix = textEdit()->textUnderCursor(cursor);

	QCompleter * c = d->m_completer;

	if( c ) {
		if( completionPrefix != c->completionPrefix() ) {
		    c->setCompletionPrefix( completionPrefix );
			c->popup()->setCurrentIndex( c->completionModel()->index(0, 0) );
		}
		cr.setWidth( c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width() );
		c->complete( cr );
	}
}
