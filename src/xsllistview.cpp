/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
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
#include "globals.h"
#include "xsllistview.h"
#include "xslproject.h"

// Qt header
#include <QDebug>
#include <QDomElement>
#include <QFile>
#include <QMessageBox>
#include <QApplication>
#include <QIcon>
#include <QDir>
#include <QFileInfo>

/* XMLParserException */

XMLParserException::XMLParserException( const QString & message, int line, int column ) : FileContentException( message, line, column ) {
}

/* XSLFileContentImportJS */

XSLFileContentImportJS::XSLFileContentImportJS( FileContentElement * parent, const QString & filename, const QDomElement & node ) : JavaScriptParser( parent, filename, node.lineNumber() ) {
	setFilename( filename );
}

XSLFileContentImportJS::~XSLFileContentImportJS() {
	
}
	
XSLFileContentImportJS * XSLFileContentImportJS::createImportFromLocation( FileContentElement * parent, const QDomElement & node ) {
	QStringList searchList;
	
	if( global.m_project ) 
		searchList += global.m_project->processedSearchPathList(); 

	QString name = node.attribute( "src" );
	foreach( QString path, searchList ) {
		if( QFile::exists( QDir( path ).absoluteFilePath( name ) ) )
			return new XSLFileContentImportJS( parent, QDir( path ).absoluteFilePath( name ), node );
	}
	
	return NULL;
}

int XSLFileContentImportJS::rowCount() {
	if( JavaScriptParser::rowCount() == 0 ) {
		setFlagEmit( false );
		try {
			loadFromFile( filename() );
		} catch( ... ) {
		}
		setFlagEmit( true );
	}
	return JavaScriptParser::rowCount();
}

/* XSLFileContentImport */

XSLFileContentImport::XSLFileContentImport( FileContentElement * parent, const QString & filename, const QDomElement & node ) : XSLFileContentParser( parent, filename, node.lineNumber() ) {
	setFilename( filename );
	m_loadFlag = false;
}

XSLFileContentImport::~XSLFileContentImport() {

}

XSLFileContentImport * XSLFileContentImport::createImportFromLocation( FileContentElement * parent, const QDomElement & node ) {
	QStringList searchList;
	if( ! parent->filename().isEmpty() )
		searchList << QFileInfo( parent->filename() ).absolutePath();
	
	if( global.m_project ) 
		searchList += global.m_project->processedSearchPathList(); 

	QString name = node.attribute( "href" );
	foreach( QString path, searchList ) {
		if( QFile::exists( QDir( path ).absoluteFilePath( name ) ) )
			return new XSLFileContentImport( parent, QDir( path ).absoluteFilePath( name ), node );
	}
	
	return NULL;
}

int XSLFileContentImport::rowCount() {
	if( ! m_loadFlag ) {
		setFlagEmit( false );
		try {
			m_loadFlag = true;
			loadFromFile( filename() );
		} catch( ... ) {
		}
		setFlagEmit( true );
	}
	return XSLFileContentParser::rowCount();
}

/* PrivateXSLFileContentParams */

class PrivateXSLFileContentParams {
public:
	QString m_value;
};

/* XSLFileContentParams */

XSLFileContentParams::XSLFileContentParams( FileContentElement * parent, const QDomElement & node )
					 : FileContentElement( parent, node.attribute( "name" ), node.lineNumber() ) {
	d = new PrivateXSLFileContentParams();
	d->m_value = node.attribute( "select", node.text() );
}

XSLFileContentParams::~XSLFileContentParams() {
	delete d;
}

const QString & XSLFileContentParams::value() const {
	return d->m_value;
}

QString XSLFileContentParams::displayTips() const {
	return FileContentElement::displayTips() + tr( "\nValue : %2" ).arg( d->m_value );
}

void XSLFileContentParams::copyFrom( FileContentElement * element ) {
	FileContentElement::copyFrom( element );
	if( dynamic_cast<XSLFileContentParams*>( element ) )
		d->m_value = dynamic_cast<XSLFileContentParams*>( element )->d->m_value;
}

QIcon XSLFileContentParams::icon() const {
	return QIcon( ":/images/html_value.png" );
}

/* XSLFileContentVariable */

XSLFileContentVariable::XSLFileContentVariable( FileContentElement * parent, const QDomElement & node )
					   : XSLFileContentParams( parent, node ) {
}

XSLFileContentVariable::~XSLFileContentVariable() {
	
}

