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
 
#include "texteditor.h"
#include "editorcompletion.h"
#include "xmlhighlighter.h"

#include <QtGui>
#include <QCompleter>


#define EOW			"~!@#$%^&*()+{}|\"<>?,./;'[]\\="
#define EOWREGEXP	"[~!@#\\$%\\^&\\*\\(\\)\\+\\{\\}|\"<>\\?,\\./;'\\[\\]\\\\=\\s]"

StudioTextEdit::StudioTextEdit(QWidget *parent) : QTextEdit(parent), m_completerNode(0), m_completerParamNodeName(""), m_completerParam(0), m_completerValue(0) {
	// Setup the main view
	QFont font;
	font.setFamily("Monospace");
	font.setFixedPitch(true);
	font.setPointSize(10);
  
	setFont(font);
	setTabStopWidth( 15 );
	new XmlHighlighter(document());

	QStringList wordList;
	for(int i = 0; i < completionNodeList->count(); i++) {
		wordList << completionNodeList->node(i)->name();
	}

	// Completer
	m_completerNode = new QCompleter(wordList, this);
	m_completerNode->setWidget(this);
	m_completerNode->setCompletionMode(QCompleter::PopupCompletion);
	m_completerNode->setCaseSensitivity(Qt::CaseInsensitive);
	connect(m_completerNode, SIGNAL(activated(const QString&)), this, SLOT(insertCompletion(const QString&)));
   
	m_completerParam = new QCompleter(this);
	m_completerParam->setWidget(this);
	m_completerParam->setCompletionMode(QCompleter::PopupCompletion);
	m_completerParam->setCaseSensitivity(Qt::CaseInsensitive);
	connect(m_completerParam, SIGNAL(activated(const QString&)), this, SLOT(insertCompletion(const QString&)));

}

StudioTextEdit::~StudioTextEdit() {
	
}
	
QString StudioTextEdit::text() const {
	return toPlainText ();	
}

void StudioTextEdit::setText( const QString &text ) {
	setPlainText(text);
}

bool StudioTextEdit::isCodeCommented(const QTextCursor & cursor) const {
  QTextCursor cursorCommentStart ( document()->find ( "<!--", cursor, QTextDocument::FindBackward ) );
  QTextCursor cursorCommentEnd ( document()->find ( "-->", cursor, QTextDocument::FindBackward ) );
  
  if( cursorCommentStart.isNull() && cursorCommentEnd.isNull() ) return false;
  if( cursorCommentEnd.isNull() ) return true;
  if( cursorCommentStart.isNull() ) return false;
  if( cursorCommentStart < cursorCommentEnd ) return false;
  return true;
}

bool StudioTextEdit::isEditBalise(const QTextCursor & cursor) const {
  QTextCursor cursorBaliseStart ( document()->find ( QRegExp("<(?!\\!\\-\\-)"), cursor, QTextDocument::FindBackward ) );
  QTextCursor cursorBaliseEnd ( document()->find ( ">", cursor, QTextDocument::FindBackward ) );
  
  if( cursorBaliseStart.isNull() && cursorBaliseStart.isNull() ) return false;
  if( cursorBaliseEnd.isNull() ) return true;
  if( cursorBaliseStart.isNull() ) return false;
  if( cursorBaliseStart < cursorBaliseEnd ) return false;
  return true;
}

bool StudioTextEdit::isEditNode(const QTextCursor & cursor) const {
	if(! isEditBalise(cursor)) return false;
	
	QTextCursor cursorBaliseStart ( document()->find ( QRegExp("<(?!\\!\\-\\-)"), cursor, QTextDocument::FindBackward ) );
	QTextCursor cursorSpace ( document()->find ( QRegExp("\\s"), cursor, QTextDocument::FindBackward ) );
	
	if(cursorSpace.isNull()) return true;
	if(cursorSpace < cursorBaliseStart) return true;
	return false;
}

bool StudioTextEdit::isEditParam(const QTextCursor & cursor) const {
	if(isEditNode(cursor) || (! isEditBalise(cursor))) return false;
	
	QTextCursor cursorSpace ( document()->find ( QRegExp("\\s"), cursor, QTextDocument::FindBackward ) );
	QTextCursor cursorEgal ( document()->find ( "=", cursor, QTextDocument::FindBackward ) );
	
	/* 
	Cas non géré : S'il y a une espace dans une valeur d'un paramêtre ....
	*/
	
	if(cursorEgal.isNull()) return true;
	if(cursorEgal < cursorSpace) return true;
	return false;
}

