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
#include "xmltexteditor.h"
#include "../editorcompletion.h"
#include "xslmodelcompleter.h"

#include <xinxpluginsloader.h>
#include <xinxconfig.h>

// Qt header
#include <QKeyEvent>
#include <QTextBlock>
#include <QCompleter>

// Define
#define EOWREGEXP	"[~!@\\$#%\\^&\\*\\(\\)\\+\\{\\}|\"<>,/;'\\[\\]\\\\=\\s]"
#define isEditBalise(value) ((value == cpEditNodeName) || (value == cpEditParamName) || (value == cpEditParamValue))


XmlTextEditor::XmlTextEditor( QWidget * parent ) : TextEditor( parent ) {
	SyntaxHighlighter * highlighter = XinxPluginsLoader::self()->createHighlighter( "XML", document() );
	setHighlighter( highlighter );
}

XmlTextEditor::~XmlTextEditor() {

}

QCompleter * XmlTextEditor::completer() {
	if( TextEditor::completer() ) {
		XSLCompletionModel * c = qobject_cast<XSLCompletionModel*>( TextEditor::completer()->model() );
		if( c ) {
			cursorPosition pos = editPosition( textCursor() );
			if( pos == cpEditParamName ) {
				c->setFilter( m_nodeName );
			} else if( pos == cpEditNodeName ) {
				c->setFilter();
			} else if( pos == cpEditParamValue ) {
				c->setFilter( m_nodeName, m_paramName );
		}

		return TextEditor::completer();
		}
	}
	return TextEditor::completer();
}

void XmlTextEditor::commentSelectedText( bool uncomment ) {
	QString nodeName, paramName;

	QTextCursor cursor( textCursor() );

	QTextCursor cursorStart( textCursor() );
	cursorStart.setPosition( cursor.selectionStart() );
	bool isStartCommented = editPosition( this, cursorStart, nodeName, paramName ) == cpEditComment;

	QTextCursor cursorEnd( textCursor() );
	cursorEnd.setPosition( cursor.selectionEnd() );
	bool isEndCommented =  editPosition( this, cursorEnd, nodeName, paramName ) == cpEditComment;

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

XmlTextEditor::cursorPosition XmlTextEditor::editPosition( const QTextEdit * qTextEdit, const QTextCursor & cursor, QString & nodeName, QString & paramName ) {
	Q_ASSERT( ! cursor.isNull() );

	cursorPosition cPosition = cpNone;
	nodeName = QString();
	paramName = QString();

	/* is a Comment ? */
	QTextCursor cursorCommentStart ( qTextEdit->document()->find ( "<!--", cursor, QTextDocument::FindBackward ) );
	QTextCursor cursorCommentEnd ( qTextEdit->document()->find ( "-->", cursor, QTextDocument::FindBackward ) );

	bool inComment = ! (
			cursorCommentStart.isNull() // No comment before where i am
		||	( (! cursorCommentEnd.isNull()) && ( cursorCommentStart < cursorCommentEnd ) ) // There is a end before, and the last is the end balise
	 );

	if( inComment ) {
		cPosition = cpEditComment;
		return cPosition;
	} // else

	/* or simple text */
	QTextCursor cursorBaliseStart ( qTextEdit->document()->find ( QRegExp("<(?!\\!\\-\\-)"), cursor, QTextDocument::FindBackward ) ); // A balise is'nt a comment
	QTextCursor cursorBaliseEnd ( qTextEdit->document()->find ( ">", cursor, QTextDocument::FindBackward ) );

	bool inNode = (! cursorBaliseStart.isNull()) && ( cursorBaliseEnd.isNull() || ( cursorBaliseEnd < cursorBaliseStart ) );
	if( ! inNode ) {
		return cPosition; // = cpNone;
	} // else

	/* Retrieve the name of node */ /* Space ? or other ? */
	QTextCursor cursorSpaceAfterNodeName ( qTextEdit->document()->find ( QRegExp( EOWREGEXP ), cursorBaliseStart ) );
	QTextCursor tc = cursor;

	tc.setPosition( cursorBaliseStart.position(), QTextCursor::MoveAnchor ) ;
	if( ! cursorSpaceAfterNodeName.isNull() )
		tc.setPosition( cursorSpaceAfterNodeName.position() - 1, QTextCursor::KeepAnchor ) ;
	else
		tc.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor ) ;

	nodeName = tc.selectedText().trimmed();

	/* i'm editing a balise name ? */
	QTextCursor cursorSpace ( qTextEdit->document()->find ( QRegExp("\\s"), cursor, QTextDocument::FindBackward ) );
	if( cursorSpace < cursorBaliseStart )
		cursorSpace = QTextCursor();

	bool editBaliseName = cursorSpace.isNull();
	if( editBaliseName ) {
		cPosition = cpEditNodeName;
		return cPosition;
	} // else

	QTextCursor cursorEgal ( qTextEdit->document()->find ( "=", cursor, QTextDocument::FindBackward ) );
	QTextCursor cursorQuote (qTextEdit->document()->find ( "\"", cursor, QTextDocument::FindBackward ) );
	if( cursorEgal < cursorBaliseStart )
		cursorEgal = QTextCursor();
	if( cursorQuote < cursorBaliseStart )
		cursorQuote = QTextCursor();

	int quoteCount = 0;
	if( ! cursorQuote.isNull() ) {
		quoteCount ++;
		QTextCursor cursorPreviousQuote( cursorQuote );

		cursorPreviousQuote.movePosition( QTextCursor::PreviousCharacter );
		cursorPreviousQuote = qTextEdit->document()->find ( "\"", cursorPreviousQuote, QTextDocument::FindBackward );
		while( ( ! cursorPreviousQuote.isNull() ) &&  ( cursorBaliseStart < cursorPreviousQuote ) ) {
			quoteCount ++;

			cursorPreviousQuote.movePosition( QTextCursor::PreviousCharacter );
			cursorPreviousQuote = qTextEdit->document()->find ( "\"", cursorPreviousQuote, QTextDocument::FindBackward );
		}
	}

	/*
		<Noeud Param1=Value1 Param2="Value 2" Param3="Value3" Param4 Param5="Value 5"/>
		       1   5    10   15   20   25   30   35   40   45   50   55   60   65   70

		1 => Param / Space not null, Egal null, Quote null / 0
		8 => Value / Space < Egal, Quote null / 0
		15 => Param / Egal < Space, Quote null / 0
		22 => Value / Space < Egal, Quote null / 1
		23 => Value / Space < Egal < Quote / 1
		29 => Value / Egal < Quote < Space / 1
		31 => Param / Egal < Space  < Quote	 / 2
		35 => Param / Egal < Quote < Space / 2

		Compter le nombre de guillement
	*/

	QTextCursor cursorParam ( qTextEdit->document()->find ( QRegExp( "[=\\s]" ), cursorSpace ) );
	tc.setPosition( cursorSpace.position(), QTextCursor::MoveAnchor ) ;
	if( ! cursorParam.isNull() )
		tc.setPosition( cursorParam.position() - 1, QTextCursor::KeepAnchor ) ;
	else
		tc.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor ) ;

	paramName = tc.selectedText().trimmed();

	cPosition = cpEditParamName;
	if( cursorEgal.isNull() || ( ( cursorEgal < cursorQuote ) && ( cursorEgal < cursorSpace ) && ( ( quoteCount % 2 ) == 0 ) ) )
		return cPosition;

	if( ! nodeName.isEmpty() )
		cPosition = cpEditParamValue;
	return cPosition;
}

