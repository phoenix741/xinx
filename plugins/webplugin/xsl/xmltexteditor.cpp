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
#include "../config/selfwebpluginsettings.h"
#include "xslcompletionnodemodel.h"
#include "xslcontentviewparser.h"

#include <plugins/xinxpluginsloader.h>
#include <core/xinxconfig.h>
#include <editors/xinxlanguagefactory.h>
#include <contentview/completionnodemodel.h>
#include <contentview/contentviewnode.h>

// QCodeEdit header
#include <qlanguagedefinition.h>
#include <qdocumentcursor.h>
#include <qdocumentline.h>

// Qt header
#include <QKeyEvent>
#include <QTextBlock>
#include <QCompleter>
#include <QTextDocument>
#include <QTextCursor>
#include <QStack>
#include <QModelIndex>

// Define
#define EOWREGEXP	"[~!@\\$#%\\^&\\*\\(\\)\\+\\{\\}|\"<>,/;'\\[\\]\\\\=\\s]"
#define isEditBalise(value) ((value == cpEditNodeName) || (value == cpEditParamName) || (value == cpEditParamValue))

/* XmlTextEditor */

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

	int cursorPosition = cursor.position();
	nodeName = QString();
	paramName = QString();

	QDocumentCursor cursorCommentTag = find( QRegExp( "(<!--)|(-->)" ), cursor, XinxCodeEdit::FindBackward );
	if( cursorCommentTag.isValid() && ( cursorCommentTag.selectedText() == "<!--" ) ) {
		return cpEditComment;
	}

	QDocumentCursor cursorNodeTag = find( QRegExp( "(<(?!!--))|>" ), cursor, XinxCodeEdit::FindBackward );
	if( cursorNodeTag.isNull() || ( cursorNodeTag.selectedText() == ">" ) ) {
		return cpNone;
	}

	QDocumentCursor tc( document() );
	tc.moveTo( cursorNodeTag.selectionEnd() );
	tc.movePosition( cursorPosition - cursorNodeTag.position() - 1, QDocumentCursor::Right, QDocumentCursor::KeepAnchor );

	QString baliseContentStr =  tc.selectedText();
	QStringList baliseContent;// = baliseContentStr.split( QRegExp( "[0-9\\w:\\-]+" ), QString::SkipEmptyParts );

	// Split baliseContentStr into baliseContent by space. But space is keeped in quote.
	for( int i = 0; i < baliseContentStr.length(); i++ ) {
		QString text;
		bool quoted = false;
		while( ( i < baliseContentStr.length() ) && baliseContentStr.at( i ).isSpace() ) i++;
		while( ( i < baliseContentStr.length() ) && ( (! baliseContentStr.at( i ).isSpace()) || quoted ) ) {
			text += baliseContentStr.at( i );
			if( baliseContentStr.at( i ) == '"' )
				quoted = ! quoted;
			i++;
		}
		if( ! text.isEmpty() )
			baliseContent.append( text );
	}

	if( baliseContent.size() == 0 ) {
		return cpEditNodeName;
	}

	bool hasTextBeforeCursor = ! baliseContentStr.right( 1 ).trimmed().isEmpty();
	nodeName = baliseContent.first();

	if( ( baliseContent.size() == 1 ) && hasTextBeforeCursor ) {
		return cpEditNodeName;
	}

	paramName = baliseContent.last();

	int equalsIndex = paramName.indexOf( "=" );
	if( equalsIndex == -1 ) {
		return cpEditParamName;
	}

	if( paramName.count( "\"" ) == 2 ) {
		paramName = QString();
		return cpEditParamName;
	}

	paramName = paramName.left( equalsIndex );

	return cpEditParamValue;
}

