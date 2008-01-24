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
#include <iextendededitor.h>

// Qt header
#include <QDomElement>
#include <QFile>
#include <QIcon>
#include <QFileInfo>
#include <QTextStream>

/* XMLParserException */

XMLParserException::XMLParserException( const QString & message, int line, int column ) : FileContentException( message, line, column ) {
}

/* XSLFileContentParams */

XSLFileContentParams::XSLFileContentParams( FileContentElement * parent, const QDomElement & node )
					 : FileContentElement( parent, node.attribute( "name" ), node.lineNumber() ) {
	XINX_TRACE( "XSLFileContentParams", QString( "( %1, node )" ).arg( (unsigned int)parent, 0, 16 ) );

	m_value = node.attribute( "select", node.text() );
}

XSLFileContentParams::~XSLFileContentParams() {
	XINX_TRACE( "~XSLFileContentParams", "()" );
}

const QString & XSLFileContentParams::value() const {
	XINX_TRACE( "XSLFileContentParams::value", "()" );

	return m_value;
}

QString XSLFileContentParams::displayTips() const {
	XINX_TRACE( "XSLFileContentParams::displayTips", "()" );

	return FileContentElement::displayTips() + tr( "\nValue : %2" ).arg( m_value );
}

void XSLFileContentParams::copyFrom( FileContentElement * element ) {
	XINX_TRACE( "XSLFileContentParams::copyFrom", QString( "( %1 )" ).arg( (unsigned int)element, 0, 16 ) );

	FileContentElement::copyFrom( element );
	if( dynamic_cast<XSLFileContentParams*>( element ) )
		m_value = dynamic_cast<XSLFileContentParams*>( element )->m_value;
}

QIcon XSLFileContentParams::icon() const {
	XINX_TRACE( "XSLFileContentParams::icon", "()" );

	return QIcon( ":/images/html_value.png" );
}

/* XSLFileContentVariable */

XSLFileContentVariable::XSLFileContentVariable( FileContentElement * parent, const QDomElement & node )
					   : XSLFileContentParams( parent, node ) {
	XINX_TRACE( "XSLFileContentVariable", QString( "( %1, node )" ).arg( (unsigned int)parent, 0, 16 ) );
}

XSLFileContentVariable::~XSLFileContentVariable() {
	XINX_TRACE( "~XSLFileContentVariable::icon", "()" );

}

QIcon XSLFileContentVariable::icon() const {
	XINX_TRACE( "XSLFileContentVariable::icon", "()" );

	return QIcon(":/images/variable.png");
}

/* XSLFileContentTemplate */

XSLFileContentTemplate::XSLFileContentTemplate( IXinxExtendedEditor * editor, FileContentElement * parent, const QDomElement & node, const QString & name )
					   : FileContentElement( parent, name, node.lineNumber() ), m_editor( editor ) {
	XINX_TRACE( "XSLFileContentTemplate", QString( "( %1, node )" ).arg( (unsigned int)parent, 0, 16 ) );

	m_mode = node.attribute( "mode" ).trimmed();
}


