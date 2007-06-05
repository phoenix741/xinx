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
 
#include <QBuffer>
#include <QFile>
#include <QIODevice>
#include <QHttp>
#include <QMessageBox>
#include <QApplication>
#include <QUrl>
#include <QIcon>
#include <QDomElement>
#include <QPair>

#include <assert.h>

#include "webservices.h"
#include "xslproject.h"
#include "wsdl.h"
#include "soap.h"
#include "connectionwebservicesdialogimpl.h"

WebServices::WebServices( const QString & link, QObject * parent ) : QObject( parent ), m_link ( link ) {
}

void WebServices::loadFromElement( const QDomElement & element ) {
	qDeleteAll( m_list );
	m_list.clear();
	
	m_wsdl = WSDL( element );
	
	foreach( WSDLService service, m_wsdl.services() ) {
		QString tnsBinding = service.port().binding();
		tnsBinding = tnsBinding.mid( tnsBinding.indexOf( ":" ) + 1 );
		
		WSDLBinding binding = m_wsdl.bindings()[ tnsBinding ];
		QString tnsType = binding.type();
		tnsType = tnsType.mid( tnsType.indexOf( ":" ) + 1 );
		
		WSDLPortType portType = m_wsdl.portTypes()[ tnsType ];
		
		foreach( WSDLOperation operation, portType.operations() ) {
			Operation * wsOperation = new Operation( operation.name() );
			
			foreach( WSDLOperation bindingOperation, binding.operations() ) {
				if( bindingOperation.name() == operation.name() ) {
					wsOperation->m_encodingStyle = bindingOperation.inputEncodingStyle();
					wsOperation->m_namespaceString = bindingOperation.inputNamespace();
				}
			}
			
			QString tnsInputMessage = operation.inputMessage();
			tnsInputMessage = tnsInputMessage.mid( tnsInputMessage.indexOf( ":" ) + 1 );
			
			WSDLMessage inputMessage = m_wsdl.messages()[ tnsInputMessage ];
			foreach( WSDLPart part, inputMessage.parts() ) {
				Parameter * param = new Parameter( part.name(), part.type() );
				wsOperation->m_inputParam.append( param );
			}
			
			
			QString tnsOutputMessage = operation.outputMessage();
			tnsOutputMessage = tnsOutputMessage.mid( tnsOutputMessage.indexOf( ":" ) + 1 );

			WSDLMessage outputMessage = m_wsdl.messages()[ tnsOutputMessage ];
			foreach( WSDLPart part, outputMessage.parts() ) {
				Parameter * param = new Parameter( part.name(), part.type() );
				wsOperation->m_outputParam.append( param );
			}
			
			m_list.append( wsOperation );
		}
	}
	
	emit updated();
}

WebServices::~WebServices() {
	qDeleteAll( m_list );
}


typedef
	QPair<QString,QString> ParamStr;

void WebServices::askWSDL( QWidget * parent ) {
	QUrl wsdlUrl( m_link );
	QBuffer buffer;
//	QFile buffer( "c:\\temp.wsdl" );
	buffer.open( QIODevice::ReadWrite );

	ConnectionWebServicesDialogImpl dlg( parent );
	if( wsdlUrl.port() >= 0 )
		dlg.setHost( wsdlUrl.host(), wsdlUrl.port() );
	else
		dlg.setHost( wsdlUrl.host() );
	QString query = wsdlUrl.path();
	if( wsdlUrl.hasQuery() ) {
		query += "?";
		foreach( ParamStr param, wsdlUrl.queryItems() ) {
			query += param.first;
			if( !param.second.isEmpty() )
				query += "=" + param.second;
		}
	}
	if( dlg.get( query, &buffer ) ) {
		buffer.seek( 0 );
		
		QDomDocument document;
		QString errorStr;
		int errorLine;
		int errorColumn;  
		if (!document.setContent( &buffer, true, &errorStr, &errorLine, &errorColumn)) {
			QMessageBox::information(qApp->activeWindow(), QObject::tr("WSDL WebServices file"), QObject::tr("Parse error at line %1, column %2:\n%3")
																						.arg(errorLine)
	        			                      											.arg(errorColumn)
 																						.arg(errorStr));
		    return;
		}  
		
		loadFromElement( document.documentElement() );
	}
}

void WebServices::call( Operation * op, const QHash<QString,QString> & param ) {
	Envelop soapEnvelop( op->encodingStyle(), op->namespaceString() , op->name() );
	QString query;
	
	for( int i = 0; i < op->inputParam().count(); i++ ) {
		soapEnvelop.setParam( op->inputParam()[i]->paramString(), op->inputParam()[i]->paramType(), param[ op->inputParam()[i]->paramString() ] );
		query += op->inputParam()[i]->paramString() + "=\n" + param[ op->inputParam()[i]->paramString() ] + "\n";
	}

	QUrl queryUrl( m_wsdl.services()[0].port().addressLocation() );
	QBuffer obuffer;
	obuffer.open( QIODevice::ReadWrite );

//	query = soapEnvelop.toString();
	QByteArray ibuffer = soapEnvelop.toString().toUtf8();
	ibuffer.truncate( ibuffer.size() - 1 );

	QHttpRequestHeader header( "POST", queryUrl.path() );
	header.setValue( "Host", queryUrl.host() );
	header.setValue( "Connection", "Keep-Alive" );
	header.setContentLength( ibuffer.size() );
	header.setContentType( "text/xml" );

	ConnectionWebServicesDialogImpl dlg;
	dlg.setHost( queryUrl.host(), queryUrl.port() );
	if( dlg.request( &header, &ibuffer, &obuffer ) ) {
		obuffer.seek( 0 );

		QDomDocument document;
		QString errorStr;
		int errorLine;
		int errorColumn;  
		if (!document.setContent( &obuffer, true, &errorStr, &errorLine, &errorColumn)) {
			QMessageBox::information(qApp->activeWindow(), QObject::tr("Invok WebServices file"), QObject::tr("Parse error at line %1, column %2:\n%3")
																						.arg(errorLine)
	        			                      											.arg(errorColumn)
																						.arg(errorStr));
		    return;
		}  
		
		emit soapResponse( query, document.toString(), QString(), QString() );
	}
}

/* Operation */

Operation::~Operation() {
	qDeleteAll( m_inputParam );
	qDeleteAll( m_outputParam );
}

