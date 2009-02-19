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
#include "xslcontentviewparser.h"

/* XslContentViewParser */

XslContentViewParser::XslContentViewParser( bool autoDelete ) : ContentViewParser( autoDelete ) {
}

XslContentViewParser::~XslContentViewParser() {
}

void XslContentViewParser::loadFromDeviceImpl( ContentViewNode * rootNode, QIODevice * device ) {
	setDevice( device );
	m_node = rootNode;

	m_node->markAllDeleted();

	while( ! atEnd() ) {
		readNext();

		if( isStartDocument() ) {
			m_node->m_codec = QTextCodec::codecForName( documentEncoding().toString().toLatin1() );
		}

		if( isStartElement() ) {
			if( name() == "stylesheet" )
				readStyleSheet();
			else
				raiseError(tr("The file is not an Stylesheet file."));
		}
	}

	if( !error() ) // Else completion can be more difficulte
		m_parent->removeMarkedDeleted();
	return ! error();
}

QString XslContentViewParser::readElementText() {
	Q_ASSERT( isStartElement() );

	QString result;

	while( ! atEnd() ) {
		QXmlStreamReader::TokenType type = readNext();

		if( isEndElement() ) break;

		switch( type ) {
		case Characters:
		case EntityReference:
			result += text().toString().trimmed();
			break;
		case StartElement:
			result += readElementText();
		default:
			continue;
		}
	}

	return result;
}

void XslContentViewParser::readStyleSheet() {
	Q_ASSERT( isStartElement() && QXmlStreamReader::name() == "stylesheet" );

	while( !atEnd() ) {
		readNext();

		if( isEndElement() )
			break;

		if( isStartElement() ) {
			if( QXmlStreamReader::name() == "param" )
				readVariable();
			else if( QXmlStreamReader::name() == "variable" )
				readVariable();
			else if( QXmlStreamReader::name() == "template" )
				readTemplate();
			else if( ( QXmlStreamReader::name() == "import" ) || ( QXmlStreamReader::name() == "include" ) ) {
				QString src = attributes().value( "href" ).toString();
				FileContentElement * element = XinxPluginsLoader::self()->createElement( src, m_parent, lineNumber() );
				if( element ) {
					element = m_parent->append( element );
					FileContentParser * parser = dynamic_cast<FileContentParser*>( element );
					if( ( ! src.isEmpty() ) && parser )
						parser->loadFromFileDelayed( src );
				} else {
					element = m_parent->append( new FileContentElement( m_parent, src, lineNumber() ) );
				}
				readElementText();
			} else
				readUnknownElement();
		}
	}
}

void XslContentViewParser::readVariable() {
	Q_ASSERT( isStartElement() && ( ( QXmlStreamReader::name() == "param" ) || ( QXmlStreamReader::name() == "variable" ) ) );

	QString name  = attributes().value( "name" ).toString();
	QString value;
	if( attributes().value( "select" ).isEmpty() ) {
		value = readElementText();
	} else {
		value = attributes().value( "select" ).toString();
		readUnknownElement();
	}
	if( name == "param" )
		m_parent->append( new XSLFileContentParams( m_parent, name, value, lineNumber() ) );
	else
		m_parent->append( new XSLFileContentVariable( m_parent, name, value, lineNumber() ) );
}

void XslContentViewParser::readTemplate( QList<struct_xsl_variable> & variables, QList<struct_script> & scripts ) {
	Q_ASSERT( isStartElement() );

	while( !atEnd() ) {
		readNext();

		if( isEndElement( ))
			break;

		if( isStartElement() ) {
			if( ( QXmlStreamReader::name() == "param" ) || ( QXmlStreamReader::name() == "variable" ) ) {
				struct struct_xsl_variable v;
				v.isParam = QXmlStreamReader::name() == "param";
				v.line  = lineNumber();
				v.name  = attributes().value( "name" ).toString();
				if( attributes().value( "select" ).isEmpty() ) {
					v.value = readElementText();
				} else {
					v.value = attributes().value( "select" ).toString();
					readUnknownElement();
				}
				variables += v;
			} else
				if( QXmlStreamReader::name() == "script" ) {
					struct struct_script s;
					s.isSrc = !attributes().value( "src" ).isEmpty();
					s.line = lineNumber();
					s.title = "JavaScript";
					if( s.isSrc ) {
						s.src = attributes().value( "src" ).toString();
						readUnknownElement();
					} else {
						s.src = "this.js";
						s.content = readElementText();
					}
					scripts += s;
				} else
					if( QXmlStreamReader::name() == "link" ) {
						struct struct_script s;
						s.isSrc = true;
						s.line = lineNumber();
						s.title = "Cascading StyleSheet";
						s.src = attributes().value( "href" ).toString();
						readUnknownElement();
						scripts += s;
					} else
						if( QXmlStreamReader::name() == "style" ) {
							struct struct_script s;
							s.isSrc = false;
							s.line = lineNumber();
							s.title = "Cascading StyleSheet";
							s.src = "this.css";
							s.content = readElementText();
							scripts += s;
						} else
							readTemplate( variables, scripts );
		}
	}
}

void XslContentViewParser::readTemplate() {
	Q_ASSERT( isStartElement() && QXmlStreamReader::name() == "template" );

	QStringList templates = ( attributes().value( "name" ).isEmpty() ? attributes().value( "match" ).toString() : attributes().value( "name" ).toString() ).split( "|", QString::SkipEmptyParts );
	QString mode = attributes().value( "mode" ).toString();
	int line = lineNumber();

	QList<struct_xsl_variable> variables;
	QList<struct_script> scripts;
	readTemplate( variables, scripts );

	foreach( const QString & name, templates ) {
		XSLFileContentTemplate * t = qobject_cast<XSLFileContentTemplate*>( m_parent->append( new XSLFileContentTemplate( m_parent, name, mode, line ) ) );
		if( ! t ) {
			raiseError( tr("Can't insert %1 template").arg( name ) );
			break;
		}

		t->markAllDeleted();
		/* Chargement des scripts */
		foreach( struct_script s, scripts ) {
			FileContentElement * element = XinxPluginsLoader::self()->createElement( s.src, m_parent, s.line );
			if( ! s.isSrc ) {
				element->setFilename( t->filename() );
				element->setName( s.title );
			}
			if( element ) {
				element = t->append( element );
				FileContentParser * parser = dynamic_cast<FileContentParser*>( element );
				if( parser )
					try {
						if( ! s.isSrc )
							parser->loadFromContent( s.content );
						else
							parser->loadFromFileDelayed( s.src );
					} catch( FileContentException e ) {
					}
			} else {
				element = m_parent->append( new FileContentElement( m_parent, s.src, lineNumber() ) );
			}
		}

		/* Chargement des variables et params */
		foreach( const struct_xsl_variable & v, variables ) {
			if( v.isParam )
				t->append( new XSLFileContentParams( t, v.name, v.value, v.line ) );
			else
				t->append( new XSLFileContentVariable( t, v.name, v.value, v.line ) );
		}
		t->removeMarkedDeleted();
	}
}

void XslContentViewParser::readUnknownElement() {
	Q_ASSERT( isStartElement() );

	while( !atEnd() ) {
		readNext();

		if( isEndElement( ))
			break;

		if( isStartElement() )
			readUnknownElement();
	}
}