void XSLFileContentTemplate::loadFromXML( const QDomElement& node ) {
	XINX_TRACE( "XSLFileContentTemplate::loadFromXML", "( node )" );

	markAllDeleted();
	
	QDomNodeList list = node.elementsByTagName( "script" );
	for( int i = 0 ; i < list.count(); i++ ) {
		QDomElement child = list.at( i ).toElement();
		QString src;
		if( !child.attribute( "src" ).isEmpty() ) 
			src = child.attribute( "src" );
		else
			src = "this.js";
		
		FileContentElement * element = m_editor->importModelData( this, src, child.lineNumber() );
		if( child.attribute( "src" ).isEmpty() ) {
			element->setFilename( this->filename() );
			element->setName( "JavaScript" );
		}
		if( element ) {
			element = append( element );
			FileContentParser * parser = dynamic_cast<FileContentParser*>( element );
			if( parser )
				try {
					if( child.attribute( "src" ).isEmpty() ) {
						QString text = child.text();
						parser->loadFromContent( text );
					} else 
						parser->loadFromFileDelayed( src );
				} catch( FileContentException e ) {
				}
		}
	}
	
	list = node.elementsByTagName( "style" );
	for( int i = 0 ; i < list.count(); i++ ) {
		QDomElement child = list.at( i ).toElement();
		QString src = "this.css";
		FileContentElement * element = m_editor->importModelData( this, src, child.lineNumber() );
		element->setFilename( this->filename() );
		element->setName( "CascadingStyleSheet" );
		if( element ) {
			element = append( element );
			FileContentParser * parser = dynamic_cast<FileContentParser*>( element );
			if( parser )
				try {
					parser->loadFromContent( child.text() );
				} catch( FileContentException e ) {
				}
		}
	}

	list = node.elementsByTagName( "link" );
	for( int i = 0 ; i < list.count(); i++ ) {
		QDomElement child = list.at( i ).toElement();
		QString src = child.attribute( "href" );
		FileContentElement * element = m_editor->importModelData( this, src, child.lineNumber() );
		if( element ) {
			element = append( element );
			FileContentParser * parser = dynamic_cast<FileContentParser*>( element );
			if( parser )
				try {
					parser->loadFromFileDelayed( src );
				} catch( FileContentException e ) {
				}
		}
	}

	list = node.elementsByTagName( "variable" );
	for( int i = 0 ; i < list.count(); i++ ) 
		append( new XSLFileContentVariable( this, list.at( i ).toElement() ) );
	
	list = node.elementsByTagName( "param" );
	for( int i = 0 ; i < list.count(); i++ ) 
		append( new XSLFileContentParams( this, list.at( i ).toElement() ) );

	removeMarkedDeleted();
}


const QString & XSLFileContentTemplate::mode() const {
	XINX_TRACE( "XSLFileContentTemplate::mode", "()" );

	return m_mode;
}

QString XSLFileContentTemplate::displayName() const {
	XINX_TRACE( "XSLFileContentTemplate::displayName", "()" );

	if( m_mode.isEmpty() )
		return name();
	else
		return QString( "%1 [%2]" ).arg( name() ).arg( mode() );
}


XSLFileContentTemplate::~XSLFileContentTemplate() {
	XINX_TRACE( "~XSLFileContentTemplate", "()" );
}

bool XSLFileContentTemplate::equals( FileContentElement * element ) {
	//XINX_TRACE( "XSLFileContentTemplate::equals", QString( "( %1 )" ).arg( (unsigned int)element, 0, 16 ) );

	return FileContentElement::equals( element ) 
		&& ( m_mode == dynamic_cast<XSLFileContentTemplate*>( element )->m_mode );
}

void XSLFileContentTemplate::copyFrom( FileContentElement * element ) {
	XINX_TRACE( "XSLFileContentTemplate::copyFrom", QString( "( %1 )" ).arg( (unsigned int)element, 0, 16 ) );

	FileContentElement::copyFrom( element );
	if( dynamic_cast<XSLFileContentTemplate*>( element ) )
		m_mode = dynamic_cast<XSLFileContentTemplate*>( element )->m_mode;
}

QIcon XSLFileContentTemplate::icon() const {
	XINX_TRACE( "XSLFileContentTemplate::icon", "()" );

	return QIcon(":/images/template.png");
}

/* XSLFileContentParser */

XSLFileContentParser::XSLFileContentParser( IXinxExtendedEditor * editor ) : FileContentElement( NULL, QString(), 0 ), m_editor( editor ), m_isLoaded( true ) {
	XINX_TRACE( "XSLFileContentParser", "()" );
}

XSLFileContentParser::XSLFileContentParser( IXinxExtendedEditor * editor, FileContentElement * parent, const QString & filename, int lineNumber ) : FileContentElement( parent, QFileInfo( filename ).fileName(), lineNumber ), m_editor( editor ), m_isLoaded( true ) {
	XINX_TRACE( "XSLFileContentParser", QString( "( %1, %2, %3 )" ).arg( (unsigned int)parent, 0, 16 ).arg( filename ).arg( lineNumber ) );
}


