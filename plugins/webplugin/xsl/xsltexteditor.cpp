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
#include "xsltexteditor.h"
#include "xsllistview.h"
#include "xslmodelcompleter.h"
#include "../config/selfwebpluginsettings.h"

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

QString XslTextEditor::paramValue( const QTextCursor & cursor, const QString & param ) {
	QString text;
	QTextCursor lt = document()->find( "<", cursor, QTextDocument::FindBackward );
	QTextCursor c = document()->find( QRegExp( param + "=\"[^\"]*\"" ), cursor, QTextDocument::FindBackward );
	if( ( ! c.isNull() ) && ( lt < c ) ) {
		text = c.selectedText();
	} else {
		QTextCursor gt = document()->find( ">", cursor );
		QTextCursor c = document()->find( QRegExp( param + "=\"[^\"]*\"" ), cursor );
		if( ( ! c.isNull() ) && ( c < gt ) ) {
			text = c.selectedText();
		} else
			return QString();
	}

	text.remove( 0, param.length() + 2 ).chop( 1 );
	return text;
}

void XslTextEditor::getTemplate( const QTextCursor & cursor, QString * name, QString * mode ) {
	if( ( name == 0 ) && ( mode == 0 ) ) return ;
	*name = QString();
	*mode = QString();
	QTextCursor c = document()->find( "xsl:template", cursor, QTextDocument::FindBackward );
	if( ! c.isNull() ) {
		*name  = paramValue( c, "name" );
		if( name->isEmpty() )
			*name = paramValue( c, "match" );
		*mode  = paramValue( c, "mode" );
	}
}

//#include <QDebug>
QCompleter * XslTextEditor::completer() {
	QCompleter * completer = XmlTextEditor::completer();

	if( completer ) {
		XSLCompletionModel * c = qobject_cast<XSLCompletionModel*>( completer->model() );
		if( c ) {
			QString templateName, templateMode;
			getTemplate( textCursor(), &templateName, &templateMode );
			//qDebug() << "Ask for the model : name = " << templateName << ", mode" << templateMode;

			c->setHiddenAttribute( QStringList() );
			c->setTemplateName( templateName, templateMode );
			cursorPosition pos = editPosition( textCursor() );
			if( pos == cpEditParamName ) {
				//qDebug() << "Edit param (node name) " << m_nodeName;
				c->setFilter( m_nodeName ); // Param of the node
				c->setHiddenAttribute( paramOfNode( textCursor() ) ); // But hide some param
			} else if( pos == cpEditNodeName ) {
				//qDebug() << "Edit node";
				c->setFilter(); // Edit Node
			} else if( pos == cpEditParamValue ) {
				//qDebug() << "Edit param value (node name, param name) " << m_nodeName << " " << m_paramName;
				//qDebug() << "match is " << paramValue( textCursor(), "select" );
				c->setFilter( m_nodeName, m_paramName ); // Edit Value so
				if( ( m_nodeName == "xsl:apply-templates" ) && ( m_paramName == "mode" ) )
					c->setApplyTemplateMatch( paramValue( textCursor(), "select" ) );
		}

		return completer;
		}
	}
	return completer;
}


int XslTextEditor::insertCompletionBalises( QTextCursor & tc, QString node ) {
	QString indent = tc.block().text();
	indent = indent.left( indent.indexOf( QRegExp( "\\S" ) ) );

	int position = -1, cnt = 0;

	if( m_parser && SelfWebPluginSettings::self()->config().xml.addDefaultSubBalise && ( node == "xsl:call-template" ) ) {
		QString nodeName = paramValue( tc, "name" );
		if( ! nodeName.isEmpty() ) {
			foreach( QString param, m_parser->params( nodeName ) ) {
				tc.insertText( "\n" + indent + "\t" );
				tc.insertText( "<xsl:with-param name=\"" + param + "\" select=\"\"/>" );
				if( position == -1 ) position = tc.position() - 3;
				cnt++;
			}
		}
	}

	if( cnt > 0 )
		tc.insertText( "\n" + indent );

	if( position == -1 ) {
		position = XmlTextEditor::insertCompletionBalises( tc, node );
	} else {
		XmlTextEditor::insertCompletionBalises( tc, node );
		tc.setPosition( position );
	}

	return position;
}

void XslTextEditor::insertCompletionAccolade( QTextCursor & tc, QString node, QString param, QString value, const QModelIndex & index ) {
	XmlTextEditor::insertCompletionAccolade( tc, node, param, value, index );

	QStringList list = paramOfNode( tc );
	QCompleter * c = completer();
	if( ( node == "xsl:apply-templates" ) && (c->completionModel()->data( index ).toString().indexOf( "[" ) >= 0) && !list.contains( "mode" ) ) {
		QString mode = c->completionModel()->data( index ).toString();
		mode = mode.mid( mode.indexOf( '[' ) + 1 );
		mode.remove( mode.indexOf( ']' ), mode.length() );

		QTextCursor tc2 = document()->find( "\"", tc );
		tc2.setPosition( tc2.selectionEnd() );
		tc2.insertText( " mode=\"" + mode + "\"" );
		tc2.insertText( "/>" );
		tc = tc2;
	}
}