XmlTextEditor::cursorPosition XmlTextEditor::editPosition( const QTextCursor & cursor ) {
	return editPosition( this, cursor, m_nodeName, m_paramName );
}

void XmlTextEditor::insertCompletion( const QModelIndex& index ) {
	TextEditor::insertCompletion( index );

	QCompleter * c = completer();
	QString completion = c->completionModel()->data( index ).toString();

	QTextCursor tc = textCursor();
	cursorPosition pos = editPosition( tc );
	if( pos == cpEditParamName ) {
		insertCompletionValue( tc, m_nodeName, completion );
	} else
		if( pos == cpEditNodeName ) {
			if( ! m_nodeName.isEmpty() )
				insertCompletionParam( tc, completion );
			else
				tc.insertText( ">" );
		} else
			if( pos == cpEditParamValue ) {
				QString type = c->completionModel()->data( index, Qt::UserRole ).toString();
				insertCompletionAccolade( tc, m_nodeName, m_paramName, completion, type );
			}

	setTextCursor( tc );
}

void XmlTextEditor::insertCompletionValue( QTextCursor & tc, QString node, QString param ) {
	if( xmlCompletionContents && xmlCompletionContents->balise( node ) && xmlCompletionContents->balise( node )->attribute( param ) && xmlCompletionContents->balise( node )->attribute( param )->defaultValue() >= 0 ) {
		int defaultValue = xmlCompletionContents->balise( node )->attribute( param )->defaultValue();
		tc.insertText( QString("=\"%1\"").arg( xmlCompletionContents->balise( node )->attribute( param )->values().at( defaultValue ) ) );
	} else {
		tc.insertText("=\"\"");
		tc.movePosition( QTextCursor::PreviousCharacter );
	}
}

int XmlTextEditor::insertCompletionParam( QTextCursor & tc, QString node, bool movePosition ) {
	int position = -1;
	if( xmlCompletionContents && xmlCompletionContents->balise( node ) ) {
		CompletionXMLBalise* balise = xmlCompletionContents->balise( node );
		foreach( CompletionXMLAttribute* attr, balise->attributes() ) {
			if( attr->isDefault() ) {
				int defaultValue = attr->defaultValue();
				QString defaultValueString;
				if( defaultValue >= 0 )
					defaultValueString = attr->values().at( defaultValue );
				tc.insertText( QString(" %1=\"%2\"").arg( attr->name() ).arg( defaultValueString ) );
				if( ( position == -1 ) && defaultValueString.isEmpty() ) {
					tc.movePosition( QTextCursor::PreviousCharacter );
					position = tc.position();
					tc.movePosition( QTextCursor::NextCharacter );
				}
			}
		}
	}
	if( ( position != -1 ) && ( movePosition ) )
		tc.setPosition( position );
	return position;
}

