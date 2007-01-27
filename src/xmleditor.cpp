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

#include <QtGui>
#include <QCompleter>

#include "xmleditor.h"
#include "editorcompletion.h"

/* XMLProcessor */

#define EOW			"~!@#$%^&*()+{}|\"<>?,./;'[]\\="
#define EOWREGEXP	"[~!@#\\$%\\^&\\*\\(\\)\\+\\{\\}|\"<>\\?,\\./;'\\[\\]\\\\=\\s]"

XMLProcessor::XMLProcessor( QTextEdit * widget, QObject * parent ) : TextProcessor( widget, parent ), m_completerParamNodeName( "" ), m_completerValue( 0 ) {
	QStringList wordList;
	for(int i = 0; i < completionNodeList->count(); i++) {
		wordList << completionNodeList->node(i)->name();
	}

	// Completer
	m_completerNode = new QCompleter( wordList, this );
	m_completerNode->setWidget( textEdit() );
	m_completerNode->setCompletionMode( QCompleter::PopupCompletion );
	m_completerNode->setCaseSensitivity( Qt::CaseInsensitive );
	connect( m_completerNode, SIGNAL(activated(const QString&)), this, SLOT(insertCompletion(const QString&)) );
   
	m_completerParam = new QCompleter( this );
	m_completerParam->setWidget( textEdit() );
	m_completerParam->setCompletionMode( QCompleter::PopupCompletion );
	m_completerParam->setCaseSensitivity( Qt::CaseInsensitive );
	connect( m_completerParam, SIGNAL(activated(const QString&)), this, SLOT(insertCompletion(const QString&)) );
}

XMLProcessor::~XMLProcessor() {
	
}
	
bool XMLProcessor::isCodeCommented( const QTextCursor & cursor ) const {
	QTextCursor cursorCommentStart ( textEdit()->document()->find ( "<!--", cursor, QTextDocument::FindBackward ) );
	QTextCursor cursorCommentEnd ( textEdit()->document()->find ( "-->", cursor, QTextDocument::FindBackward ) );
  
	if( cursorCommentStart.isNull() && cursorCommentEnd.isNull() ) return false;
	if( cursorCommentEnd.isNull() ) return true;
	if( cursorCommentStart.isNull() ) return false;
	if( cursorCommentStart < cursorCommentEnd ) return false;
	return true;
}

bool XMLProcessor::isEditBalise( const QTextCursor & cursor ) const {
	QTextCursor cursorBaliseStart ( textEdit()->document()->find ( QRegExp("<(?!\\!\\-\\-)"), cursor, QTextDocument::FindBackward ) );
	QTextCursor cursorBaliseEnd ( textEdit()->document()->find ( ">", cursor, QTextDocument::FindBackward ) );
  
	if( cursorBaliseStart.isNull() && cursorBaliseStart.isNull() ) return false;
	if( cursorBaliseEnd.isNull() ) return true;
	if( cursorBaliseStart.isNull() ) return false;
	if( cursorBaliseStart < cursorBaliseEnd ) return false;
	return true;
}

bool XMLProcessor::isEditNode( const QTextCursor & cursor ) const {
	if( ! isEditBalise(cursor) ) return false;
	
	QTextCursor cursorBaliseStart ( textEdit()->document()->find ( QRegExp("<(?!\\!\\-\\-)"), cursor, QTextDocument::FindBackward ) );
	QTextCursor cursorSpace ( textEdit()->document()->find ( QRegExp("\\s"), cursor, QTextDocument::FindBackward ) );
	
	if( cursorSpace.isNull() ) return true;
	if( cursorSpace < cursorBaliseStart ) return true;
	return false;
}

bool XMLProcessor::isEditParam( const QTextCursor & cursor ) const {
	if( isEditNode( cursor ) || (! isEditBalise( cursor )) ) return false;
	
	QTextCursor cursorSpace ( textEdit()->document()->find ( QRegExp("\\s"), cursor, QTextDocument::FindBackward ) );
	QTextCursor cursorEgal ( textEdit()->document()->find ( "=", cursor, QTextDocument::FindBackward ) );
	
	/* 
		Cas non géré : S'il y a une espace dans une valeur d'un paramètre ....
	*/
	
	if( cursorEgal.isNull() ) return true;
	if( cursorEgal < cursorSpace ) return true;
	return false;
}

bool XMLProcessor::isEditValue( const QTextCursor & cursor ) const {
	return isEditBalise( cursor ) && !isEditNode( cursor ) && !isEditParam( cursor );
}


QString XMLProcessor::textUnderCursor( const QTextCursor & cursor ) const {
	QTextCursor before ( textEdit()->document()->find ( QRegExp( EOWREGEXP ), cursor, QTextDocument::FindBackward ) );
	QTextCursor after ( textEdit()->document()->find ( QRegExp( EOWREGEXP ), cursor ) );

	QTextCursor tc = cursor;
	
	if( ! before.isNull() )
		tc.setPosition( before.position(), QTextCursor::MoveAnchor );
	else
		tc.movePosition( QTextCursor::StartOfBlock, QTextCursor::MoveAnchor ) ;
		
	if( ! after.isNull() )
		tc.setPosition( after.position() - 1, QTextCursor::KeepAnchor ) ;
	else
		tc.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor ) ;
		
	return tc.selectedText().trimmed();
}