QStringList XmlTextEditor::paramOfNode( const QDocumentCursor & cursor ) {
	QString nodeName, paramName;
	XmlTextEditor::cursorPosition position = editPosition( cursor, nodeName, paramName );
	if( ( position == cpEditComment ) || ( position == cpNone ) ) return QStringList();

	QDocumentCursor baliseStart = find( "<", cursor, XinxCodeEdit::FindBackward ).selectionStart();
	QDocumentCursor baliseStop = find( QRegExp("[<>]"), cursor ).selectionEnd();
	if( baliseStart.isNull() || baliseStop.isNull() ) return QStringList();

	QStringList result;

	QDocumentCursor c = cursor;
	do {
		c = find( QRegExp("\\s[\\w:-]+=\""), c, XinxCodeEdit::FindBackward );
		if( c.isNull() || ( baliseStart >= c ) ) break;
		QDocumentCursor text( document() );
		text.moveTo( c.selectionStart() );
		text.movePosition( 1, QDocumentCursor::Right );
		text.movePosition( c.selectionEnd().position() - c.selectionStart().position() - 3,  QDocumentCursor::Right, QDocumentCursor::KeepAnchor );
		result << text.selectedText();
	} while( baliseStart < c );
	c = cursor;
	do {
 		c = find( QRegExp("[\\w:-]+=\""), c );
		if( c.isNull() || ( c >= baliseStop ) ) break;
		c.movePosition( 2, QDocumentCursor::Left, QDocumentCursor::KeepAnchor );
		result << c.selectedText();
	} while( c < baliseStop );

	return result;
}

void XmlTextEditor::insertCompletion( const QModelIndex& index ) {
	QDocumentCursor tc = textCursor();

	QCompleter * c = completer();
	QString completion = c->completionModel()->data( index, CompletionNodeModel::CompletionNodeName ).toString(),
			prefix     = c->completionPrefix();

	textUnderCursor( tc, true );
	tc = textCursor();

	tc.insertText( completion );

	QString nodeName, paramName;
	cursorPosition pos = editPosition( tc, nodeName, paramName );
	if( pos == cpEditParamName ) {
		insertCompletionValue( tc, nodeName, completion );
	} else
		if( pos == cpEditNodeName ) {
			if( ! nodeName.isEmpty() )
				insertCompletionParam( tc, completion );
			else
				tc.insertText( ">" );
		} else
			if( pos == cpEditParamValue ) {
				insertCompletionAccolade( tc, nodeName, paramName, completion, index );
			}

	setTextCursor( tc );
}

void XmlTextEditor::insertCompletionValue( QDocumentCursor & tc, QString node, QString param ) {
	ContentViewNode * n = XmlCompletionParser::self()->baliseAttribute( node, param );
	if( n && SelfWebPluginSettings::self()->config().xml.addDefaultAttribute ) {
		ContentViewNode * defaultValue = XmlCompletionParser::self()->defaultValue( n );
		if( defaultValue ) {
			tc.insertText( QString("=\"%1\"").arg( defaultValue->data().toString() ) );
			return;
		}
	}
	tc.insertText("=\"\"");
	tc.movePosition( 1, QDocumentCursor::PreviousCharacter );
}

