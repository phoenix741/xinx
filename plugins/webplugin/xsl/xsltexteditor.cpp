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
#include "xsltexteditor.h"
#include "xsllistview.h"
#include "xslmodelcompleter.h"
#include "../config/selfwebpluginsettings.h"

// QCodeEdit header
#include <qdocumentcursor.h>
#include <qdocumentline.h>

// Qt header
#include <QCompleter>
#include <QTextCursor>
#include <QTextBlock>

/* XslTextEditor */

XslTextEditor::XslTextEditor( QWidget * parent ) : XmlTextEditor( parent ), m_parser( 0 ) {

}

XslTextEditor::~XslTextEditor() {

}

void XslTextEditor::setParser( XslContentElementList * parser ) {
	m_parser = parser;
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

	if( completer ) {
		XSLCompletionModel * c = qobject_cast<XSLCompletionModel*>( completer->model() );
		if( c ) {
			QString templateName, templateMode;
			getTemplate( textCursor(), &templateName, &templateMode );

			c->setHiddenAttribute( QStringList() );
			c->setTemplateName( templateName, templateMode );
			cursorPosition pos = editPosition( textCursor() );
			if( pos == cpEditParamName ) {
				c->setFilter( m_nodeName ); // Param of the node
				c->setHiddenAttribute( paramOfNode( textCursor() ) ); // But hide some param
			} else if( pos == cpEditNodeName ) {
				c->setFilter(); // Edit Node
			} else if( pos == cpEditParamValue ) {
				c->setFilter( m_nodeName, m_paramName ); // Edit Value so
				if( ( m_nodeName == "xsl:apply-templates" ) && ( m_paramName == "mode" ) )
					c->setApplyTemplateMatch( paramValue( textCursor(), "select" ) );
		}

		return completer;
		}
	}
	return completer;
}


QDocumentCursor XslTextEditor::insertCompletionBalises( QDocumentCursor & tc, QString node ) {
	QString indent = tc.line().text();
	indent = indent.left( indent.indexOf( QRegExp( "\\S" ) ) );

	QDocumentCursor position = QDocumentCursor();
	int cnt = 0;

	if( m_parser && SelfWebPluginSettings::self()->config().xml.addDefaultSubBalise && ( node == "xsl:call-template" ) ) {
		QString nodeName = paramValue( tc, "name" );
		if( ! nodeName.isEmpty() ) {
			foreach( QString param, m_parser->params( nodeName ) ) {
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