int XmlTextEditor::insertCompletionBalises( QTextCursor & tc, QString node ) {
	QString indent = tc.block().text();
	indent = indent.left( indent.indexOf( QRegExp( "\\S" ) ) );

	int position = -1, cnt = 0;

	if( XINXConfig::self()->config().editor.completionLevel == 3 && xmlCompletionContents && xmlCompletionContents->balise( node ) ) {
		CompletionXMLBalise* balise = xmlCompletionContents->balise( node );
		foreach( CompletionXMLBalise* b, balise->balises() ) {
			if( b->isDefault() ) {
				tc.insertText( "\n" + indent + "\t" );
				tc.insertText( "<" + b->name() );
				int insertPosition = insertCompletionParam( tc, b->name(), false );
				if( position == -1 ) position = insertPosition;
				tc.insertText( ">" );
				insertCompletionBalises( tc, b->name() );
				cnt++;
			}
		}
	}

	if( XINXConfig::self()->config().editor.completionLevel >= 2 ) {
		if( cnt > 0 )
			tc.insertText( "\n" + indent );
		tc.insertText( QString("</%1>").arg( node ) );
		if( position != -1 )
			tc.setPosition( position );
	}

	return position;
}

void XmlTextEditor::insertCompletionAccolade( QTextCursor & tc, QString node, QString param, QString value, QString type ) {
	Q_UNUSED( param );

	QTextCursor tc2( tc );
	tc2.movePosition( QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor, value.length() );

	bool insertDollard = true, insertAccolade = true;
	QTextCursor tc3( tc2 );
	tc3.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor );
	if( tc3.selectedText() == "$" ) {
		tc2.movePosition( QTextCursor::PreviousCharacter );
		insertDollard = false;
	}

	QTextCursor accOpen, accClose, debParam;
	accOpen = document()->find( "{", tc2, QTextDocument::FindBackward );
	accClose = document()->find( "}", tc2, QTextDocument::FindBackward );
	debParam = document()->find( "\"", tc2, QTextDocument::FindBackward );
	if( ( accOpen > accClose ) && ( accOpen > debParam ) ) insertAccolade = false;

	if( xmlCompletionContents && xmlCompletionContents->balise( node ) ) {
		CompletionXMLBalise* balise = xmlCompletionContents->balise( node );
		if( insertAccolade && (balise->category() != "stylesheet") && ( ( type == "XSLFileContentParams" ) || ( type == "XSLFileContentVariable" ) || ( type == "XSLFileContentTemplate" ) )) {
			if( insertDollard && ( ( type == "XSLFileContentParams" ) || ( type == "XSLFileContentVariable" ) ) )
				tc2.insertText( "{$" );
			else
				tc2.insertText( "{" );
			tc.insertText( "}" );
		} else
		if( insertDollard && ( ( type == "XSLFileContentParams" ) || ( type == "XSLFileContentVariable" ) ) ) {
			tc2.insertText( "$" );
		}
	}
}

bool XmlTextEditor::processKeyPress( QKeyEvent * e ) {
	if( !e->text().isEmpty() ) {
		if( ( e->text().right(1) == ">" ) && ( XINXConfig::self()->config().editor.completionLevel >= 2 ) ) {
			QTextCursor tc( textCursor() );
			tc.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor );
			tc.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor );

			QString selected = tc.selectedText();

			tc.movePosition( QTextCursor::NextCharacter );

			if( isEditBalise( editPosition( tc ) ) && selected != "/>" ){
				QString name = m_nodeName;
				if( ( ! name.isEmpty() ) && ( name.at(0) != '/' ) ) {
					tc.movePosition( QTextCursor::NextCharacter );
					int position = textCursor().position();
					int newPosition = insertCompletionBalises( tc, name );
					if( newPosition == -1 )
						tc.setPosition(position);
					setTextCursor( tc );
				}
			}
			return true;
		} else if( e->text().right(1) == "=" && XINXConfig::self()->config().editor.completionLevel >= 2 ) {
			QTextCursor tc( textCursor() );
			QTextCursor tc2( textCursor() );
			tc2.movePosition( QTextCursor::PreviousCharacter );
			if( editPosition( tc2 ) == cpEditParamName ) {
				tc.deletePreviousChar();
				insertCompletionValue( tc, m_nodeName, m_paramName );
				setTextCursor( tc );
			}
			return true;
		} else if( e->text().right(1) == " " && XINXConfig::self()->config().editor.completionLevel >= 3 ) {
			QTextCursor tc( textCursor() );
			QTextCursor tc2( textCursor() );
			tc2.movePosition( QTextCursor::PreviousCharacter );
			if( editPosition( tc2 ) == cpEditNodeName ) {
				tc.deletePreviousChar();
				if( insertCompletionParam( tc, m_nodeName ) == -1 )
					tc.insertText( " " );
				setTextCursor( tc );
			}
			return true;
		}
	}
	return false;
}

