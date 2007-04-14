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
 
#include "editorcompletion.h"

#include <QDomElement>
#include <QFile>
#include <QMessageBox>
#include <QApplication>

CplNodeList * completionNodeList = NULL;

CplNode::CplNode(QObject * parent, const QDomElement & node) : QObject(parent) {
	m_name = node.tagName();
	if( ! node.prefix().isEmpty() )
		m_name = node.prefix() + ":" + m_name;
	QDomNamedNodeMap attr = node.attributes();
	
	for(int i = 0; i < attr.count(); i++) {
		if( attr.item( i ).isAttr() ) 
			m_params.push_back( attr.item( i ).toAttr().name() );
	}
}

void CplNodeList::loadFiles() {
  m_list.clear();

  QFile file(m_filesPath);
  QDomDocument objectFile;
  
  // Open the file
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    QMessageBox::warning(qApp->activeWindow(), tr("Completion"), tr("Cannot read file %1:\n%2.")
                                                        .arg(m_filesPath)
														.arg(file.errorString()));
    return;
  }

  // Load XML Document
  QString errorStr;
  int errorLine;
  int errorColumn;  
  if (!objectFile.setContent(&file, true, &errorStr, &errorLine, &errorColumn)) {
    QMessageBox::information(qApp->activeWindow(), tr("Completion"), tr("Parse error at line %1, column %2:\n%3")
																.arg(errorLine)
                              									.arg(errorColumn)
																.arg(errorStr));
    return;
  }  
  
  QDomElement root = objectFile.documentElement();
  
  // Test if Completion
  if(root.tagName() != "completion") throw ENotCompletionFile(tr("%1 is not auto completion file").arg(m_filesPath));
  	
  // HTML
  QDomElement html = root.firstChildElement("html");

  QDomNode child = html.firstChild();
  while ((! child.isNull()) && (child.isElement())) {
    m_list.append(new CplNode(this, child.toElement()));
    child = child.nextSibling();
  }	

  // XSL
  QDomElement xsl = root.firstChildElement("xsl");

  child = xsl.firstChild();
  while ((! child.isNull()) && (child.isElement())) {
    m_list.append(new CplNode(this, child.toElement()));
    child = child.nextSibling();
  }	
}

CplNodeList::CplNodeList(const QString & name) {
	m_filesPath = name;
}

CplNode* CplNodeList::node(const QString & name) const {
	for(int i = 0; i < count(); i++) {
		if(name == node(i)->name())
			return node(i);
	}
	return NULL;
}


/* Completion */

Completion * completionContents = NULL;


Completion::Completion( const QString & name ) : m_name( name ) {
	load();
}

Completion::~Completion() {
	qDeleteAll( m_xmlBalises );
}

void Completion::load() {
	QFile file( m_name );
	QDomDocument objectFile;
  
	// Open the file
	if ( !file.open( QFile::ReadOnly | QFile::Text ) ) {
		QMessageBox::warning(
			qApp->activeWindow(), 
			QApplication::translate("Completion", "Completion"), 
			QApplication::translate("Completion", "Cannot read file %1:\n%2.").arg( m_name ).arg( file.errorString() ) );
	    return;
	}

	// Load XML Document
	QString errorStr;
	int errorLine;
	int errorColumn;  
	if ( !objectFile.setContent(&file, false, &errorStr, &errorLine, &errorColumn) ) {
		QMessageBox::information(
			qApp->activeWindow(), 
			QApplication::translate("Completion", "Completion"), 
			QApplication::translate("Completion", "Parse error at line %1, column %2:\n%3")
				.arg(errorLine)
				.arg(errorColumn)
				.arg(errorStr) );
		return;
	}  
  
	QDomElement root = objectFile.documentElement();
  
	// Test if Completion
	if( root.tagName() != "completion" ) 
		throw ENotCompletionFile( QApplication::translate("Completion", "%1 is not auto completion file").arg( m_name ) );
  	
	// HTML
	QDomElement xml = root.firstChildElement( "xml" );
	if( ! xml.isNull() ) {
		QDomElement type = xml.firstChildElement( "type" );
		while( !type.isNull() ) {
			QString typeName = type.attribute( "name", "other" );
			
			QDomNodeList balises = type.elementsByTagName( "balise" );
			for( int i = 0; i < balises.count(); i++ ) 
				if( balises.at( i ).isElement() ) 
					m_xmlBalises.append( new CompletionXMLBalise( typeName, balises.at( i ).toElement() ) );
				
			
			type = type.nextSiblingElement( "type" );
		};
	}
}

/* CompletionXMLBalise */

CompletionXMLBalise::CompletionXMLBalise( const QString & category, const QDomElement & node ) : m_category( category ) {
	m_name = node.attribute( "name" );
	m_isDefault = node.attribute( "default" ) == "true";
	
	QDomNodeList balises = node.elementsByTagName( "balise" );
	for( int i = 0; i < balises.count(); i++ ) 
		if( balises.at( i ).isElement() ) 
			m_balises.append( new CompletionXMLBalise( category, balises.at( i ).toElement() ) );
	
	QDomNodeList attributes = node.elementsByTagName( "attribute" );
	for( int i = 0; i < attributes.count(); i++ ) 
		if( attributes.at( i ).isElement() ) 
			m_attributes.append( new CompletionXMLAttribute( attributes.at( i ).toElement() ) );
}

CompletionXMLBalise::~CompletionXMLBalise() {
	qDeleteAll( m_attributes );
	qDeleteAll( m_balises );
}

/* CompletionXMLAttribute */

CompletionXMLAttribute::CompletionXMLAttribute( const QDomElement & node ) {
	m_name = node.attribute( "name" );
	m_isDefault = node.attribute( "default" ) == "true";
	m_defaultValue = -1;

	QDomNodeList values = node.elementsByTagName( "value" );
	for( int i = 0; i < values.count(); i++ ) 
		if( values.at( i ).isElement() ) {
			QDomElement value = values.at( i ).toElement();
			
			m_values.append( value.text() );
			
			if( ( m_defaultValue == -1 ) && ( value.attribute( "default" ) == "true" ) ) {
				m_defaultValue = m_values.size() - 1;
			}
		}
}

CompletionXMLAttribute::~CompletionXMLAttribute() {
	
}