QString XMLProcessor::nodeName( const QTextCursor & cursor ) const {
	if( ! isEditBalise( cursor ) ) return QString();
	
	QTextCursor cursorBaliseStart ( textEdit()->document()->find ( QRegExp("<(?!\\!\\-\\-)"), cursor, QTextDocument::FindBackward ) );
	QTextCursor cursorSpace ( textEdit()->document()->find ( QRegExp( EOWREGEXP ), cursorBaliseStart ) );
	
	QTextCursor tc = cursor;
	
	tc.setPosition( cursorBaliseStart.position(), QTextCursor::MoveAnchor ) ;
	if( !cursorSpace.isNull() )
		tc.setPosition( cursorSpace.position() - 1, QTextCursor::KeepAnchor ) ;
	else
		tc.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor ) ;
		
	return tc.selectedText().trimmed();
}

void XMLProcessor::insertCompletion( const QString& completion ) {
	QTextCursor tc = textEdit()->textCursor();
	QCompleter * c = currentCompleter( tc );
	
	int extra = completion.length() - c->completionPrefix().length();
	tc.insertText(completion.right(extra));
	tc.movePosition( QTextCursor::EndOfWord );
	textEdit()->setTextCursor( tc );
}

QCompleter * XMLProcessor::currentCompleter( const QTextCursor & cursor ) {
	if( isEditNode( cursor ) ) {
		return m_completerNode;
	} else if ( isEditParam( cursor ) ) {
		QString node = nodeName(cursor);
		
		if(node != m_completerParamNodeName) {
			m_completerParamNodeName = node;
			if(completionNodeList->node(node)) {
				QStringList wordList;
				for(int i = 0; i < completionNodeList->node(node)->count(); i++) {
					wordList << completionNodeList->node(node)->param(i);
				}
			
				m_completerParam->setModel(new QStringListModel(wordList, m_completerParam));
			} else 
				return NULL;
		}
		
		return m_completerParam;
	} else if ( isEditValue ( cursor ) ) {
		return m_completerValue;
	}
	return NULL;	
}

void XMLProcessor::complete() {
	QTextCursor cursor = textEdit()->textCursor();
	
	QRect cr = textEdit()->cursorRect();
	QString completionPrefix = textUnderCursor(cursor);

	QCompleter * c = currentCompleter(textEdit()->textCursor());

	if(c) {
		if( completionPrefix != m_completerNode->completionPrefix() ) {
		    c->setCompletionPrefix( completionPrefix );
			c->popup()->setCurrentIndex( c->completionModel()->index(0, 0) );
		}
		cr.setWidth( c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width() );
		c->complete( cr );
	}
}

void XMLProcessor::keyPressEvent( QKeyEvent *e ) {
	QCompleter * c = currentCompleter( textEdit()->textCursor() );
	
	if (c && c->popup()->isVisible()) {
		// The following keys are forwarded by the completer to the widget
		switch (e->key()) {
		case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
			e->ignore();
			return; // let the completer do default behavior
		default:
            break;
        }
     }

	bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
	if (!c || !isShortcut) // dont process the shortcut when we have a completer
		parentKeyPressEvent( e );
		
	if(!e->text().isEmpty()) {
		if(e->text().right(1) == ">") {
			QTextCursor tc(textEdit()->textCursor());
			tc.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor );
			tc.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor );
		
			QString selected = tc.selectedText();

			tc.movePosition( QTextCursor::NextCharacter );
	
			if( isEditBalise( tc ) && selected != "/>" ){
				QString name = nodeName( tc );
				if( ! name.isEmpty() ) {
					int position = textEdit()->textCursor().position();
	         	
					textEdit()->textCursor().insertText( QString("</%1>").arg(name) );
		
					tc.setPosition(position);
					textEdit()->setTextCursor( tc );
				}
			}
		} else if(e->text().right(1) == "=") {
			QTextCursor tc(textEdit()->textCursor());
			if( isEditValue( tc ) ) {
				tc.insertText( "\"\"" );
				tc.movePosition( QTextCursor::PreviousCharacter );
				textEdit()->setTextCursor( tc );
			}
		}
	}

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

	if (!c || (ctrlOrShift && e->text().isEmpty()))
		return;

     static QString eow(EOW); // end of word
     bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
     QString completionPrefix = textUnderCursor( textEdit()->textCursor() );

     if (!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 2 || eow.contains(e->text().right(1)))) {
         c->popup()->hide();
         return;
     }

     if (completionPrefix != c->completionPrefix()) {
         c->setCompletionPrefix(completionPrefix);
         c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
     }

     QRect cr = textEdit()->cursorRect();
     cr.setWidth(c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width());
     c->complete(cr); // popup it up!
}

void XMLProcessor::commentSelectedText( bool uncomment ) {
	QTextCursor cursor( textEdit()->textCursor() );
 
	QTextCursor cursorStart( textEdit()->textCursor() );
	cursorStart.setPosition( cursor.selectionStart() );
	bool isStartCommented = isCodeCommented( cursorStart );

	QTextCursor cursorEnd( textEdit()->textCursor() );
	cursorEnd.setPosition( cursor.selectionEnd() );
	bool isEndCommented =  isCodeCommented( cursorEnd );

	QString text = cursor.selectedText();
	text = text.replace( "<!--", "" );
	text = text.replace( "-->", "" );
  
	cursor.beginEditBlock();

	cursor.removeSelectedText();
	if(! ( isStartCommented ^ uncomment ) ) {
		// Comment  	
		if(! uncomment)
			cursor.insertText("<!--");  
		else
			cursor.insertText("-->");  
	}
	cursor.insertText(text);  
	if(! ( isEndCommented ^ uncomment )) {
		// End the comment  	
		if(! uncomment)
			cursor.insertText("-->");  
		else
			cursor.insertText("<!--");  
	}

	cursor.endEditBlock();
}

#include "xmleditor.moc"
