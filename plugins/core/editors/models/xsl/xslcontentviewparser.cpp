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
#include <plugins/plugininterfaces.h>
#include <plugins/xinxpluginsloader.h>
#include <core/xinxcore.h>

// Qt header
#include <QtConcurrentRun>
#include <QTextCodec>
#include <QIcon>
#include <QVariant>
#include <QQueue>

/* Static member */

XmlCompletionParser * XmlCompletionParser::s_self = 0;

/* XslContentViewParser */

XslContentViewParser::XslContentViewParser( bool autoDelete ) : ContentViewParser( autoDelete ), m_codec( 0 ) {
}

XslContentViewParser::~XslContentViewParser() {
}

void XslContentViewParser::loadFromDeviceImpl() {
	setDevice( inputDevice() );

	if( rootNode() )
		rootNode()->setData( ":/images/typexsl.png", ContentViewNode::NODE_ICON );

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
	} else
			throw ContentViewException( errorString(), filename(), lineNumber(), columnNumber() );
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

	if( rootNode() ) {
		if( QXmlStreamReader::name() == "param" )
			attacheNewParamsNode( rootNode(), name.trimmed(), value, lineNumber() );
		else
			attacheNewVariableNode( rootNode(), name.trimmed(), value, lineNumber() );
	}
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
					s.src = ( rootNode() ? rootNode()->fileName() : filename() ) + ".js";
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
				s.src = ( rootNode() ? rootNode()->fileName() : filename() ) + ".css";
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
		ContentViewNode * t = 0;
		if( rootNode() )
			t = attacheNewTemplateNode( rootNode(), name.trimmed(), mode, line );

		loadAttachedNode( t );
		/* Chargement des scripts */
		foreach( struct_script s, scripts ) {
			// Si JavaScript externe
			if( s.isSrc ) {
				createContentViewNode( t, s.src );
			} else if( t ) { // C'est un Javascript interne
				ContentViewNode * node = new ContentViewNode( s.title, s.line );
				node->setData( "internal", ContentViewNode::NODE_TYPE );
				node->setData( s.title, ContentViewNode::NODE_DISPLAY_NAME );
				node->setData( tr( "Element at line : %1" ).arg( s.line ), ContentViewNode::NODE_DISPLAY_TIPS );
				node->setData( ":/images/import.png", ContentViewNode::NODE_ICON );

				// Il nous faut un parser JavaScript
				IFileTypePlugin * ft = XinxPluginsLoader::self()->matchedFileType( s.src );
				ContentViewParser * parser = 0;
				if( ft && ( parser = ft->createParser() ) ) {
					try {
						node->setData( ft->icon(), ContentViewNode::NODE_ICON );

						node = attachNode( t, node );

						parser->setDecalage( s.line );
						parser->setAttachId( (unsigned long)rootNode() );

						QByteArray textToParse = qPrintable( s.content );
						if( codec() )
							textToParse = codec()->fromUnicode( s.content );
						parser->loadFromContent( node, textToParse );
					} catch( ContentViewException e ) {
					}
					delete parser;
				} else
					node = attachNode( t, node );
			}
		}

		if( t ) {
			/* Chargement des variables et params */
			foreach( const struct_xsl_variable & v, variables ) {
				if( v.isParam )
					attacheNewParamsNode( t, v.name.trimmed(), v.value.trimmed(), v.line );
				else
					attacheNewVariableNode( t, v.name.trimmed(), v.value.trimmed(), v.line );
			}
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
	node->setData( ":/images/template.png", ContentViewNode::NODE_ICON );
	node->setData( displayName, ContentViewNode::NODE_DISPLAY_NAME );
	node->setData( tr( "Element at line : %1\nMode = %2" ).arg( line ).arg( mode ), ContentViewNode::NODE_DISPLAY_TIPS );
	node->setData( mode, ContentViewNode::NODE_USER_VALUE );

	node = attachNode( parent, node );

	return node;
}

ContentViewNode * XslContentViewParser::attacheNewParamsNode( ContentViewNode * parent, const QString & name, const QString & value,  int line ) {
	QString displayName = name;
	if( ! value.isEmpty() )
		displayName += "=" + value;

	ContentViewNode * node = new ContentViewNode( name, line );
	node->setData( "XslParam", ContentViewNode::NODE_TYPE );
	node->setData( ":/images/html_value.png", ContentViewNode::NODE_ICON );
	node->setData( displayName, ContentViewNode::NODE_DISPLAY_NAME );
	node->setData( tr( "Element at line : %1\nValue = %2" ).arg( line ).arg( value ), ContentViewNode::NODE_DISPLAY_TIPS );
	node->setData( value, ContentViewNode::NODE_USER_VALUE );

	node = attachNode( parent, node );

	return node;
}