QDocumentCursor XmlTextEditor::insertCompletionParam( QDocumentCursor & tc, QString node, bool movePosition ) {
	QDocumentCursor result = QDocumentCursor();

	ContentViewNode * balise = 0;
	if( SelfWebPluginSettings::self()->config().xml.addDefaultAttribute && ( balise = XmlCompletionParser::self()->balise( node ) ) ) {
		QStringList paramList = paramOfNode( tc );

		foreach( ContentViewNode * attr, XmlCompletionParser::self()->defaultAttributes( balise ) ) {
			if( !paramList.contains( attr->data().toString() ) ) {
				QString defaultValueString;
				ContentViewNode * defaultValue = XmlCompletionParser::self()->defaultValue( balise );
				if( defaultValue ) {
					defaultValueString = defaultValue->data().toString();
				}

				tc.insertText( QString(" %1=\"%2\"").arg( attr->data().toString() ).arg( defaultValueString ) );

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

	ContentViewNode * balise = 0;
	if( SelfWebPluginSettings::self()->config().xml.addDefaultSubBalise && ( balise = XmlCompletionParser::self()->balise( node ) ) ) {
		foreach( ContentViewNode * b, XmlCompletionParser::self()->defaultBalises( balise ) ) {
			QString name = b->data().toString();
			tc.insertText( "\n" + indent + "\t" );
			tc.insertText( "<" + name );
			QDocumentCursor insertPosition = insertCompletionParam( tc, name, false );
			if( result.isNull() ) result = insertPosition;
			tc.insertText( ">" );
			insertCompletionBalises( tc, name );
			cnt++;
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
	bool isVariable = c->completionModel()->data( index, XslCompletionNodeModel::isVariable ).toBool();
	bool isHtmlOnly = c->completionModel()->data( index, XslCompletionNodeModel::isHtmlOnly ).toBool();

	tc.setAutoUpdated( true );
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

	ContentViewNode * balise = XmlCompletionParser::self()->balise( node );
	if( balise ) {
		if( insertAccolade && ( balise->data( XmlCompletionParser::NODE_XML_TYPE ).toString() != "stylesheet" ) && ( isVariable || !isHtmlOnly ) ) {
			if( insertDollard && isVariable ) {
				tc2.insertText( "{$" );
				//tc.movePosition( 2, QDocumentCursor::NextCharacter );
			} else {
				tc2.insertText( "{" );
				//tc.movePosition( 1, QDocumentCursor::NextCharacter );
			}
			tc.insertText( "}" );
		} else
		if( insertDollard && isVariable ) {
			tc2.insertText( "$" );
			//tc.movePosition( 1, QDocumentCursor::NextCharacter );
		}
	}
}

bool XmlTextEditor::localKeyPressExecute( QKeyEvent * e ) {
	if ( ( e->key() == Qt::Key_Return ) && ( ( e->modifiers() == Qt::ControlModifier ) || ( e->modifiers() == (Qt::ShiftModifier | Qt::ControlModifier) ) ) ) {
		key_shenter( e->modifiers() & Qt::ShiftModifier );
		e->ignore();
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
		if( ( e->key() == Qt::Key_Enter ) || ( e->key() == Qt::Key_Return ) ) {
			QDocumentCursor tc( textCursor() );
			bool hasOpenBalise = false, hasCloseBalise = false;
			QDocumentLine line = tc.line(), previousLine = line.previous();

			foreach ( QParenthesis p, line.parentheses() ) {
				if ( p.role & QParenthesis::Close )
					hasCloseBalise = true;
			}
			foreach ( QParenthesis p, previousLine.parentheses() ) {
				if ( p.role & QParenthesis::Open )
					hasOpenBalise = true;
			}

			if( hasOpenBalise && hasCloseBalise )
				tc.deletePreviousChar();

			return true;
		} else if( ( e->text().right(1) == ">" ) && ( SelfWebPluginSettings::self()->config().xml.addClosedBalise ) ) {
			QDocumentCursor tc( textCursor() );
			tc.movePosition( 2, QDocumentCursor::PreviousCharacter, QDocumentCursor::KeepAnchor );

			QString selected = tc.selectedText();

			tc.movePosition( 1, QDocumentCursor::NextCharacter );

			QString nodeName, paramName;
			if( isEditBalise( editPosition( tc, nodeName, paramName ) ) && selected != "/>" ){
				QString name = nodeName;
				if( ( ! name.isEmpty() ) && ( name.at(0) != '/' ) ) {
					tc.movePosition( 1, QDocumentCursor::NextCharacter );
					QDocumentCursor position = textCursor();
					position.setAutoUpdated( false );
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
			QString nodeName, paramName;
			if( editPosition( tc2, nodeName, paramName ) == cpEditParamName ) {
				tc.deletePreviousChar();
				insertCompletionValue( tc, nodeName, paramName );
				setTextCursor( tc );
			}
			return true;
		} else if( e->text().right(1) == " " && SelfWebPluginSettings::self()->config().xml.addDefaultAttribute ) {
			QDocumentCursor tc( textCursor() );
			QDocumentCursor tc2( textCursor() );
			tc2.movePosition( 1, QDocumentCursor::PreviousCharacter );
			QString nodeName, paramName;
			if( editPosition( tc2, nodeName, paramName ) == cpEditNodeName ) {
				tc.deletePreviousChar();
				if( insertCompletionParam( tc, nodeName ).isNull() )
					tc.insertText( " " );
				setTextCursor( tc );
			}
			return true;
		} else if( e->text().right(1) == "/" ) {
			QDocumentCursor tc( textCursor() );
			tc.movePosition( 2, QDocumentCursor::PreviousCharacter, QDocumentCursor::KeepAnchor );

			QString selected = tc.selectedText();
			if( selected == "</" ) {
				QDocumentCursor balise = find( QRegExp("[<>]"), textCursor() );
				if( balise.isNull() || ( balise.selectedText() == "<" ) ) {
					QDocumentCursor c = textCursor();
					QList<XPathBalise> path = xpath( c );
					QString baliseName = path.first().name;
					c.insertText( baliseName + ">" );
					setTextCursor( c );
				}
			}
		}
	}
	return false;
}

QList<XPathBalise> XmlTextEditor::xpath( const QDocumentCursor & cursor, const QStringList & includeOnly, const QString & prefix, const QStringList & attributeName ) {
	QList<XPathBalise> xpath;
	QStack<QString> baliseClose;

	QDocumentCursor c = cursor;
	do {
		c = find( QRegExp( "<[^<>]*>" ), c, XinxCodeEdit::FindBackward );
		if( c.isNull() ) break;

		QString baliseText = c.selectedText(), baliseEpuree = baliseText;
		baliseText.remove( 0, 1 ).chop( 1 );
		baliseText = baliseText.trimmed();
		baliseEpuree.remove( 0, 2 ).chop( 1 );
		baliseEpuree = baliseEpuree.trimmed();
		baliseEpuree = baliseEpuree.section( ' ', 0, 0 );

		if( ! (prefix.isEmpty() || baliseText.startsWith( prefix + ":" ) || baliseText.startsWith( "/" + prefix + ":" ) ) ) {
			// do nothing
		} else if( baliseText.startsWith( '?' ) || baliseText.startsWith( '!' ) ) {
			// do nothing
		} else if( baliseText.startsWith( '/' ) && (includeOnly.isEmpty() || includeOnly.contains( baliseEpuree ) ) ) {
			baliseClose.push( baliseEpuree );
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

				foreach( const QString & param, params ) {
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
	foreach( const XPathBalise & balise, xp ) {
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

/* XslTextEditor */

XslTextEditor::XslTextEditor( QWidget * parent ) : XmlTextEditor( parent ), m_model( 0 ) {

}

XslTextEditor::~XslTextEditor() {

}

void XslTextEditor::setModel( XslCompletionNodeModel * model ) {
	m_model = model;
}

QString XslTextEditor::paramValue( const QDocumentCursor & cursor, const QString & param ) {
	QString text;
	QDocumentCursor lt = find( "<", cursor, XinxCodeEdit::FindBackward );
	QDocumentCursor c = find( QRegExp( param + "=\"[^\"]*\"" ), cursor, XinxCodeEdit::FindBackward );

	if( ( ! c.isNull() ) && ( lt < c ) ) {
		text = c.selectedText();
	} else {
		QDocumentCursor gt = find( ">", cursor );
		QDocumentCursor c = find( QRegExp( param + "=\"[^\"]*\"" ), cursor );
		if( ( ! c.isNull() ) && ( c < gt ) ) {
			text = c.selectedText();
		} else
			return QString();
	}

	text.remove( 0, param.length() + 2 ).chop( 1 );
	return text;
}

void XslTextEditor::getTemplate( const QDocumentCursor & cursor, QString * name, QString * mode ) {
	if( ( name == 0 ) && ( mode == 0 ) ) return ;
	*name = QString();
	*mode = QString();
	QDocumentCursor c = find( "xsl:template", cursor, XinxCodeEdit::FindBackward );
	if( ! c.isNull() ) {
		*name  = paramValue( c, "name" );
		if( name->isEmpty() )
			*name = paramValue( c, "match" );
		*mode  = paramValue( c, "mode" );
	}
}

QCompleter * XslTextEditor::completer() {
	QCompleter * completer = XmlTextEditor::completer();

	if( completer && m_model ) {
		QString templateName, templateMode, nodeName, paramName, applyTemplateMatch;
		getTemplate( textCursor(), &templateName, &templateMode );
		cursorPosition pos = editPosition( textCursor(), nodeName, paramName );

		switch( pos ) {
		case cpEditNodeName:
			m_model->setCompleteNode();
			break;
		case cpEditParamName:
			m_model->setCompleteAttribute( nodeName, paramOfNode( textCursor() ) );
			break;
		case cpEditParamValue:
			if( ( nodeName == "xsl:apply-templates" ) && ( paramName == "mode" ) )
				applyTemplateMatch = paramValue( textCursor(), "select" );

			m_model->setCompleteValue( nodeName, paramName, applyTemplateMatch );
			m_model->setCurrentTemplateName( templateName, templateMode );
			break;
		case cpEditComment:
		case cpNone:
			m_model->setCompleteNone();
			break;
		}
	}
	return completer;
}


QDocumentCursor XslTextEditor::insertCompletionBalises( QDocumentCursor & tc, QString node ) {
	QString indent = tc.line().text();
	indent = indent.left( indent.indexOf( QRegExp( "\\S" ) ) );

	QDocumentCursor position = QDocumentCursor();
	int cnt = 0;

	if( m_model && SelfWebPluginSettings::self()->config().xml.addDefaultSubBalise && ( node == "xsl:call-template" ) ) {
		QString nodeName = paramValue( tc, "name" );
		if( ! nodeName.isEmpty() ) {
			foreach( const QString & param, m_model->params( nodeName ) ) {
				tc.insertText( "\n" + indent + "\t" );
				tc.insertText( "<xsl:with-param name=\"" + param + "\" select=\"\"/>" );
				if( position.isNull() ) {
					position = tc;
					position.movePosition( 3, QDocumentCursor::Left );
				}
				cnt++;
			}
		}
	}

	if( cnt > 0 )
		tc.insertText( "\n" + indent );

	if( position.isNull() ) {
		position = XmlTextEditor::insertCompletionBalises( tc, node );
	} else {
		XmlTextEditor::insertCompletionBalises( tc, node );
		tc.moveTo( position );
	}

	return position;
}

void XslTextEditor::insertCompletionAccolade( QDocumentCursor & tc, QString node, QString param, QString value, const QModelIndex & index ) {
	XmlTextEditor::insertCompletionAccolade( tc, node, param, value, index );

	QStringList list = paramOfNode( tc );
	QCompleter * c = completer();
	if( ( node == "xsl:apply-templates" ) && (c->completionModel()->data( index ).toString().indexOf( "[" ) >= 0) && !list.contains( "mode" ) ) {
		QString mode = c->completionModel()->data( index ).toString();
		mode = mode.mid( mode.indexOf( '[' ) + 1 );
		mode.remove( mode.indexOf( ']' ), mode.length() );

		QDocumentCursor tc2 = find( "\"", tc );
		tc2.moveTo( tc2.selectionEnd() );
		tc2.insertText( " mode=\"" + mode + "\"" );
		tc2.insertText( "/>" );
		tc = tc2;
	}
}
