/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
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
 
#include "xslproject.h"

#include <QtGui>
#include <QtXml>

#include "webservices.h"

XSLProject::XSLProject() : QObject() {
	QDomElement root = m_projectDocument.createElement( "XSLProject" );
	m_projectDocument.appendChild( root );
	m_webServicesModel = NULL;
}

XSLProject::XSLProject( const XSLProject & object ) : QObject() {
	m_fileName =  object.m_fileName;
	m_projectDocument = object.m_projectDocument;
	m_webServicesModel = NULL;
}

XSLProject::XSLProject( const QString & project ) : QObject() {
	m_webServicesModel = NULL;
	loadFromFile(project);
}

XSLProject::~XSLProject() {
	saveToFile();
}
	
void XSLProject::loadFromFile( const QString & filename ) {
	QFile file(filename);
	
	// Open the file
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		QMessageBox::warning(qApp->activeWindow(), QObject::tr("Project file"), QObject::tr("Cannot read file %1:\n%2.")
                                                        				 		.arg(filename)
																		 		.arg(file.errorString()));
		return;
	}
	
	// Load XML Document
	QString errorStr;
	int errorLine;
	int errorColumn;  
	if (!m_projectDocument.setContent(&file, true, &errorStr, &errorLine, &errorColumn)) {
		QMessageBox::information(qApp->activeWindow(), QObject::tr("Project file"), QObject::tr("Parse error at line %1, column %2:\n%3")
																					.arg(errorLine)
        			                      											.arg(errorColumn)
																					.arg(errorStr));
	    return;
	}  
	
	QDomElement root = m_projectDocument.documentElement();
  
	// Test if Project File
	if(root.tagName() != "XSLProject") {
		QMessageBox::information(qApp->activeWindow(), QObject::tr("Project file"), QObject::tr("The file isn't a XINX Project"));
	    return;
	}
	
	m_fileName = filename;
	
	loadOpenedFile();
	loadWebServicesLink();
	if( projectType() == SERVICES ) {
		refreshWebServices();
	}
}

void XSLProject::saveToFile( const QString & filename ) {
	saveOpenedFile();
	saveWebServicesLink();
	
	if( ! filename.isEmpty() ) m_fileName = filename;
	if( m_fileName.isEmpty() ) return;
	
	static const int IndentSize = 3;

	QFile file( m_fileName );
	
	// Open the file
	if (!file.open(QFile::WriteOnly | QFile::Text)) {
		QMessageBox::warning(qApp->activeWindow(), QObject::tr("Project file"), QObject::tr("Cannot write file %1:\n%2.")
                                                        				 		.arg( m_fileName )
																		 		.arg( file.errorString()) );
		return;
	}
	QTextStream text( &file );
	
	m_projectDocument.save( text, IndentSize );
}
	
QString XSLProject::getValue( const QString & node ) const {
	QDomElement root = m_projectDocument.documentElement();
	QDomElement elt  = root.firstChildElement( node );

	if( ! elt.isNull() ){
		QDomNode node = elt.firstChild();
		while( ! ( node.isNull() || node.isText() ) ) node = node.nextSibling();
		QDomText text = node.toText();
		return text.nodeValue();
	} else
		return QString();
}

void XSLProject::setValue( const QString & node, const QString & value ) {
	QDomElement root = m_projectDocument.documentElement();
	QDomElement elt  = root.firstChildElement( node );
	QDomText    text;

	if( elt.isNull() ){
		elt = m_projectDocument.createElement( node );
		root.appendChild( elt );
	} else {
		QDomNode node = elt.firstChild();
		while( ! ( node.isNull() || node.isText() ) ) node = node.nextSibling();
		text = node.toText();
	}
	
	if( text.isNull() ) {
		text = m_projectDocument.createTextNode( value );
		elt.appendChild( text );
	} else
		text.setData( value );
}

XSLProject::enumProjectType XSLProject::projectType() const {
	if( getValue( "type" ) == "services" ) 
		return SERVICES; 	
	else
		return WEB;
}

void XSLProject::setProjectType( const XSLProject::enumProjectType & value ) {
	switch( value ) {
	case SERVICES:
		setValue( "type", "services" );	
		refreshWebServices();
		break;
	case WEB:
		setValue( "type", "web" );	
		qDeleteAll( m_webServices );
		m_webServices.clear();
		delete m_webServicesModel; m_webServicesModel = NULL;
		break;
	default:
		setValue( "type", "default" );	
	}
}

XSLProject::enumProjectVersion XSLProject::projectVersion() const {
	bool ok;
	int version = getValue( "version" ).toInt( &ok );
	
	if( ok )
		return (enumProjectVersion)version;
	else
		return GCE120;
}

