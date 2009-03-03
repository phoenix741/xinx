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
#include <contentview/contentviewnode.h>

// Qt header
#include <QTextCodec>
#include <QIcon>
#include <QVariant>

/* XslContentViewParser */

XslContentViewParser::XslContentViewParser( bool autoDelete ) : ContentViewParser( autoDelete ), m_codec( 0 ) {
}

XslContentViewParser::~XslContentViewParser() {
}

void XslContentViewParser::loadFromDeviceImpl() {
	setDevice( inputDevice() );

	rootNode()->setData( QImage(":/images/typexsl.png"), ContentViewNode::NODE_ICON );

	loadAttachedNode( rootNode() );

	while( ! atEnd() ) {
		readNext();

		if( isStartDocument() ) {
			m_codec = QTextCodec::codecForName( documentEncoding().toString().toLatin1() );
		}

		if( isStartElement() ) {
			if( name() == "stylesheet" )
				readStyleSheet();
			else
				raiseError(tr("The file is not an Stylesheet file."));
		}
	}

	if( !error() ) { // Else completion can be more difficulte
		detachAttachedNode();
	}

	if( error() )
	    throw ContentViewException( errorString(), lineNumber(), columnNumber() );
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
				createContentViewNode( rootNode(), src );
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
		attacheNewParamsNode( rootNode(), name, value, lineNumber() );
	else
		attacheNewVariableNode( rootNode(), name, value, lineNumber() );
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
			} else if( QXmlStreamReader::name() == "script" ) {
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
			} else if( QXmlStreamReader::name() == "link" ) {
				struct struct_script s;
				s.isSrc = true;
				s.line = lineNumber();
				s.title = "Cascading StyleSheet";
				s.src = attributes().value( "href" ).toString();
				readUnknownElement();
				scripts += s;
			} else if( QXmlStreamReader::name() == "style" ) {
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
		ContentViewNode * t = attacheNewTemplateNode( rootNode(), name, mode, line );

		loadAttachedNode( t );
		/* Chargement des scripts */
		foreach( struct_script s, scripts ) {
			/*createContentViewNode( m_parent, s.src );

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
			*/
		}

		/* Chargement des variables et params */
		foreach( const struct_xsl_variable & v, variables ) {
			if( v.isParam )
				attacheNewParamsNode( t, v.name, v.value, v.line );
			else
				attacheNewVariableNode( t, v.name, v.value, v.line );
		}
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

ContentViewNode * XslContentViewParser::attacheNewTemplateNode( ContentViewNode * parent, const QString & name, const QString & mode, int line ) {
	QString displayName = name;
	if( ! mode.isEmpty() )
		displayName += " [" + mode + "]";

	ContentViewNode * node = new ContentViewNode( name, line );
	node->setData( "XslTemplate", ContentViewNode::NODE_TYPE );
	node->setData( QImage(":/images/template.png"), ContentViewNode::NODE_ICON );
	node->setData( displayName, ContentViewNode::NODE_DISPLAY_NAME );
	node->setData( tr( "Element at line : %1\nMode = %2" ).arg( line ).arg( mode ), ContentViewNode::NODE_DISPLAY_TIPS );
	node->attach( parent );

	return node;
}

ContentViewNode * XslContentViewParser::attacheNewParamsNode( ContentViewNode * parent, const QString & name, const QString & value,  int line ) {
	QString displayName = name;
	if( ! value.isEmpty() )
		displayName += "=" + value;

	ContentViewNode * node = new ContentViewNode( name, line );
	node->setData( "XslParam", ContentViewNode::NODE_TYPE );
	node->setData( QImage(":/images/html_value.png"), ContentViewNode::NODE_ICON );
	node->setData( displayName, ContentViewNode::NODE_DISPLAY_NAME );
	node->setData( tr( "Element at line : %1\nValue = %2" ).arg( line ).arg( value ), ContentViewNode::NODE_DISPLAY_TIPS );
	node->attach( parent );

	return node;
}

ContentViewNode * XslContentViewParser::attacheNewVariableNode( ContentViewNode * parent, const QString & name, const QString & value, int line ) {
	QString displayName = name;
	if( ! value.isEmpty() )
		displayName += "=" + value;

	ContentViewNode * node = new ContentViewNode( name, line );
	node->setData( "XslVariable", ContentViewNode::NODE_TYPE );
	node->setData( QImage(":/images/variable.png"), ContentViewNode::NODE_ICON );
	node->setData( displayName, ContentViewNode::NODE_DISPLAY_NAME );
	node->setData( tr( "Element at line : %1\nValue = %2" ).arg( line ).arg( value ), ContentViewNode::NODE_DISPLAY_TIPS );
	node->attach( parent );

	return node;
}

