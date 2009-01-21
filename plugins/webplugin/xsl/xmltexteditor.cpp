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

// Xinx header
#include "xmltexteditor.h"
#include "../editorcompletion.h"
#include "xslmodelcompleter.h"
#include "../config/selfwebpluginsettings.h"

#include <xinxpluginsloader.h>
#include <xinxconfig.h>
#include <xinxlanguagefactory.h>

// QCodeEdit header
#include <qlanguagedefinition.h>
#include <qdocumentline.h>

// Qt header
#include <QKeyEvent>
#include <QTextBlock>
#include <QCompleter>
#include <QTextDocument>
#include <QStack>

// Define
#define EOWREGEXP	"[~!@\\$#%\\^&\\*\\(\\)\\+\\{\\}|\"<>,/;'\\[\\]\\\\=\\s]"
#define isEditBalise(value) ((value == cpEditNodeName) || (value == cpEditParamName) || (value == cpEditParamValue))


XmlTextEditor::XmlTextEditor( QWidget * parent ) : XinxCodeEdit( parent ) {
	setHighlighter( "XML" );
}

XmlTextEditor::~XmlTextEditor() {

}

QCompleter * XmlTextEditor::completer() {
	if( ! SelfWebPluginSettings::self()->config().xml.activeCompletion ) return 0;

	return XinxCodeEdit::completer();
}