ContentViewNode * XslContentViewParser::attacheNewVariableNode( ContentViewNode * parent, const QString & name, const QString & value, int line ) {
	QString displayName = name;
	if( ! value.isEmpty() )
		displayName += "=" + value;

	ContentViewNode * node = new ContentViewNode( name, line );
	node->setData( "XslVariable", ContentViewNode::NODE_TYPE );
	node->setData( ":/images/variable.png", ContentViewNode::NODE_ICON );
	node->setData( displayName, ContentViewNode::NODE_DISPLAY_NAME );
	node->setData( tr( "Element at line : %1\nValue = %2" ).arg( line ).arg( value ), ContentViewNode::NODE_DISPLAY_TIPS );
	node->setData( value, ContentViewNode::NODE_USER_VALUE );

	node = attachNode( parent, node );

	return node;
}

/* XmlCompletionParser */

XmlCompletionParser::XmlCompletionParser() : ContentViewParser( false ), m_codec( 0 ) {
	ContentViewNode * node = new ContentViewNode( "XmlRoot", -1 );
	node->setAutoDelete( false );
	setRootNode( node );
}

XmlCompletionParser::~XmlCompletionParser() {
	rootNode()->setAutoDelete( true );
}

XmlCompletionParser * XmlCompletionParser::self() {
	if( ! s_self ) {
		s_self = new XmlCompletionParser();
		try {
			s_self->setFilename( "datas:baseplugin_xml.xml" );
			QFuture<void> future = QtConcurrent::run( (ContentViewParser*)s_self, &ContentViewParser::loadFromMember );
		} catch( ContentViewException e ) {
			qWarning( qPrintable( e.getMessage() ) );

		}
		XINXStaticDeleter::self()->add( s_self );
	}
	return s_self;
}

ContentViewNode * XmlCompletionParser::rootNode() const {
	return ContentViewParser::rootNode();
}

void XmlCompletionParser::loadFromDeviceImpl() {
	Q_ASSERT( rootNode() );

	m_index = 0;
	setDevice( inputDevice() );

	loadAttachedNode( rootNode() );
	m_parentNode.clear();

	while( ! atEnd() ) {
		readNext();

		if( isStartDocument() ) {
			m_codec = QTextCodec::codecForName( documentEncoding().toString().toLatin1() );
		}

		if( isStartElement() ) {
			if( name() == "xml" )
				readRootTag();
			else
				raiseError(tr("The file is not a completion file."));
		}
	}

	if( !error() ) { // Else completion can be more difficulte
		detachAttachedNode();
	} else
		throw ContentViewException( errorString(), rootNode()->fileName(), lineNumber(), columnNumber() );
}

