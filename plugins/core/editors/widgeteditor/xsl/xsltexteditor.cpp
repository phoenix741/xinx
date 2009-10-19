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
#include "config/selfwebpluginsettings.h"
#include "editors/models/xsl/xslcompletionnodemodel.h"
#include "editors/models/xsl/xslcontentviewparser.h"

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
