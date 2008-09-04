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
#include "xsllistview.h"
#include <xinxpluginsloader.h>

// Qt header
#include <QDomElement>
#include <QFile>
#include <QIcon>
#include <QFileInfo>
#include <QTextStream>
#include <QBuffer>
#include <QTextCodec>

/* XMLParserException */

XMLParserException::XMLParserException( const QString & message, int line, int column ) : FileContentException( message, line, column ) {
}

/* XSLFileContentParams */

XSLFileContentParams::XSLFileContentParams( FileContentElement * parent, const QString & name, const QString & value, int line )
				     : FileContentElement( parent, name, line ), m_value( value ) {
}

XSLFileContentParams::XSLFileContentParams( FileContentElement * parent, const QDomElement & node )
					 : FileContentElement( parent, node.attribute( "name" ), node.lineNumber() ) {


	m_value = node.attribute( "select", node.text() );
}

XSLFileContentParams::~XSLFileContentParams() {

}

const QString & XSLFileContentParams::value() const {
	return m_value;
}

QString XSLFileContentParams::displayTips() const {
	return FileContentElement::displayTips() + tr( "\nValue : %2" ).arg( m_value );
}

void XSLFileContentParams::copyFrom( FileContentElement * element ) {
	FileContentElement::copyFrom( element );
	if( qobject_cast<XSLFileContentParams*>( element ) )
		m_value = qobject_cast<XSLFileContentParams*>( element )->m_value;
}

QIcon XSLFileContentParams::icon() const {
	return QIcon( ":/images/html_value.png" );
}

/* XSLFileContentVariable */

XSLFileContentVariable::XSLFileContentVariable( FileContentElement * parent, const QString & name, const QString & value, int line )
					   : XSLFileContentParams( parent, name, value, line ) {

}

XSLFileContentVariable::XSLFileContentVariable( FileContentElement * parent, const QDomElement & node )
					   : XSLFileContentParams( parent, node ) {

}

XSLFileContentVariable::~XSLFileContentVariable() {


}

QIcon XSLFileContentVariable::icon() const {
	return QIcon(":/images/variable.png");
}

/* XSLFileContentTemplate */

XSLFileContentTemplate::XSLFileContentTemplate( FileContentElement * parent, const QString & name, const QString & mode, int line )
					   : FileContentElement( parent, name, line ), m_mode( mode ) {

}


XSLFileContentTemplate::XSLFileContentTemplate( FileContentElement * parent, const QDomElement & node, const QString & name )
					   : FileContentElement( parent, name, node.lineNumber() ) {
	m_mode = node.attribute( "mode" ).trimmed();
}

const QString & XSLFileContentTemplate::mode() const {
	return m_mode;
}

QString XSLFileContentTemplate::displayName() const {
	if( m_mode.isEmpty() )
		return name();
	else
		return QString( "%1 [%2]" ).arg( name() ).arg( mode() );
}


XSLFileContentTemplate::~XSLFileContentTemplate() {

}

bool XSLFileContentTemplate::equals( FileContentElement * element ) {
	return FileContentElement::equals( element )
		&& ( m_mode == qobject_cast<XSLFileContentTemplate*>( element )->m_mode );
}

void XSLFileContentTemplate::copyFrom( FileContentElement * element ) {
	FileContentElement::copyFrom( element );
	if( qobject_cast<XSLFileContentTemplate*>( element ) )
		m_mode = qobject_cast<XSLFileContentTemplate*>( element )->m_mode;
}

QIcon XSLFileContentTemplate::icon() const {
	return QIcon(":/images/template.png");
}

/* XSLFileContentParser */

XSLFileContentParser::XSLFileContentParser() : FileContentElement( NULL, QString(), 0 ), m_isLoaded( true ), m_codec( 0 ) {

}

XSLFileContentParser::XSLFileContentParser( FileContentElement * parent, const QString & filename, int lineNumber ) : FileContentElement( parent, QFileInfo( filename ).fileName(), lineNumber ), m_isLoaded( true ), m_codec( 0 ) {

}


XSLFileContentParser::~XSLFileContentParser() {

}