bool StudioTextEdit::isEditValue(const QTextCursor & cursor) const {
	return isEditBalise(cursor) && !isEditNode(cursor) && !isEditParam(cursor);
}


QString StudioTextEdit::textUnderCursor(const QTextCursor & cursor) const {
	QTextCursor before ( document()->find ( QRegExp( EOWREGEXP ), cursor, QTextDocument::FindBackward ) );
	QTextCursor after ( document()->find ( QRegExp( EOWREGEXP ), cursor ) );

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

QString StudioTextEdit::nodeName(const QTextCursor & cursor) const {
	if(! isEditBalise(cursor)) return QString();
	
	QTextCursor cursorBaliseStart ( document()->find ( QRegExp("<(?!\\!\\-\\-)"), cursor, QTextDocument::FindBackward ) );
	QTextCursor cursorSpace ( document()->find ( QRegExp( EOWREGEXP ), cursorBaliseStart ) );
	
	QTextCursor tc = cursor;
	
	tc.setPosition( cursorBaliseStart.position(), QTextCursor::MoveAnchor ) ;
	if( !cursorSpace.isNull() )
		tc.setPosition( cursorSpace.position() - 1, QTextCursor::KeepAnchor ) ;
	else
		tc.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor ) ;
		
	return tc.selectedText().trimmed();
}

void StudioTextEdit::insertCompletion(const QString& completion) {
	QTextCursor tc = textCursor();
	QCompleter * c = currentCompleter(tc);
	
	int extra = completion.length() - c->completionPrefix().length();
	tc.insertText(completion.right(extra));
	tc.movePosition(QTextCursor::EndOfWord);
	setTextCursor(tc);
}

QCompleter * StudioTextEdit::currentCompleter(const QTextCursor & cursor) {
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

void StudioTextEdit::complete() {
	QTextCursor cursor = textCursor();
	
	QRect cr = cursorRect();
	QString completionPrefix = textUnderCursor(cursor);

	QCompleter * c = currentCompleter(textCursor());

	if(c) {
		if( completionPrefix != m_completerNode->completionPrefix() ) {
		    c->setCompletionPrefix( completionPrefix );
			c->popup()->setCurrentIndex( c->completionModel()->index(0, 0) );
		}
		cr.setWidth( c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width() );
		c->complete( cr );
	}
}

void StudioTextEdit::keyPressEvent(QKeyEvent *e) {
	QCompleter * c = currentCompleter(textCursor());
	
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
		QTextEdit::keyPressEvent(e);

	if(!e->text().isEmpty()) {
		if(e->text().right(1) == ">") {
			QTextCursor tc(textCursor());
			tc.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor );
			tc.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor );
		
			QString selected = tc.selectedText();

			tc.movePosition( QTextCursor::NextCharacter );
	
			if( isEditBalise( tc ) && selected != "/>" ){
				QString name = nodeName( tc );
				int position = textCursor().position();
	         	
				textCursor().insertText( QString("</%1>").arg(name) );
		
				tc.setPosition(position);
				setTextCursor( tc );
			}
		} else if(e->text().right(1) == "=") {
			QTextCursor tc(textCursor());
			if( isEditValue( tc ) ) {
				tc.insertText( "\"\"" );
				tc.movePosition( QTextCursor::PreviousCharacter );
				setTextCursor( tc );
			}
		}
	}

	const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
	if (!c || (ctrlOrShift && e->text().isEmpty()))
		return;

     static QString eow(EOW); // end of word
     bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
     QString completionPrefix = textUnderCursor( textCursor() );

     if (!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 2 || eow.contains(e->text().right(1)))) {
         c->popup()->hide();
         return;
     }

     if (completionPrefix != c->completionPrefix()) {
         c->setCompletionPrefix(completionPrefix);
         c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
     }

     QRect cr = cursorRect();
     cr.setWidth(c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width());
     c->complete(cr); // popup it up!
}