QString XmlCompletionParser::readElementText() {
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

void XmlCompletionParser::readRootTag() {
	Q_ASSERT( isStartElement() && QXmlStreamReader::name() == "xml" );

	m_parentNode.push( rootNode() );

	while( !atEnd() ) {
		readNext();

		if( isEndElement() )
			break;

		if( isStartElement() ) {
			if( QXmlStreamReader::name() == "type" )
				readTypeTag();
			else
				raiseError( tr("Wrong node name %1 in root element").arg( QXmlStreamReader::name().toString() ) );
		}
	}

	m_parentNode.pop();
}

void XmlCompletionParser::readTypeTag() {
	Q_ASSERT( isStartElement() && QXmlStreamReader::name() == "type" );

	m_type = attributes().value( "name" ).toString();

	while( !atEnd() ) {
		readNext();

		if( isEndElement() )
			break;

		if( isStartElement() ) {
			if( QXmlStreamReader::name() == "balise" )
				readBaliseTag();
			else
				raiseError( tr("Wrong node name %1 in type element").arg( QXmlStreamReader::name().toString() ) );
		}
	}
}

void XmlCompletionParser::readBaliseTag() {
	Q_ASSERT( isStartElement() && QXmlStreamReader::name() == "balise" );

	QString
		name = attributes().value( "name" ).toString(),
		defaultValue = attributes().value( "default" ).toString();

	ContentViewNode * node = new ContentViewNode( name, lineNumber() );
	node->setData( "XmlBalise", ContentViewNode::NODE_TYPE );
	node->setData( ":/images/balise.png", ContentViewNode::NODE_ICON );
	node->setData( name, ContentViewNode::NODE_DISPLAY_NAME );
	node->setData( m_type, XmlCompletionParser::NODE_XML_TYPE );
	node->setData( defaultValue == "true" ? true : false, XmlCompletionParser::NODE_XML_ISDEFAULT );
	node->setData( (uint)++m_index, XmlCompletionParser::NODE_XML_SORT_INDEX );

	node = attachNode( m_parentNode.top(), node );

	m_parentNode.push( node );
	while( !atEnd() ) {
		readNext();

		if( isEndElement() )
			break;

		if( isStartElement() ) {
			if( QXmlStreamReader::name() == "balise" )
				readBaliseTag();
			else
				if( QXmlStreamReader::name() == "attribute" )
					readAttributeTag();
			else
				raiseError( tr("Wrong node name %1 in balise element").arg( QXmlStreamReader::name().toString() ) );
		}
	}
	m_parentNode.pop();
}

void XmlCompletionParser::readAttributeTag() {
	Q_ASSERT( isStartElement() && QXmlStreamReader::name() == "attribute" );

	QString
		name = attributes().value( "name" ).toString(),
		defaultValue = attributes().value( "default" ).toString();

	ContentViewNode * node = new ContentViewNode( name, lineNumber() );
	node->setData( "XmlAttribute", ContentViewNode::NODE_TYPE );
	node->setData( ":/images/noeud.png", ContentViewNode::NODE_ICON );
	node->setData( name, ContentViewNode::NODE_DISPLAY_NAME );

	node->setData( m_type, XmlCompletionParser::NODE_XML_TYPE );
	node->setData( defaultValue == "true" ? true : false, XmlCompletionParser::NODE_XML_ISDEFAULT );

	node->setData( (uint)++m_index, XmlCompletionParser::NODE_XML_SORT_INDEX );

	node = attachNode( m_parentNode.top(), node );

	m_parentNode.push( node );
	while( !atEnd() ) {
		readNext();

		if( isEndElement() )
			break;

		if( isStartElement() ) {
			if( QXmlStreamReader::name() == "value" )
				readValueTag();
			else
				raiseError( tr("Wrong node name %1 in attribute element").arg( QXmlStreamReader::name().toString() ) );
		}
	}
	m_parentNode.pop();
}

void XmlCompletionParser::readValueTag() {
	Q_ASSERT( isStartElement() && QXmlStreamReader::name() == "value" );

	QString name, defaultValue = attributes().value( "default" ).toString();
	name = readElementText();

	ContentViewNode * node = new ContentViewNode( name, lineNumber() );
	node->setData( "XmlValue", ContentViewNode::NODE_TYPE );
	node->setData( ":/images/html_value.png", ContentViewNode::NODE_ICON );
	node->setData( name, ContentViewNode::NODE_DISPLAY_NAME );

	node->setData( m_type, XmlCompletionParser::NODE_XML_TYPE );
	node->setData( defaultValue == "true" ? true : false, XmlCompletionParser::NODE_XML_ISDEFAULT );

	node = attachNode( m_parentNode.top(), node );
}

ContentViewNode * XmlCompletionParser::balise( const QString & name ) const {
	QQueue<ContentViewNode*> list;
	list.enqueue( rootNode() );

	while( list.size() ) {
		ContentViewNode * parent = list.dequeue();

		foreach( ContentViewNode * n, parent->childs() ) {
			if( n->data( ContentViewNode::NODE_TYPE ).toString() == "XmlBalise" ) {
				if( n->data( ContentViewNode::NODE_NAME ).toString() == name ) {
					return n;
				}
				list.enqueue( n );
			}
		}
	}

	return 0;
}

bool XmlCompletionBaliseSort( ContentViewNode * node1, ContentViewNode * node2 ) {
	return node1->data( XmlCompletionParser::NODE_XML_SORT_INDEX ).toInt() < node2->data( XmlCompletionParser::NODE_XML_SORT_INDEX ).toInt();
}

QList<ContentViewNode*> XmlCompletionParser::balises() const {
	QList<ContentViewNode*> result;
	foreach( ContentViewNode * n, rootNode()->childs() ) {
		if( n->data( ContentViewNode::NODE_TYPE ).toString() == "XmlBalise" ) {
			result += n;
		}
	}
	qSort( result.begin(), result.end(), XmlCompletionBaliseSort );
	return result;
}

ContentViewNode * XmlCompletionParser::baliseAttribute( const QString & name, const QString & attribute ) const {
	ContentViewNode * b = balise( name );
	if( !b ) return 0;

	foreach( ContentViewNode * n, b->childs() ) {
		if( ( n->data( ContentViewNode::NODE_TYPE ).toString() == "XmlAttribute" ) && ( n->data( ContentViewNode::NODE_NAME ).toString() == attribute ) ) {
			return n;
		}
	}
	return 0;
}

QList<ContentViewNode*> XmlCompletionParser::baliseAttributes( const QString & name ) const {
	ContentViewNode * b = balise( name );
	if( !b ) return QList<ContentViewNode*>();

	QList<ContentViewNode*> result;
	foreach( ContentViewNode * n, b->childs() ) {
		if( n->data( ContentViewNode::NODE_TYPE ).toString() == "XmlAttribute" ) {
			result += n;
		}
	}
	qSort( result.begin(), result.end(), XmlCompletionBaliseSort );
	return result;
}

ContentViewNode * XmlCompletionParser::baliseOfBalise( const QString & name, const QString & baliseName ) const {
	ContentViewNode * b = balise( name );
	if( !b ) return 0;

	foreach( ContentViewNode * n, b->childs() ) {
		if( ( n->data( ContentViewNode::NODE_TYPE ).toString() == "XmlBalise" ) && ( n->data( ContentViewNode::NODE_NAME ).toString() == baliseName ) ) {
			return n;
		}
	}
	return 0;
}

ContentViewNode * XmlCompletionParser::baliseAttributeValue( const QString & name, const QString & attribute, const QString & value ) const {
	ContentViewNode * a = baliseAttribute( name, attribute );
	if( !a ) return 0;

	foreach( ContentViewNode * n, a->childs() ) {
		if( ( n->data( ContentViewNode::NODE_TYPE ).toString() == "XmlValue" ) && ( n->data( ContentViewNode::NODE_NAME ).toString() == value ) ) {
			return n;
		}
	}
	return 0;
}

QList<ContentViewNode*> XmlCompletionParser::baliseAttributeValues( const QString & name, const QString & attribute ) const {
	ContentViewNode * a = baliseAttribute( name, attribute );
	if( !a ) return QList<ContentViewNode*>();

	QList<ContentViewNode*> result;
	foreach( ContentViewNode * n, a->childs() ) {
		if( n->data( ContentViewNode::NODE_TYPE ).toString() == "XmlValue" ) {
			result += n;
		}
	}
	qSort( result.begin(), result.end(), XmlCompletionBaliseSort );
	return result;
}

ContentViewNode * XmlCompletionParser::defaultValue( ContentViewNode * node ) {
	ContentViewNode * def = 0;
	foreach( ContentViewNode * n, node->childs() ) {
		if( ( n->data( ContentViewNode::NODE_TYPE ).toString() == "XmlValue" ) && ( n->data( XmlCompletionParser::NODE_XML_ISDEFAULT ).toBool() ) )
			def = n;
	}
	return def;
}

QList<ContentViewNode*> XmlCompletionParser::defaultAttributes( ContentViewNode * node ) {
	QList<ContentViewNode *> defs;
	foreach( ContentViewNode * n, node->childs() ) {
		if( ( n->data( ContentViewNode::NODE_TYPE ).toString() == "XmlAttribute" ) && ( n->data( XmlCompletionParser::NODE_XML_ISDEFAULT ).toBool() ) )
			defs += n;
	}
	qSort( defs.begin(), defs.end(), XmlCompletionBaliseSort );
	return defs;
}

QList<ContentViewNode*> XmlCompletionParser::defaultBalises( ContentViewNode * node ) {
	QList<ContentViewNode *> defs;
	foreach( ContentViewNode * n, node->childs() ) {
		if( ( n->data( ContentViewNode::NODE_TYPE ).toString() == "XmlBalise" ) && ( n->data( XmlCompletionParser::NODE_XML_ISDEFAULT ).toBool() ) )
			defs += n;
	}
	qSort( defs.begin(), defs.end(), XmlCompletionBaliseSort );
	return defs;
}