QIcon XSLFileContentParser::icon() const {
	return QIcon(":/images/import.png");
}

void XSLFileContentParser::loadFromDevice( QIODevice * device ) {
	m_isLoaded = true;

	Parser p( this );
	if(! p.loadFromDevice( device ) )
		throw XMLParserException( p.errorString(), p.lineNumber(), p.columnNumber() );
}

void XSLFileContentParser::loadFromFileDelayed( const QString & filename ) {
	setFilename( filename );
	setName( QFileInfo( filename ).fileName() );

	m_isLoaded = false;
}

bool XSLFileContentParser::isLoaded() {
	return m_isLoaded;
}

void XSLFileContentParser::loadFromFile( const QString & filename ) {
	setFilename( filename );
	setName( QFileInfo( filename ).fileName() );

	FileContentParser::loadFromFile( filename );
}

void XSLFileContentParser::loadFromContent( const QString & content ) {
	if( content.isEmpty() ) {
		FileContentElement::clear();
		return;
	}

	FileContentParser::loadFromContent( content );
}

int XSLFileContentParser::rowCount() {
	if( ! m_isLoaded )
		try {
			loadFromFile( filename() );
		} catch( FileContentException e ) {
		}
	return FileContentElement::rowCount();
}

/* XSLFileContentParser::Parser */

