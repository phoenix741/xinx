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
 
#include "webservices.h"

#include <QtGui>
#include <QtXml>
#include <QHttp>

WebServices::WebServices( const QString & link, QObject * parent ) : QObject( parent ), m_requestId( -1 ), m_http( NULL ), m_response( NULL ) {
	m_response = new QBuffer( this );
//	m_response = new QFile( "c:\\temp.services" , this );
	m_http = new QHttp( this );

	askWSDL( link );
}

void WebServices::loadFromElement( const QDomElement & element ) {
	// /definitions
	QDomElement xmlDefinition = element; //element.firstChild( "definitions" );
	m_name = xmlDefinition.attribute( "name" );

	// /definitions/binding
	QDomNodeList xmlBindings = xmlDefinition.elementsByTagName( "binding" );
	for( int i = 0; i < xmlBindings.size(); i++ ) {
		QDomElement xmlBinding = xmlBindings.at( i ).toElement();
			
		// /definitions/binding[@name="WSCRUDManagerBinding"]
		if( xmlBinding.attribute( "name" ) == ( m_name + "Binding" ) ) {
			QDomNodeList xmlOperations = xmlBinding.elementsByTagName( "operation" );
				
			for( int j = 0; j < xmlOperations.size(); j++ ) {
				// /definitions/binding[@name="WSCRUDManagerBinding"]/operation
				QDomElement xmlOperation = xmlOperations.at( j ).toElement();
				QDomElement xmlSoapAction = xmlOperation.firstChildElement( "operation" );
				
				Operation operation( xmlOperation.attribute( "name" ), xmlSoapAction.attribute( "soapAction" ) );
				
				// /definitions/message
				QDomNodeList xmlMessages = xmlDefinition.elementsByTagName( "message" );
				for( int k = 0; k < xmlMessages.size(); k++ ) {
					QDomElement xmlMessage = xmlMessages.at( k ).toElement();
					if( xmlMessage.attribute( "name" ) == operation.name() + "Input" ) {
						QDomNodeList xmlParts = xmlMessage.elementsByTagName( "part" );
						
						for( int l = 0; l < xmlParts.size(); l++ ){
							QDomElement xmlPart = xmlParts.at( l ).toElement();
							operation.m_inputParam.append( xmlPart.attribute( "name" ) );
						}
					}
					if( xmlMessage.attribute( "name" ) == operation.name() + "Output" ) {
						QDomNodeList xmlParts = xmlMessage.elementsByTagName( "part" );
						
						for( int l = 0; l < xmlParts.size(); l++ ){
							QDomElement xmlPart = xmlParts.at( l ).toElement();
							operation.m_outputParam.append( xmlPart.attribute( "name" ) );
						}
					}
				}
				
				m_list.append( operation );
			}
				
			break;
		}
	}
}

void WebServices::httpRequestFinished ( int id, bool error ) {
	if( ( id == m_requestId ) && ( ! error ) ) {
		m_response->seek( 0 );
		
		QDomDocument document;
		QString errorStr;
		int errorLine;
		int errorColumn;  
		if (!document.setContent( m_response, true, &errorStr, &errorLine, &errorColumn)) {
			QMessageBox::information(qApp->activeWindow(), QObject::tr("WSDL WebServices file"), QObject::tr("Parse error at line %1, column %2:\n%3")
																						.arg(errorLine)
	        			                      											.arg(errorColumn)
																						.arg(errorStr));
		    return;
		}  
		
		loadFromElement( document.documentElement() );
	}
}

void WebServices::askWSDL( const QString & link ) {
	m_response->open( QIODevice::ReadWrite );

	QUrl wsdlUrl( link );
	m_http->setHost( wsdlUrl.host(), wsdlUrl.port() );
	connect( m_http, SIGNAL( requestFinished(int,bool) ), this, SLOT(httpRequestFinished(int,bool)) );

	m_requestId = m_http->get( wsdlUrl.path() + "?WSDL", m_response ); 
}
