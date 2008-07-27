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
#include "../config/selfwebpluginsettings.h"

#include <xinxpluginsloader.h>
#include <xinxconfig.h>

// Qt header
#include <QKeyEvent>
#include <QTextBlock>
#include <QCompleter>
#include <QTextDocument>
#include <QStack>

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
	if( ! SelfWebPluginSettings::self()->config().xml.activeCompletion ) return 0;

	return TextEditor::completer();
}

void XmlTextEditor::commentSelectedText( bool uncomment ) {
	QString nodeName, paramName;

	QTextCursor cursor( textCursor() );

	QTextCursor cursorStart( textCursor() );
	cursorStart.setPosition( cursor.selectionStart() );
	bool isStartCommented = editPosition( cursorStart, nodeName, paramName ) == cpEditComment;

	QTextCursor cursorEnd( textCursor() );
	cursorEnd.setPosition( cursor.selectionEnd() );
	bool isEndCommented =  editPosition( cursorEnd, nodeName, paramName ) == cpEditComment;

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

XmlTextEditor::cursorPosition XmlTextEditor::editPosition( const QTextCursor & cursor, QString & nodeName, QString & paramName ) {
	Q_ASSERT( ! cursor.isNull() );

	cursorPosition cPosition = cpNone;
	nodeName = QString();
	paramName = QString();

	/* is a Comment ? */
	QTextCursor cursorCommentStart ( document()->find ( "<!--", cursor, QTextDocument::FindBackward ) );
	QTextCursor cursorCommentEnd ( document()->find ( "-->", cursor, QTextDocument::FindBackward ) );

	bool inComment = ! (
			cursorCommentStart.isNull() // No comment before where i am
		||	( (! cursorCommentEnd.isNull()) && ( cursorCommentStart < cursorCommentEnd ) ) // There is a end before, and the last is the end balise
	 );

	if( inComment ) {
		cPosition = cpEditComment;
		return cPosition;
	} // else

	/* or simple text */
	QTextCursor cursorBaliseStart ( document()->find ( QRegExp("<(?!\\!\\-\\-)"), cursor, QTextDocument::FindBackward ) ); // A balise is'nt a comment
	QTextCursor cursorBaliseEnd ( document()->find ( ">", cursor, QTextDocument::FindBackward ) );

	bool inNode = (! cursorBaliseStart.isNull()) && ( cursorBaliseEnd.isNull() || ( cursorBaliseEnd < cursorBaliseStart ) );
	if( ! inNode ) {
		return cPosition; // = cpNone;
	} // else

	/* Retrieve the name of node */ /* Space ? or other ? */
	QTextCursor cursorSpaceAfterNodeName ( document()->find ( QRegExp( EOWREGEXP ), cursorBaliseStart ) );
	QTextCursor tc = cursor;

	tc.setPosition( cursorBaliseStart.position(), QTextCursor::MoveAnchor ) ;
	if( ! cursorSpaceAfterNodeName.isNull() )
		tc.setPosition( cursorSpaceAfterNodeName.position() - 1, QTextCursor::KeepAnchor ) ;
	else
		tc.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor ) ;

	nodeName = tc.selectedText().trimmed();

	/* i'm editing a balise name ? */
	QTextCursor cursorSpace ( document()->find ( QRegExp("\\s"), cursor, QTextDocument::FindBackward ) );
	if( cursorSpace < cursorBaliseStart )
		cursorSpace = QTextCursor();

	bool editBaliseName = cursorSpace.isNull();
	if( editBaliseName ) {
		cPosition = cpEditNodeName;
		return cPosition;
	} // else

	QTextCursor cursorEgal ( document()->find ( "=", cursor, QTextDocument::FindBackward ) );
	QTextCursor cursorQuote (document()->find ( "\"", cursor, QTextDocument::FindBackward ) );
	if( cursorEgal < cursorBaliseStart )
		cursorEgal = QTextCursor();
	if( cursorQuote < cursorBaliseStart )
		cursorQuote = QTextCursor();

	int quoteCount = 0;
	if( ! cursorQuote.isNull() ) {
		quoteCount ++;
		QTextCursor cursorPreviousQuote( cursorQuote );

		cursorPreviousQuote.movePosition( QTextCursor::PreviousCharacter );
		cursorPreviousQuote = document()->find ( "\"", cursorPreviousQuote, QTextDocument::FindBackward );
		while( ( ! cursorPreviousQuote.isNull() ) &&  ( cursorBaliseStart < cursorPreviousQuote ) ) {
			quoteCount ++;

			cursorPreviousQuote.movePosition( QTextCursor::PreviousCharacter );
			cursorPreviousQuote = document()->find ( "\"", cursorPreviousQuote, QTextDocument::FindBackward );
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

	QTextCursor cursorParam ( document()->find ( QRegExp( "[=\\s]" ), cursorSpace ) );
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
	return editPosition( cursor, m_nodeName, m_paramName );
}


QStringList XmlTextEditor::paramOfNode( const QTextCursor & cursor ) {
	XmlTextEditor::cursorPosition position = editPosition( cursor );
	if( ( position == cpEditComment ) || ( position == cpNone ) ) return QStringList();

	QTextCursor baliseStart = document()->find( "<", cursor, QTextDocument::FindBackward );
	QTextCursor baliseStop = document()->find( ">", cursor );
	if( baliseStart.isNull() || baliseStop.isNull() ) return QStringList();

	QStringList result;

	QTextCursor c = cursor;
	do {
		c = document()->find( QRegExp("\\W\\w+=\""), c, QTextDocument::FindBackward );
		if( c.isNull() || ( baliseStart >= c ) ) break;
		QTextCursor text( document() );
		text.setPosition( c.selectionStart() + 1 );
		text.setPosition( c.selectionEnd() - 2, QTextCursor::KeepAnchor );
		result << text.selectedText();
	} while( baliseStart < c );
	c = cursor;
	do {
		c = document()->find( QRegExp("\\w+=\""), c );
		if( c.isNull() || ( c >= baliseStop ) ) break;
		c.setPosition( c.selectionEnd() - 2, QTextCursor::KeepAnchor );
		result << c.selectedText();
	} while( c < baliseStop );

	return result;
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
	if( xmlCompletionContents && xmlCompletionContents->balise( node ) && xmlCompletionContents->balise( node )->attribute( param ) && SelfWebPluginSettings::self()->config().xml.addDefaultAttribute ) {
		int defaultValue = xmlCompletionContents->balise( node )->attribute( param )->defaultValue();
		if( defaultValue >= 0 ) {
			tc.insertText( QString("=\"%1\"").arg( xmlCompletionContents->balise( node )->attribute( param )->values().at( defaultValue ) ) );
			return;
		}
	}
	tc.insertText("=\"\"");
	tc.movePosition( QTextCursor::PreviousCharacter );
}

int XmlTextEditor::insertCompletionParam( QTextCursor & tc, QString node, bool movePosition ) {
	int position = -1;
	if( SelfWebPluginSettings::self()->config().xml.addDefaultAttribute && xmlCompletionContents && xmlCompletionContents->balise( node ) ) {
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

	if( SelfWebPluginSettings::self()->config().xml.addDefaultSubBalise && xmlCompletionContents && xmlCompletionContents->balise( node ) ) {
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

	if( SelfWebPluginSettings::self()->config().xml.addClosedBalise ) {
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

void XmlTextEditor::localKeyPressExecute( QKeyEvent * e ) {
	if ( ( e->key() == Qt::Key_Return ) && ( ( e->modifiers() == Qt::ControlModifier ) || ( e->modifiers() == (Qt::ShiftModifier | Qt::ControlModifier) ) ) ) {
		key_shenter( e->modifiers() & Qt::ShiftModifier );
		e->accept();
	} else
		TextEditor::localKeyPressExecute( e );
}

void XmlTextEditor::key_shenter( bool back ) {
	QTextDocument::FindFlags flags = 0;
	if( back ) flags |= QTextDocument::FindBackward;

	QTextCursor cursor = textCursor();
	if( back ) cursor.setPosition( cursor.selectionStart() - 2, QTextCursor::MoveAnchor );
	cursor = document()->find( QRegExp("=\"[^\"]*\""), cursor, flags );

	if( ! cursor.isNull() ) {
		QTextCursor newCursor = textCursor();
		newCursor.setPosition( cursor.selectionStart() + 2, QTextCursor::MoveAnchor );
		newCursor.setPosition( cursor.selectionEnd() - 1, QTextCursor::KeepAnchor );
		setTextCursor( newCursor );
	}
}

bool XmlTextEditor::processKeyPress( QKeyEvent * e ) {
	if( ! SelfWebPluginSettings::self()->config().xml.activeCompletion ) return false;

	if( !e->text().isEmpty() ) {
		if( ( e->text().right(1) == ">" ) && ( SelfWebPluginSettings::self()->config().xml.addClosedBalise ) ) {
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
		} else if( e->text().right(1) == "=" && SelfWebPluginSettings::self()->config().xml.addDefaultAttribute ) {
			QTextCursor tc( textCursor() );
			QTextCursor tc2( textCursor() );
			tc2.movePosition( QTextCursor::PreviousCharacter );
			if( editPosition( tc2 ) == cpEditParamName ) {
				tc.deletePreviousChar();
				insertCompletionValue( tc, m_nodeName, m_paramName );
				setTextCursor( tc );
			}
			return true;
		} else if( e->text().right(1) == " " && SelfWebPluginSettings::self()->config().xml.addDefaultAttribute ) {
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

QList<XPathBalise> XmlTextEditor::xpath( const QTextCursor & cursor, const QStringList & includeOnly, const QString & prefix, const QStringList & attributeName ) {
	QList<XPathBalise> xpath;
	QStack<QString> baliseClose;

	QTextCursor c = cursor;
	do {
		c = document()->find( QRegExp( "<[^<>]*>" ), c, QTextDocument::FindBackward );
		if( c.isNull() ) break;

		QString baliseText = c.selectedText();
		baliseText.remove( 0, 1 ).chop( 1 );
		baliseText = baliseText.trimmed();

		if( ! (prefix.isEmpty() || baliseText.startsWith( prefix + ":" ) || baliseText.startsWith( "/" + prefix + ":" ) ) ) {
			// do nothing
		} else if( baliseText.startsWith( '?' ) || baliseText.startsWith( '!' ) ) {
			// do nothing
		} else if( baliseText.startsWith( '/' ) && (includeOnly.isEmpty() || includeOnly.contains( baliseText ) ) ) {
			baliseText = baliseText.remove( 0, 1 ).trimmed();
			baliseClose.push( baliseText );
		} else if( baliseText.endsWith( '/' ) ) {
			// do nothing : add and remove from baliseClose
		} else {
			QString baliseName = baliseText.section( ' ', 0, 0 );
			
			if( ! ( includeOnly.isEmpty() || includeOnly.contains( baliseName ) ) ) continue; // do nothing
			if( !baliseClose.isEmpty() && ( baliseName == baliseClose.top() ) ) {
				baliseClose.pop();
			} else {
				XPathBalise xpathBalise;
				xpathBalise.name = baliseName;

				QStringList params = baliseText.split( ' ', QString::SkipEmptyParts );
				params.removeFirst();

				foreach( QString param, params ) {
					QString name  = param.section( '=', 0, 0 );
					if( attributeName.contains( name ) ) {
						QString value = param.section( '=', 1, 1 ).remove( '"' ).trimmed();
						xpathBalise.attributes[ name ] = value;
					}
				}

				xpath.append( xpathBalise );
			}
		}

		//c.setPosition( c.selectionStart() - 1 );
	} while( true );

	return xpath;
}

QString XmlTextEditor::xpathToString( const QList<XPathBalise> & xp ) {
	QString xps;
	foreach( XPathBalise balise, xp ) {
		QString text = balise.name;
		if( ! balise.attributes.isEmpty() ) {
			text += "[";
			QHashIterator<QString,QString> i( balise.attributes );
			while( i.hasNext() ) {
				i.next();
				text += "@" + i.key() + "='" + i.value() + "'";
				if( i.hasNext() )
					text += " and ";
			}
			text += "]";
		}
		xps.insert( 0, "/" + text );
	}
	return xps;
}