bool XSLFileContentParser::Parser::loadFromDevice( QIODevice * device ) {
	setDevice( device );

	m_parent->markAllDeleted();

	while( ! atEnd() ) {
		readNext();

		if( isStartDocument() ) {
			m_parent->m_codec = QTextCodec::codecForName( documentEncoding().toString().toLatin1() );
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

QString XSLFileContentParser::Parser::readElementText() {
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

void XSLFileContentParser::Parser::readStyleSheet() {
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

void XSLFileContentParser::Parser::readVariable() {
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

void XSLFileContentParser::Parser::readTemplate( QList<struct_xsl_variable> & variables, QList<struct_script> & scripts ) {
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

void XSLFileContentParser::Parser::readTemplate() {
	Q_ASSERT( isStartElement() && QXmlStreamReader::name() == "template" );

	QStringList templates = ( attributes().value( "name" ).isEmpty() ? attributes().value( "match" ).toString() : attributes().value( "name" ).toString() ).split( "|", QString::SkipEmptyParts );
	QString mode = attributes().value( "mode" ).toString();
	int line = lineNumber();

	QList<struct_xsl_variable> variables;
	QList<struct_script> scripts;
	readTemplate( variables, scripts );

	foreach( QString name, templates ) {
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
		foreach( struct_xsl_variable v, variables ) {
			if( v.isParam )
				t->append( new XSLFileContentParams( t, v.name, v.value, v.line ) );
			else
				t->append( new XSLFileContentVariable( t, v.name, v.value, v.line ) );
		}
		t->removeMarkedDeleted();
	}
}

void XSLFileContentParser::Parser::readUnknownElement() {
	Q_ASSERT( isStartElement() );

	while( !atEnd() ) {
		readNext();

		if( isEndElement( ))
			break;

		if( isStartElement() )
			readUnknownElement();
    }
}

/* XslContentElementList */

XslContentElementList::XslContentElementList( FileContentElement * root ) : FileContentElementList( root ) {
	if( root ) {
		connect( root, SIGNAL(aboutToAdd(FileContentElement*,int)), this, SLOT(slotAdd(FileContentElement*,int)) );
		connect( root, SIGNAL(aboutToRemove(FileContentElement*)), this, SLOT(slotRemove(FileContentElement*)) );
	}
	slotReset();
}

XslContentElementList::~XslContentElementList() {

}

void XslContentElementList::setTemplateName( const QString & templateMatchName, const QString & mode ) {
	//qDebug( qPrintable( QString("Name = \"%1\", Mode = \"%2\"").arg( templateMatchName ).arg( mode ) ) );
	if( ( templateMatchName != m_templateMatchName ) || ( m_templateMode != mode ) ) {
		m_templateMatchName = templateMatchName;
		m_templateMode      = mode;
		slotReset();
	}
}

const QString & XslContentElementList::templateMatchName() const {
	return m_templateMatchName;
}

const QString & XslContentElementList::templateMode() const {
	return m_templateMode;
}

QStringList XslContentElementList::modes( QString templateName ) const {
	return QStringList( m_modes.values( templateName ) );
}

QStringList XslContentElementList::params( QString templateName ) const {
	//qDebug( qPrintable( QString( "Name = %1 : %2" ).arg( templateName ).arg( m_params.values( templateName ).size() ) ) );
	return QStringList( m_params.values( templateName ) );
}

bool XslContentElementList::isElementShowed( FileContentElement * element ) {
	Q_ASSERT( element );
	XSLFileContentTemplate * templ = qobject_cast<XSLFileContentTemplate*>( element->parent() );
	if( ! templ ) return true;
	//qDebug( qPrintable( QString( "really name = \"%1\", really mode = \"%2\", asked name = \"%3\", asked mode = \"%4\"" ).arg( templ->name() ).arg( templ->mode() ).arg( m_templateMatchName ).arg( m_templateMode ) ) );
	if( ( templ->name() == m_templateMatchName ) && ( m_templateMode.isEmpty() || ( templ->mode() == m_templateMode ) ) ) return true;
	return false;
}

void XslContentElementList::slotAdd( FileContentElement * element, int row ) {
	Q_UNUSED( row );

	XSLFileContentTemplate * templ = qobject_cast<XSLFileContentTemplate*>( element );
	if( templ ) addElementList( templ );

	XSLFileContentParams * param = qobject_cast<XSLFileContentParams*>( element );
	if( param && ( param->metaObject()->className() == QLatin1String("XSLFileContentParams") ) && ( templ = qobject_cast<XSLFileContentTemplate*>( param->parent() ) ) ) {
		//qDebug( qPrintable( QString( "Name = %1, Mode = %2, Param = %3").arg( templ->name() ).arg( templ->mode() ).arg( param->name() ) ) );
		m_params.insert( templ->name(), param->name() );
	}
}

void XslContentElementList::slotRemove( FileContentElement * element ) {
	XSLFileContentTemplate * templ = qobject_cast<XSLFileContentTemplate*>( element );
	if( templ ) {
		if( ! templ->mode().isEmpty() )
			m_modes.remove( templ->displayName(), templ->mode() );

		for( int i = 0; i < templ->rowCount(); i++ ) {
			XSLFileContentParams * param = qobject_cast<XSLFileContentParams*>( templ->element( i ) );
			if( param && ( param->metaObject()->className() == QLatin1String("XSLFileContentParams") ) )
				m_params.remove( templ->displayName(), param->displayName() );
		}
	}
}

void XslContentElementList::addElementList( XSLFileContentTemplate * templ ) {
	Q_ASSERT( templ );

	//qDebug( qPrintable( QString( "Name = %1, Mode = %2, CountChildren = %3").arg( templ->name() ).arg( templ->mode() ).arg( templ->rowCount() ) ) );

	if( !templ->mode().isEmpty() )
		m_modes.insert( templ->name(), templ->mode() );

	for( int i = 0; i < templ->rowCount(); i++ ) {
		XSLFileContentParams * param = qobject_cast<XSLFileContentParams*>( templ->element( i ) );
		if( param && ( param->metaObject()->className() == QLatin1String("XSLFileContentParams") ) )
			m_params.insert( templ->name(), param->name() );
	}
}

void XslContentElementList::addElementList( XSLFileContentParser * parser ) {
	Q_ASSERT( parser );

	for( int i = 0; i < parser->rowCount(); i++ ) {
		XSLFileContentTemplate * templ = qobject_cast<XSLFileContentTemplate*>( parser->element( i ) );
		if( templ ) addElementList( templ );
		XSLFileContentParser * p = qobject_cast<XSLFileContentParser*>( parser->element( i ) );
		if( p ) addElementList( p );
	}
}

void XslContentElementList::slotReset() {
	m_modes.clear();
	m_params.clear();
	if( qobject_cast<XSLFileContentParser*>( rootElement() ) ) addElementList( qobject_cast<XSLFileContentParser*>( rootElement() ) );
}
