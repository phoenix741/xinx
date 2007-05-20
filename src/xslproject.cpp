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
 
#include <QFile>
#include <QMessageBox>
#include <QApplication>
#include <QTextStream>
#include "xslproject.h"
#include "webservices.h"

XSLProject::XSLProject() : QObject() {
	QDomElement root = m_projectDocument.createElement( "XSLProject" );
	m_projectDocument.appendChild( root );
	
	m_rootSession = m_sessionDocument.createElement( "Session" );
	m_sessionDocument.appendChild( m_rootSession );
	
	m_sessionNode = m_sessionDocument.createElement( "Opened" );
	m_rootSession.appendChild( m_sessionNode );
}

XSLProject::XSLProject( const XSLProject & object ) : QObject() {
	m_fileName =  object.m_fileName;
	m_projectDocument = object.m_projectDocument;
	m_sessionDocument = object.m_sessionDocument;
	m_sessionNode     = object.m_sessionNode;
}

XSLProject::XSLProject( const QString & project ) : QObject() {
	loadFromFile( project );
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
	loadWebServicesLink();
	
	loadSessionFile( m_fileName + ".session" );
}

void XSLProject::saveToFile( const QString & filename ) {
	if( ! filename.isEmpty() ) m_fileName = filename;
	if( m_fileName.isEmpty() ) return;
	
	saveSessionFile( m_fileName + ".session" );
	
	saveWebServicesLink();
	
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
		break;
	case WEB:
		setValue( "type", "web" );	
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
	if( ! path.isEmpty() ) {
		if( path[ path.length() - 1 ] != '/' ) 
			path = path + '/';
	}
	return path;
}

void XSLProject::setProjectPath( const QString & value ) {
	setValue( "project", value );
}
	
QString XSLProject::specifPath() const {
	QString path = getValue( "specifique" );
	if( ! path.isEmpty() ) {
		if( path[ path.length() - 1 ] != '/' ) 
			path = path + '/';
	}
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

void XSLProject::loadSessionFile( const QString fileName ) {
	QFile file( fileName );
	
	// Open the file
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		QDomElement rootSession = m_sessionDocument.createElement( "Session" );
		m_sessionDocument.appendChild( rootSession );
	
		QDomElement sessionNode = m_sessionDocument.createElement( "Opened" );
		rootSession.appendChild( sessionNode );
	} else {
		// Load XML Document
		QString errorStr;
		int errorLine;
		int errorColumn;  
		if (!m_sessionDocument.setContent(&file, true, &errorStr, &errorLine, &errorColumn)) {
			QMessageBox::information(qApp->activeWindow(), QObject::tr("Session file"), QObject::tr("Parse error at line %1, column %2:\n%3")
																						.arg(errorLine)
	        			                      											.arg(errorColumn)
																						.arg(errorStr));
		    return;
		}  
	}
		
	m_rootSession = m_sessionDocument.documentElement();
  
	// Test if Project File
	if( m_rootSession.tagName() != "Session" ) {
		QMessageBox::information(qApp->activeWindow(), QObject::tr("Session file"), QObject::tr("The file isn't a XINX Project Session file"));
	    return;
	}
	
	m_sessionNode = m_rootSession.firstChildElement( "Opened" );
	
	m_lastOpenedFile.clear();
	QDomElement lastOpenedFile = m_rootSession.firstChildElement( "lastOpenedFile" );
	if( ! lastOpenedFile.isNull() ) {
		QDomElement file = lastOpenedFile.firstChildElement( "file" );
		while( ! file.isNull() ) {
			m_lastOpenedFile.append( file.attribute( "name" ) );
			
			file = file.nextSiblingElement( "file" );
		}
	}
	
}

void XSLProject::saveSessionFile( const QString fileName ) {
	QDomElement lastOpenedFile = m_rootSession.firstChildElement( "lastOpenedFile" );

	m_rootSession.removeChild( lastOpenedFile );
	lastOpenedFile = m_sessionDocument.createElement( "lastOpenedFile" );
	m_rootSession.appendChild( lastOpenedFile );	

	foreach( QString name, m_lastOpenedFile ) {
		QDomElement file = m_sessionDocument.createElement( "file" );
		file.setAttribute( "name", name );
		lastOpenedFile.appendChild( file );
	}
	
	static const int IndentSize = 3;
	QFile file( fileName );
	
	// Open the file
	if (!file.open(QFile::WriteOnly | QFile::Text)) {
		QMessageBox::warning(qApp->activeWindow(), QObject::tr("Session file"), QObject::tr("Cannot write file %1:\n%2.")
                                                        				 		.arg( m_fileName )
																		 		.arg( file.errorString()) );
		return;
	}
	QTextStream text( &file );
	
	m_sessionDocument.save( text, IndentSize );
}

void XSLProject::clearSessionNode() {
	m_rootSession.removeChild( m_sessionNode );
	m_sessionNode = m_sessionDocument.createElement( "Opened" );
	m_rootSession.appendChild( m_sessionNode );
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