XSLFileContentParser::~XSLFileContentParser() {
	XINX_TRACE( "~XSLFileContentParser", "()" );
}

QIcon XSLFileContentParser::icon() const {
	XINX_TRACE( "XSLFileContentParser::icon", "()" );

	return QIcon(":/images/import.png");
}

void XSLFileContentParser::loadFromFileDelayed( const QString & filename ) {
	XINX_TRACE( "XSLFileContentParser::loadFromFileDelayed", QString( "( %1 )" ).arg( filename ) );

	setFilename( filename );
	setName( QFileInfo( filename ).fileName() );
	
	m_isLoaded = false;
}

bool XSLFileContentParser::isLoaded() {
	return m_isLoaded;
}

void XSLFileContentParser::loadFromFile( const QString & filename ) {
	XINX_TRACE( "XSLFileContentParser::loadFromFile", QString( "( %1 )" ).arg( filename ) );

	setFilename( filename );
	setName( QFileInfo( filename ).fileName() );
	
	QFile file( filename );

	// Open the file
	if (!file.open(QFile::ReadOnly | QFile::Text)) 
		throw XMLParserException( QObject::tr("Cannot read file %1:\n%2.").arg(filename).arg(file.errorString()), 0, 0 );

	QTextStream text( &file );
	QString content = text.readAll();
	loadFromContent( content );
}

void XSLFileContentParser::loadFromContent( const QString & content ) {
	XINX_TRACE( "XSLFileContentParser::loadFromContent", QString( "( %1 )" ).arg( content ) );

	m_isLoaded = true;

	if( content.isEmpty() ) {
		clear();
		return;
	}

	QDomDocument xsl;

	// Load XML Document
	QString errorStr;
	int errorLine = 0;
	int errorColumn = 0;  
	if( xsl.setContent(content, true, &errorStr, &errorLine, &errorColumn) ) {
		QDomElement root = xsl.documentElement();
		if( root.prefix() == "xsl" && root.tagName() == "stylesheet" )	
			loadFromXML( root );
		else 
			throw XMLParserException( QObject::tr("Not a stylesheet content."), 0, 0 );
	} else 
		throw XMLParserException( errorStr, errorLine, errorColumn );
	
}

void XSLFileContentParser::loadFromXML( const QDomElement & element ) {	
	XINX_TRACE( "XSLFileContentParser::loadFromXML", "( element )" );

	markAllDeleted();
	
	QDomElement child = element.firstChildElement();
	while (! child.isNull()) {
		if( child.prefix() != "xsl" ) { 
			child = child.nextSiblingElement();
			continue;
		}
		if( ( child.tagName() == "import" ) || ( child.tagName() == "include" ) ) {
			QString src = child.attribute( "href" );
			FileContentElement * element = m_editor->importModelData( this, src, child.lineNumber() );
			FileContentParser * parser = dynamic_cast<FileContentParser*>( element );
			if( element ) {
				append( element );
				if( ( ! src.isEmpty() ) && parser )
					try {
						parser->loadFromFileDelayed( src );
					} catch( FileContentException e ) {
					}
			}
		} else if( child.tagName() == "variable" ) {
			append( new XSLFileContentVariable( this, child ) );
		} else if( child.tagName() == "param" ) {
			append( new XSLFileContentParams( this, child ) );
		} else if( child.tagName() == "template" ) {
			QStringList list = child.attribute( "name", child.attribute( "match" ) ).split( "|", QString::SkipEmptyParts );
			foreach( QString template_name, list ) {
				XSLFileContentTemplate * template_object = new XSLFileContentTemplate( m_editor, this, child, template_name.trimmed() );
				template_object = dynamic_cast<XSLFileContentTemplate*>( append( template_object ) );
				template_object->loadFromXML( child );
			}
		}
		
		child = child.nextSiblingElement();
	} 

	removeMarkedDeleted();
}

int XSLFileContentParser::rowCount() {
	if( ! m_isLoaded )
		try {
			loadFromFile( filename() );
		} catch( FileContentException e ) {
		}
	return FileContentElement::rowCount();
}
