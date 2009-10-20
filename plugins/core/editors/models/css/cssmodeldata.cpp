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
#include "cssmodeldata.h"
#include <contentview/contentviewnode.h>

// Qt header
#include <QFileInfo>
#include <QTextStream>
#include <QRegExp>
#include <QImage>
#include <QVariant>

/* CSSFileContentParser */

CSSFileContentParser::CSSFileContentParser( bool autoDelete ) : ContentViewParser( autoDelete ) {

}

CSSFileContentParser::~CSSFileContentParser() {

}

void CSSFileContentParser::loadFromDeviceImpl() {
	if( ! rootNode() ) return;

	rootNode()->setData( ":/images/typecss.png", ContentViewNode::NODE_ICON );

	loadAttachedNode( rootNode() );

	QString text = inputDevice()->readAll();

	if( text.isEmpty() ) {
		removeAttachedNodes();
		return;
	}

	QRegExp commentStartExpression("^/\\*");
	QRegExp commentEndExpression("\\*/");
	QRegExp keywordExpression("[A-Za-z_][A-Za-z0-9_-:.]*");
	QRegExp indentifierExpression("[^\n]*;");

	QList<ContentViewNode*> elements;

	int pos;
	ParsingState state = CssDefault;

	m_line = 1;

	for( int i = 0; i < text.length(); i++ ) {
		char c = text.at(i).toLower().toAscii();
		if( c == '/' ) {
			pos = commentStartExpression.indexIn( text, i, QRegExp::CaretAtOffset );

			if( pos == i ) {
				const int posEnd = commentEndExpression.indexIn( text, pos + 2, QRegExp::CaretAtOffset );
				if( posEnd == -1 )
					break;

				m_line += text.mid( pos, posEnd - pos ).count( QRegExp( "\\n" ) );

				i = posEnd;
			}
		} else if( ( c >= 'a' ) && ( c <= 'z' ) ) {
			if( state == CssDefault ) {
				// TAG
				pos = keywordExpression.indexIn( text, i, QRegExp::CaretAtOffset );

				if( pos == i ) {
					const int iLength = keywordExpression.matchedLength();
					elements.append( attacheNewTagNode( rootNode(), keywordExpression.cap(), m_line ) );
					i += iLength - 1;
				}
			} else if( state == CssIdentifier ) {
				// Identifier
				pos = indentifierExpression.indexIn( text, i, QRegExp::CaretAtOffset );

				if( pos == i ) {
					const int iLength = indentifierExpression.matchedLength();
					foreach( ContentViewNode * element, elements ) {
						attacheNewPropertyNode( element, indentifierExpression.cap(), m_line );
					}
					i += iLength - 1;
				}
			}
		} else if( c == ':' ) {
			if( state != CssIdentifier ){
				pos = keywordExpression.indexIn( text, i + 1, QRegExp::CaretAtOffset );
				if( pos == ( i + 1 ) ) {
					const int iLength = keywordExpression.matchedLength();
					// Pseudo class
					elements.append( attacheNewClassNode( rootNode(), ":" + keywordExpression.cap(), m_line ) );
					i += iLength;
				}
			}
		} else if( c == '.' ) {
			if( state != CssIdentifier ) {
				pos = keywordExpression.indexIn( text, i + 1, QRegExp::CaretAtOffset );
				if( pos == ( i + 1 ) ) {
					const int iLength = keywordExpression.matchedLength();
					// Class
					elements.append( attacheNewClassNode( rootNode(), "." + keywordExpression.cap(), m_line ) );
					i += iLength;
				}
			}
		} else if( c == '#' ) {
			if( state != CssIdentifier ) {
				pos = keywordExpression.indexIn( text, i + 1, QRegExp::CaretAtOffset );
				if( pos == ( i + 1 ) ) {
					const int iLength = keywordExpression.matchedLength();
					// ID
					elements.append( attacheNewIdNode( rootNode(), "#" + keywordExpression.cap(), m_line ) );
					i += iLength;
				}
			}
		} else if( c == '*' ) {
			if( state == CssDefault ) {
				elements.append( attacheNewTagNode( rootNode(), "*", m_line ) );
			}
		} else if( c == '{' ) {
			if( state == CssDefault )
				state = CssIdentifier;
		} else if( c == '}' ) {
			state = CssDefault;
			elements.clear();
		} else if( c == '\n' ) {
			m_line++;
		}
	}

	detachAttachedNode();
}