void XmlTextEditor::commentSelectedText( bool uncomment ) {
	QString nodeName, paramName;

	QDocumentCursor cursor( textCursor() );

	QDocumentCursor cursorStart( document() );
	cursorStart.moveTo( cursor.selectionStart() );
	bool isStartCommented = editPosition( cursorStart, nodeName, paramName ) == cpEditComment;

	QDocumentCursor cursorEnd( textCursor() );
	cursorEnd.moveTo( cursor.selectionEnd() );
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

XmlTextEditor::cursorPosition XmlTextEditor::editPosition( const QDocumentCursor & cursor, QString & nodeName, QString & paramName ) {
	Q_ASSERT( ! cursor.isNull() );

	cursorPosition cPosition = cpNone;
	nodeName = QString();
	paramName = QString();

	/* is a Comment ? */
	QDocumentCursor cursorCommentStart( find ( "<!--", cursor, XinxCodeEdit::FindBackward ).selectionStart() );
	QDocumentCursor cursorCommentEnd( find ( "-->", cursor, XinxCodeEdit::FindBackward ).selectionStart() );

	bool inComment = ! (
			cursorCommentStart.isNull() // No comment before where i am
		||	( (! cursorCommentEnd.isNull()) && ( cursorCommentStart < cursorCommentEnd ) ) // There is a end before, and the last is the end balise
	 );

	if( inComment ) {
		cPosition = cpEditComment;
		return cPosition;
	} // else

	/* or simple text */
	QDocumentCursor cursorBaliseStart ( find ( QRegExp("<(?!\\!\\-\\-)"), cursor, XinxCodeEdit::FindBackward ).selectionEnd() ); // A balise is'nt a comment
	QDocumentCursor cursorBaliseEnd ( find ( ">", cursor, XinxCodeEdit::FindBackward ).selectionStart() );

	bool inNode = (! cursorBaliseStart.isNull()) && ( cursorBaliseEnd.isNull() || ( cursorBaliseEnd < cursorBaliseStart ) );
	if( ! inNode ) {
		return cPosition; // = cpNone;
	} // else

	/* Retrieve the name of node */ /* Space ? or other ? */
	QDocumentCursor cursorSpaceAfterNodeName ( find ( QRegExp( EOWREGEXP ), cursorBaliseStart ).selectionStart() );
	QDocumentCursor tc = cursor;

	tc.moveTo( cursorBaliseStart );
	if( ! cursorSpaceAfterNodeName.isNull() )
		tc.movePosition( cursorSpaceAfterNodeName.position() - tc.position(), QDocumentCursor::Right, QDocumentCursor::KeepAnchor ) ;
	else
		tc.movePosition( 1, QDocumentCursor::EndOfBlock, QDocumentCursor::KeepAnchor ) ;

	nodeName = tc.selectedText().trimmed();

	/* i'm editing a balise name ? */
	QDocumentCursor cursorSpace ( find ( QRegExp("\\s"), cursor, XinxCodeEdit::FindBackward ).selectionEnd() );
	if( cursorSpace < cursorBaliseStart )
		cursorSpace = QDocumentCursor();

	bool editBaliseName = cursorSpace.isNull();
	if( editBaliseName ) {
		cPosition = cpEditNodeName;
		return cPosition;
	} // else

	QDocumentCursor cursorEgal ( find ( "=", cursor, XinxCodeEdit::FindBackward ).selectionStart() );
	QDocumentCursor cursorQuote ( find ( "\"", cursor, XinxCodeEdit::FindBackward ).selectionStart() );
	if( cursorEgal < cursorBaliseStart )
		cursorEgal = QDocumentCursor();
	if( cursorQuote < cursorBaliseStart )
		cursorQuote = QDocumentCursor();

	int quoteCount = 0;
	if( ! cursorQuote.isNull() ) {
		quoteCount ++;
		QDocumentCursor cursorPreviousQuote( cursorQuote );

		cursorPreviousQuote.movePosition( 1, QDocumentCursor::PreviousCharacter );
		cursorPreviousQuote = find ( "\"", cursorPreviousQuote, XinxCodeEdit::FindBackward ).selectionStart();
		while( ( ! cursorPreviousQuote.isNull() ) &&  ( cursorBaliseStart < cursorPreviousQuote ) ) {
			quoteCount ++;

			cursorPreviousQuote.movePosition( 1, QDocumentCursor::PreviousCharacter );
			cursorPreviousQuote = find ( "\"", cursorPreviousQuote, XinxCodeEdit::FindBackward ).selectionStart();
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

	QDocumentCursor cursorParam ( find ( QRegExp( "[=\\s]" ), cursorSpace ).selectionStart() );
	tc.moveTo( cursorSpace ) ;
	if( ! cursorParam.isNull() )
		tc.movePosition( cursorParam.position() - tc.position(), QDocumentCursor::Right, QDocumentCursor::KeepAnchor ) ;
	else
		tc.movePosition( 1, QDocumentCursor::EndOfBlock, QDocumentCursor::KeepAnchor ) ;

	paramName = tc.selectedText().trimmed();

	cPosition = cpEditParamName;
	if( cursorEgal.isNull() || ( cursorQuote.isValid() && cursorSpace.isValid() && ( cursorEgal < cursorQuote ) && ( cursorEgal < cursorSpace ) && ( ( quoteCount % 2 ) == 0 ) ) )
		return cPosition;

	if( ! nodeName.isEmpty() )
		cPosition = cpEditParamValue;
	return cPosition;
}

XmlTextEditor::cursorPosition XmlTextEditor::editPosition( const QDocumentCursor & cursor ) {
	return editPosition( cursor, m_nodeName, m_paramName );
}


QStringList XmlTextEditor::paramOfNode( const QDocumentCursor & cursor ) {
	XmlTextEditor::cursorPosition position = editPosition( cursor );
	if( ( position == cpEditComment ) || ( position == cpNone ) ) return QStringList();

	QDocumentCursor baliseStart = find( QRegExp("[<>]"), cursor, XinxCodeEdit::FindBackward ).selectionStart();
	QDocumentCursor baliseStop = find( QRegExp("[<>]"), cursor ).selectionEnd();
	if( baliseStart.isNull() || baliseStop.isNull() ) return QStringList();

	QStringList result;

	QDocumentCursor c = cursor;
	do {
		c = find( QRegExp("\\W\\w+=\""), c, XinxCodeEdit::FindBackward ).selectionStart();
		if( c.isNull() || ( baliseStart >= c ) ) break;
		QDocumentCursor text( document() );
		text.moveTo( c.selectionStart() );
		text.movePosition( c.selectionEnd().position() - c.selectionStart().position() - 2,  QDocumentCursor::Right, QDocumentCursor::KeepAnchor );
		result << text.selectedText();
	} while( baliseStart < c );
	c = cursor;
	do {
// 		c = find( QRegExp("\\w+=\""), c );
		if( c.isNull() || ( c >= baliseStop ) ) break;
		c.movePosition( 2, QDocumentCursor::Left, QDocumentCursor::KeepAnchor );
		result << c.selectedText();
	} while( c < baliseStop );

	return result;
}

void XmlTextEditor::insertCompletion( const QModelIndex& index ) {
	QDocumentCursor tc = textCursor();

	QCompleter * c = completer();
	QString completion = c->completionModel()->data( index, XSLCompletionModel::Name ).toString(),
			prefix     = c->completionPrefix();

	textUnderCursor( tc, true );
	tc = textCursor();

	tc.insertText( completion );

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
				insertCompletionAccolade( tc, m_nodeName, m_paramName, completion, index );
			}

	setTextCursor( tc );
}

void XmlTextEditor::insertCompletionValue( QDocumentCursor & tc, QString node, QString param ) {
	if( xmlCompletionContents && xmlCompletionContents->balise( node ) && xmlCompletionContents->balise( node )->attribute( param ) && SelfWebPluginSettings::self()->config().xml.addDefaultAttribute ) {
		int defaultValue = xmlCompletionContents->balise( node )->attribute( param )->defaultValue();
		if( defaultValue >= 0 ) {
			tc.insertText( QString("=\"%1\"").arg( xmlCompletionContents->balise( node )->attribute( param )->values().at( defaultValue ) ) );
			return;
		}
	}
	tc.insertText("=\"\"");
	tc.movePosition( 1, QDocumentCursor::PreviousCharacter );
}

QDocumentCursor XmlTextEditor::insertCompletionParam( QDocumentCursor & tc, QString node, bool movePosition ) {
	QDocumentCursor result = QDocumentCursor();

	if( SelfWebPluginSettings::self()->config().xml.addDefaultAttribute && xmlCompletionContents && xmlCompletionContents->balise( node ) ) {
		CompletionXMLBalise* balise = xmlCompletionContents->balise( node );
		QStringList paramList = paramOfNode( tc );

		foreach( CompletionXMLAttribute* attr, balise->attributes() ) {
			if( attr->isDefault() && !paramList.contains( attr->name() ) ) {
				int defaultValue = attr->defaultValue();
				QString defaultValueString;

				if( defaultValue >= 0 )
					defaultValueString = attr->values().at( defaultValue );

				tc.insertText( QString(" %1=\"%2\"").arg( attr->name() ).arg( defaultValueString ) );

				if( result.isNull() && defaultValueString.isEmpty() ) {
					result = tc;
					result.movePosition( 1, QDocumentCursor::PreviousCharacter );
				}
			}
		}
	}
	if( result.isValid() && movePosition )
		tc.moveTo( result );

	return result;
}

QDocumentCursor XmlTextEditor::insertCompletionBalises( QDocumentCursor & tc, QString node ) {
	QString indent = tc.line().text();
	indent = indent.left( indent.indexOf( QRegExp( "\\S" ) ) );

	QDocumentCursor result = QDocumentCursor();
	int cnt = 0;

	if( SelfWebPluginSettings::self()->config().xml.addDefaultSubBalise && xmlCompletionContents && xmlCompletionContents->balise( node ) ) {
		CompletionXMLBalise* balise = xmlCompletionContents->balise( node );
		foreach( CompletionXMLBalise* b, balise->balises() ) {
			if( b->isDefault() ) {
				tc.insertText( "\n" + indent + "\t" );
				tc.insertText( "<" + b->name() );
				QDocumentCursor insertPosition = insertCompletionParam( tc, b->name(), false );
				if( result.isNull() ) result = insertPosition;
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
		if( result.isValid() )
			tc.moveTo( result );
	}

	return result;
}

void XmlTextEditor::insertCompletionAccolade( QDocumentCursor & tc, QString node, QString param, QString value, const QModelIndex & index ) {
	Q_UNUSED( param );
	QCompleter * c = completer();
	bool isVariable = c->completionModel()->data( index, XSLCompletionModel::isVariable ).toBool();
	bool isHtmlOnly = c->completionModel()->data( index, XSLCompletionModel::isHtmlOnly ).toBool();

	QDocumentCursor tc2( tc );
	tc2.movePosition( value.length(), QDocumentCursor::PreviousCharacter, QDocumentCursor::MoveAnchor );

	bool insertDollard = true, insertAccolade = true;
	QDocumentCursor tc3( tc2 );
	tc3.movePosition( 1, QDocumentCursor::PreviousCharacter, QDocumentCursor::KeepAnchor );
	if( tc3.selectedText() == "$" ) {
		tc2.movePosition( 1, QDocumentCursor::PreviousCharacter );
		insertDollard = false;
	}

	QDocumentCursor accOpen, accClose, debParam;
	accOpen = find( "{", tc2, XinxCodeEdit::FindBackward ).selectionStart();
	accClose = find( "}", tc2, XinxCodeEdit::FindBackward ).selectionStart();
	debParam = find( "\"", tc2, XinxCodeEdit::FindBackward ).selectionStart();
	if( accOpen.isValid() && debParam.isValid() && ( accClose.isNull() || ( accOpen > accClose ) ) && ( accOpen > debParam ) ) insertAccolade = false;

	if( xmlCompletionContents && xmlCompletionContents->balise( node ) ) {
		CompletionXMLBalise* balise = xmlCompletionContents->balise( node );

		if( insertAccolade && ( balise->category() != "stylesheet" ) && ( isVariable || !isHtmlOnly ) ) {
			if( insertDollard && isVariable ) {
				tc2.insertText( "{$" );
				tc.movePosition( 2, QDocumentCursor::NextCharacter );
			} else {
				tc2.insertText( "{" );
				tc.movePosition( 1, QDocumentCursor::NextCharacter );
			}
			tc.insertText( "}" );
		} else
		if( insertDollard && isVariable ) {
			tc2.insertText( "$" );
			tc.movePosition( 1, QDocumentCursor::NextCharacter );
		}
	}
}

bool XmlTextEditor::localKeyPressExecute( QKeyEvent * e ) {
	if ( ( e->key() == Qt::Key_Return ) && ( ( e->modifiers() == Qt::ControlModifier ) || ( e->modifiers() == (Qt::ShiftModifier | Qt::ControlModifier) ) ) ) {
		key_shenter( e->modifiers() & Qt::ShiftModifier );
		e->accept();
		return false;
	} else
		return XinxCodeEdit::localKeyPressExecute( e );
}

void XmlTextEditor::key_shenter( bool back ) {
	XinxCodeEdit::FindFlags flags = 0;
	if( back ) flags |= XinxCodeEdit::FindBackward;

	QDocumentCursor cursor = textCursor();
	if( back ) {
		cursor.moveTo( cursor.selectionStart() );
		cursor.movePosition( 2, QDocumentCursor::Left, QDocumentCursor::MoveAnchor );
	}
	cursor = find( QRegExp("=\"[^\"]*\""), cursor, flags );

	if( ! cursor.isNull() ) {
		QDocumentCursor newCursor = textCursor();
		newCursor.moveTo( cursor.selectionStart() );
		newCursor.movePosition( 2, QDocumentCursor::Right );
		newCursor.movePosition( cursor.selectionEnd().position() - cursor.selectionStart().position() - 3, QDocumentCursor::Right, QDocumentCursor::KeepAnchor );
		setTextCursor( newCursor );
	}
}

bool XmlTextEditor::processKeyPress( QKeyEvent * e ) {
	if( ! SelfWebPluginSettings::self()->config().xml.activeCompletion ) return false;

	if( !e->text().isEmpty() ) {
		if( ( e->text().right(1) == ">" ) && ( SelfWebPluginSettings::self()->config().xml.addClosedBalise ) ) {
			QDocumentCursor tc( textCursor() );
			tc.movePosition( 2, QDocumentCursor::PreviousCharacter, QDocumentCursor::KeepAnchor );

			QString selected = tc.selectedText();

			tc.movePosition( 1, QDocumentCursor::NextCharacter );

			if( isEditBalise( editPosition( tc ) ) && selected != "/>" ){
				QString name = m_nodeName;
				if( ( ! name.isEmpty() ) && ( name.at(0) != '/' ) ) {
					tc.movePosition( 1, QDocumentCursor::NextCharacter );
					QDocumentCursor position = textCursor();
					QDocumentCursor newPosition = insertCompletionBalises( tc, name );
					if( newPosition.isNull() )
						tc.moveTo( position );
					setTextCursor( tc );
				}
			}
			return true;
		} else if( e->text().right(1) == "=" && SelfWebPluginSettings::self()->config().xml.addDefaultAttribute ) {
			QDocumentCursor tc( textCursor() );
			QDocumentCursor tc2( textCursor() );
			tc2.movePosition( 1, QDocumentCursor::PreviousCharacter );
			if( editPosition( tc2 ) == cpEditParamName ) {
				tc.deletePreviousChar();
				insertCompletionValue( tc, m_nodeName, m_paramName );
				setTextCursor( tc );
			}
			return true;
		} else if( e->text().right(1) == " " && SelfWebPluginSettings::self()->config().xml.addDefaultAttribute ) {
			QDocumentCursor tc( textCursor() );
			QDocumentCursor tc2( textCursor() );
			tc2.movePosition( 1, QDocumentCursor::PreviousCharacter );
			if( editPosition( tc2 ) == cpEditNodeName ) {
				tc.deletePreviousChar();
				if( insertCompletionParam( tc, m_nodeName ).isNull() )
					tc.insertText( " " );
				setTextCursor( tc );
			}
			return true;
		}
	}
	return false;
}

QList<XPathBalise> XmlTextEditor::xpath( const QDocumentCursor & cursor, const QStringList & includeOnly, const QString & prefix, const QStringList & attributeName ) {
	QList<XPathBalise> xpath;
	QStack<QString> baliseClose;

	QDocumentCursor c = cursor;
	do {
		c = find( QRegExp( "<[^<>]*>" ), c, XinxCodeEdit::FindBackward ).selectionStart();
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
