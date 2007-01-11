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

XSLProject::XSLProject() {
	QDomElement root = m_projectDocument.createElement( "XSLProject" );
	m_projectDocument.appendChild( root );
}

XSLProject::XSLProject( const XSLProject & object ) {
	m_fileName =  object.m_fileName;
	m_projectDocument = object.m_projectDocument;
}

XSLProject::XSLProject( const QString & project ) {
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
		QMessageBox::information(qApp->activeWindow(), QObject::tr("Project file"), QObject::tr("The file isn't an XSL Project"));
	    return;
	}
	
	m_fileName = filename;
}

void XSLProject::saveToFile( const QString & filename ) {
	static const int IndentSize = 3;

	QFile file(filename);
	
	// Open the file
	if (!file.open(QFile::WriteOnly | QFile::Text)) {
		QMessageBox::warning(qApp->activeWindow(), QObject::tr("Project file"), QObject::tr("Cannot write file %1:\n%2.")
                                                        				 		.arg(filename)
																		 		.arg(file.errorString()));
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
	return getValue( "project" );
}

void XSLProject::setProjectPath( const QString & value ) {
	setValue( "project", value );
}
	
QString XSLProject::specifPath() const {
	return getValue( "specifique" );
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
	
const QString & XSLProject::fileName() const {
	return m_fileName;
}