ContentViewNode * CSSFileContentParser::attacheNewPropertyNode( ContentViewNode * parent, const QString & name, int line ) {
	QStringList property = name.split( ':' );
	QString n, v;

	int cnt = property.count();
	if( cnt > 0 )
		n = property.at( 0 ).trimmed();
	if( cnt > 1 )
		v = property.at( 1 ).trimmed();
	if( v.endsWith( ';' ) )
		v.chop( 1 );


	ContentViewNode * node = new ContentViewNode( name, line );
	node->setData( "CssProperty", ContentViewNode::NODE_TYPE );
	node->setData( ":/images/html_value.png", ContentViewNode::NODE_ICON );
	node->setData( n, ContentViewNode::NODE_DISPLAY_NAME );
	node->setData( tr( "Element at line : %1\nValue : %2" ).arg( line ).arg( v ), ContentViewNode::NODE_DISPLAY_TIPS );
	node = attachNode( parent, node );

	return node;
}

ContentViewNode * CSSFileContentParser::attacheNewIdentifierNode( ContentViewNode * parent, const QString & name, int line ) {
	ContentViewNode * node = new ContentViewNode( name, line );
	node->setData( "CssIdentifier", ContentViewNode::NODE_TYPE );
	node->setData( ":/images/noeud.png", ContentViewNode::NODE_ICON );
	node->setData( name, ContentViewNode::NODE_DISPLAY_NAME );
	node->setData( tr( "Element at line : %1" ).arg( line ), ContentViewNode::NODE_DISPLAY_TIPS );
	node = attachNode( parent, node );

	return node;
}

ContentViewNode * CSSFileContentParser::attacheNewClassNode( ContentViewNode * parent, const QString & name, int line ) {
	ContentViewNode * node = new ContentViewNode( name, line );
	node->setData( "CssClass", ContentViewNode::NODE_TYPE );
	node->setData( ":/images/noeud.png", ContentViewNode::NODE_ICON );
	node->setData( name, ContentViewNode::NODE_DISPLAY_NAME );
	node->setData( tr( "Element at line : %1" ).arg( line ), ContentViewNode::NODE_DISPLAY_TIPS );
	node = attachNode( parent, node );

	return node;
}

ContentViewNode * CSSFileContentParser::attacheNewTagNode( ContentViewNode * parent, const QString & name, int line ) {
	ContentViewNode * node = new ContentViewNode( name, line );
	node->setData( "CssTag", ContentViewNode::NODE_TYPE );
	node->setData( ":/images/noeud.png", ContentViewNode::NODE_ICON );
	node->setData( name, ContentViewNode::NODE_DISPLAY_NAME );
	node->setData( tr( "Element at line : %1" ).arg( line ), ContentViewNode::NODE_DISPLAY_TIPS );
	node = attachNode( parent, node );

	return node;
}

ContentViewNode * CSSFileContentParser::attacheNewIdNode( ContentViewNode * parent, const QString & name, int line ) {
	ContentViewNode * node = new ContentViewNode( name, line );
	node->setData( "CssId", ContentViewNode::NODE_TYPE );
	node->setData( ":/images/noeud.png", ContentViewNode::NODE_ICON );
	node->setData( name, ContentViewNode::NODE_DISPLAY_NAME );
	node->setData( tr( "Element at line : %1" ).arg( line ), ContentViewNode::NODE_DISPLAY_TIPS );
	node = attachNode( parent, node );

	return node;
}