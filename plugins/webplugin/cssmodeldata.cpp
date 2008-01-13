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
#include "cssmodeldata.h"
#include "iextendededitor.h"

// Qt header
#include <QFileInfo>
#include <QTextStream>
#include <QRegExp>

/* CssParserException */

CssParserException::CssParserException( const QString & message, int line, int column ) : FileContentException( message, line, column ) {
}

/* CssFileContentProperty */

CssFileContentProperty::CssFileContentProperty( FileContentElement * parent, const QString & name, int line )  : FileContentElement( parent, name, line ) {
	QStringList property = name.split( ':' );
	int cnt = property.count();
	if( cnt > 0 )
		setName( property.at( 0 ).trimmed() );
	if( cnt > 1 )
		m_value = property.at( 1 ).trimmed();
	if( m_value.endsWith( ';' ) )
		m_value.chop( 1 );
}

const QString & CssFileContentProperty::value() const {
	return m_value;
}

QString CssFileContentProperty::displayTips() const {
	return FileContentElement::displayTips() + tr( "\nValue : %2" ).arg( m_value );
}

void CssFileContentProperty::copyFrom( FileContentElement * element ) {
	FileContentElement::copyFrom( element );
	if( dynamic_cast<CssFileContentProperty*>( element ) )
		m_value = dynamic_cast<CssFileContentProperty*>( element )->m_value;
}

QIcon CssFileContentProperty::icon() const {
	XINX_TRACE( "XSLFileContentVariable::icon", "()" );

	return QIcon(":/images/html_value.png");
}

/* CssFileContentIdentifier */

CssFileContentIdentifier::CssFileContentIdentifier( FileContentElement * parent, const QString & name, int line ) : FileContentElement( parent, name, line ) {
	
}

FileContentElement * CssFileContentIdentifier::append( FileContentElement * element ) {
	return FileContentElement::append( element );
}

QIcon CssFileContentIdentifier::icon() const {
	return QIcon( ":/images/noeud.png" );
}

/* CssFileContentClass */

CssFileContentClass::CssFileContentClass( FileContentElement * parent, const QString & name, int line ) : CssFileContentIdentifier( parent, name, line ) {
	
}

/* CssFileContentTag */

CssFileContentTag::CssFileContentTag( FileContentElement * parent, const QString & name, int line ) : CssFileContentIdentifier( parent, name, line ) {
	
}

/* CssFileContentId */

CssFileContentId::CssFileContentId( FileContentElement * parent, const QString & name, int line ) : CssFileContentIdentifier( parent, name, line ) {
	
}

/* CSSFileContentParser */

CSSFileContentParser::CSSFileContentParser( IXinxExtendedEditor * editor, FileContentElement * parent, const QString & filename, int lineNumber ) : FileContentElement( parent, filename, lineNumber ), m_editor( editor ), m_isLoaded( true ) {
	
}

CSSFileContentParser::~CSSFileContentParser() {
	
}

void CSSFileContentParser::loadFromContent( const QString & text ) {
	m_isLoaded = true;

	if( text.isEmpty() ) {
		clear();
		return;
	}

	QRegExp commentStartExpression("^/\\*");
	QRegExp commentEndExpression("\\*/");
	QRegExp keywordExpression("[A-Za-z_][A-Za-z0-9_-:.]*");
	QRegExp indentifierExpression("[^\n]*;");
	
	QList<FileContentElement*> elements;
	
	int pos;
	ParsingState state = CssDefault;

	markAllDeleted();
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
					elements.append( append( new CssFileContentTag( this, keywordExpression.cap(), m_line ) ) );
					i += iLength - 1;
				}
			} else if( state == CssIdentifier ) {
				// Identifier
				pos = indentifierExpression.indexIn( text, i, QRegExp::CaretAtOffset );
			
				if( pos == i ) {
					const int iLength = indentifierExpression.matchedLength();
					foreach( FileContentElement* element, elements )
						dynamic_cast<CssFileContentIdentifier*>( element )->append( new CssFileContentProperty( element, indentifierExpression.cap(), m_line ) );
					i += iLength - 1;
				}
			}
		} else if( c == ':' ) {
			if( state != CssIdentifier ){
				pos = keywordExpression.indexIn( text, i + 1, QRegExp::CaretAtOffset );
				if( pos == ( i + 1 ) ) {
					const int iLength = keywordExpression.matchedLength();
					// Pseudo class
					elements.append( append( new CssFileContentClass( this, ":" + keywordExpression.cap(), m_line ) ) );
					i += iLength;
				}
			}
		} else if( c == '.' ) {
			if( state != CssIdentifier ) {
				pos = keywordExpression.indexIn( text, i + 1, QRegExp::CaretAtOffset );
				if( pos == ( i + 1 ) ) {
					const int iLength = keywordExpression.matchedLength();
					// Class
					elements.append( append( new CssFileContentClass( this, "." + keywordExpression.cap(), m_line ) ) );
					i += iLength;
				}
			}
		} else if( c == '#' ) {
			if( state != CssIdentifier ) {
				pos = keywordExpression.indexIn( text, i + 1, QRegExp::CaretAtOffset );
				if( pos == ( i + 1 ) ) {
					const int iLength = keywordExpression.matchedLength();
					// ID
					elements.append( append( new CssFileContentId( this, "#" + keywordExpression.cap(), m_line ) ) );
					i += iLength;
				}
			}
		} else if( c == '*' ) {
			if( state == CssDefault )
				elements.append( append( new CssFileContentTag( this, "*", m_line ) ) );
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
	
	removeMarkedDeleted();
}

void CSSFileContentParser::loadFromFile( const QString & filename ) {
	setFilename( filename );
	setName( QFileInfo( filename ).fileName() );

	QFile file( filename );

	// Open the file
	if (!file.open(QFile::ReadOnly | QFile::Text)) 
		throw CssParserException( QObject::tr("Cannot read file %1:\n%2.").arg(filename).arg(file.errorString()), 0, 0 );

	QTextStream text( &file );
	QString content = text.readAll();
	loadFromContent( content );
}

void CSSFileContentParser::loadFromFileDelayed( const QString & filename ) {
	setFilename( filename );
	setName( QFileInfo( filename ).fileName() );
	
	m_isLoaded = false;
}

bool CSSFileContentParser::isLoaded() {
	return m_isLoaded;
}
	
int CSSFileContentParser::rowCount() {
	if( ! m_isLoaded )
		try {
			loadFromFile( filename() );
		} catch( FileContentException e ) {
		}
	return FileContentElement::rowCount();
}