QIcon XSLFileContentVariable::icon() const {
	return QIcon(":/images/variable.png");
}


/* PrivateXSLFileContentTemplate */

class PrivateXSLFileContentTemplate {
public:
	QString m_mode;
};

/* XSLFileContentTemplate */

XSLFileContentTemplate::XSLFileContentTemplate( FileContentElement * parent, const QDomElement & node, const QString & name )
					   : FileContentElement( parent, name, node.lineNumber() ) {
	d = new PrivateXSLFileContentTemplate();
	d->m_mode = node.attribute( "mode" ).trimmed();
}


void XSLFileContentTemplate::loadFromXML( const QDomElement& node ) {
	markAllDeleted();
	
	QDomNodeList list = node.elementsByTagName( "script" );
	for( int i = 0 ; i < list.count(); i++ ) {
		QDomElement child = list.at( i ).toElement();
		if( child.attribute( "type" ).toLower().contains( "javascript" ) && (!child.attribute( "src" ).isEmpty()) ) {
			FileContentElement * element = XSLFileContentImportJS::createImportFromLocation( this, child );
			if( element )
				append( element );
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
	return d->m_mode;
}

QString XSLFileContentTemplate::displayName() const {
	if( d->m_mode.isEmpty() )
		return name();
	else
		return QString( "%1 [%2]" ).arg( name() ).arg( mode() );
}


XSLFileContentTemplate::~XSLFileContentTemplate() {
	delete d;
}

bool XSLFileContentTemplate::equals( FileContentElement * element ) {
	QMutexLocker l( &locker() );
	QMutexLocker l2( &element->locker() );
	
	return FileContentElement::equals( element ) 
		&& ( d->m_mode == dynamic_cast<XSLFileContentTemplate*>( element )->d->m_mode );
}

void XSLFileContentTemplate::copyFrom( FileContentElement * element ) {
	QMutexLocker l( &locker() );
	QMutexLocker l2( &(element->locker()) );

	FileContentElement::copyFrom( element );
	if( dynamic_cast<XSLFileContentTemplate*>( element ) )
		d->m_mode = dynamic_cast<XSLFileContentTemplate*>( element )->d->m_mode;
}

QIcon XSLFileContentTemplate::icon() const {
	return QIcon(":/images/template.png");
}

/* XSLFileContentParser */

XSLFileContentParser::XSLFileContentParser() : FileContentElement( NULL, QString(), 0 ) {
}

XSLFileContentParser::XSLFileContentParser( FileContentElement * parent, const QString & filename, int lineNumber ) : FileContentElement( parent, QFileInfo( filename ).fileName(), lineNumber ) {

}


XSLFileContentParser::~XSLFileContentParser() {

}

QIcon XSLFileContentParser::icon() const {
	return QIcon(":/images/import.png");
}

void XSLFileContentParser::loadFromFile( const QString & filename ) {
	QFile file( filename );
	QDomDocument xsl;

	// Open the file
	if (!file.open(QFile::ReadOnly | QFile::Text)) 
		throw XMLParserException( QObject::tr("Cannot read file %1:\n%2.").arg(filename).arg(file.errorString()), 0, 0 );

	QTextStream text( &file );
	QString content = text.readAll();
	loadFromContent( content );
}

void XSLFileContentParser::loadFromContent( const QString & content ) {
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
	markAllDeleted();
	
	QDomElement child = element.firstChildElement();
	while (! child.isNull()) {
		if( child.prefix() != "xsl" ) { 
			child = child.nextSiblingElement();
			continue;
		}
		
		if( ( child.tagName() == "import" ) || ( child.tagName() == "include" ) ) 
			append( XSLFileContentImport::createImportFromLocation( this, child ) );
		else if( child.tagName() == "variable" ) 
			append( new XSLFileContentVariable( this, child ) );
		else if( child.tagName() == "params" )
			append( new XSLFileContentParams( this, child ) );
		else if( child.tagName() == "template" ) {
			QStringList list = child.attribute( "name", child.attribute( "match" ) ).split( "|", QString::SkipEmptyParts );
			foreach( QString template_name, list ) {
				XSLFileContentTemplate * template_object = new XSLFileContentTemplate( this, child, template_name.trimmed() );
				template_object = dynamic_cast<XSLFileContentTemplate*>( append( template_object ) );
				template_object->loadFromXML( child );
			}
		}
		
		child = child.nextSiblingElement();
	} 

	removeMarkedDeleted();
}