void XSLProject::setProjectVersion( const XSLProject::enumProjectVersion & value ) {
	setValue( "version", QString::number( (int)value ) );
}	

QString XSLProject::projectName() const {
	return getValue( "name" );	
}

void XSLProject::setProjectName( const QString & value ) {
	setValue( "name", value );	
}
	
QString XSLProject::defaultLang() const {
	return getValue( "lang" );
}

void XSLProject::setDefaultLang( const QString & value ) {
	setValue( "lang", value );
}
	
QString XSLProject::defaultNav() const {
	return getValue( "nav" );
}

void XSLProject::setDefaultNav( const QString & value ) {
	setValue( "nav", value );
}
	
QString XSLProject::projectPath() const {
	QString path = getValue( "project" );
	if( path[ path.length() - 1 ] != '/' ) 
		path = path + '/';
	return path;
}

void XSLProject::setProjectPath( const QString & value ) {
	setValue( "project", value );
}
	
QString XSLProject::specifPath() const {
	QString path = getValue( "specifique" );
	if( path[ path.length() - 1 ] != '/' ) 
		path = path + '/';
	return path;
}

void XSLProject::setSpecifPath( const QString & value ) {
	setValue( "specifique", value );
}
	
QString XSLProject::specifPrefix() const {
	return getValue( "prefix" );
}
	
void XSLProject::setSpecifPrefix( const QString & value ) {
	setValue( "prefix", value );
}

QString	XSLProject::standardConfigurationFile() const {
	return getValue( "standard" );
}

void XSLProject::setStandardConfigurationFile( const QString & value ) {
	return setValue( "standard", value );
}
	
QString	XSLProject::specifiqueConfigurationFile() const {
	return getValue( "specifique" );
}
	
void XSLProject::setSpecifiqueConfigurationFile( const QString & value ) {
	return setValue( "specifique", value );
}

void XSLProject::loadOpenedFile() {
	QDomElement root = m_projectDocument.documentElement();
	QDomElement elt  = root.firstChildElement( "openedElementCount" );
	
	m_openedFile.clear();
	
	if( elt.isNull() ) return;
		
	QDomElement file = elt.firstChildElement( "file" );
	
	while ( ! file.isNull() ) {
		m_openedFile.append( file.firstChild().toText().nodeValue() );	
		
		file = file.nextSiblingElement( "file" );
	}
}

void XSLProject::saveOpenedFile() {
	QDomElement root = m_projectDocument.documentElement();
	QDomElement elt  = root.firstChildElement( "openedElementCount" );
	
	if( ! elt.isNull() )
		root.removeChild( elt );

	elt = m_projectDocument.createElement( "openedElementCount" );
	root.appendChild( elt );
	
	foreach(QString filename, m_openedFile) {
		QDomElement file = m_projectDocument.createElement( "file" );
		elt.appendChild( file );
		
		QDomText text = m_projectDocument.createTextNode( filename );
		file.appendChild( text );
	}
}

void XSLProject::loadWebServicesLink() {
	QDomElement root = m_projectDocument.documentElement();
	QDomElement elt  = root.firstChildElement( "webServiceLink" );
	
	m_webServiceLink.clear();
	
	if( elt.isNull() ) return;
		
	QDomElement file = elt.firstChildElement( "link" );
	
	while ( ! file.isNull() ) {
		m_webServiceLink.append( file.firstChild().toText().nodeValue() );	
		
		file = file.nextSiblingElement( "link" );
	}
}

void XSLProject::saveWebServicesLink() {
	QDomElement root = m_projectDocument.documentElement();
	QDomElement elt  = root.firstChildElement( "webServiceLink" );
	
	if( ! elt.isNull() )
		root.removeChild( elt );

	elt = m_projectDocument.createElement( "webServiceLink" );
	root.appendChild( elt );
	
	foreach(QString filename, m_webServiceLink) {
		QDomElement file = m_projectDocument.createElement( "link" );
		elt.appendChild( file );
		
		QDomText text = m_projectDocument.createTextNode( filename );
		file.appendChild( text );
	}
}

const QString & XSLProject::fileName() const {
	return m_fileName;
}

void XSLProject::refreshWebServices() {
	qDeleteAll( m_webServices );
	m_webServices.clear();
	
	foreach( QString link, m_webServiceLink ) {
		m_webServices.append( new WebServices( link, this ) );
	}
		
	if( ! m_webServicesModel ) {
		m_webServicesModel = new WebServicesModel( this, this );
	} else
		m_webServicesModel->reset();
}

