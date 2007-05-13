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
#include <QIODevice>
#include <QHttp>
#include <QMessageBox>
#include <QApplication>
#include <QUrl>
#include <QIcon>
#include <QDomElement>

#include <assert.h>

#include "webservices.h"
#include "xslproject.h"
#include "wsdl.h"
#include "connectionwebservicesdialogimpl.h"

WebServices::WebServices( const QString & link, QObject * parent ) : QObject( parent ), m_link ( link ), m_requestId( -1 ) {
	m_response = new QBuffer( this );
//	m_response = new QFile( "c:\\temp.services" , this );
	m_http = new QHttp( this );
}

void WebServices::loadFromElement( const QDomElement & element ) {
	m_list.clear();
	
	m_wsdl = WSDL( element );
	
	foreach( WSDLService service, m_wsdl.services() ) {
		QString tnsBinding = service.port().binding().remove( "tns:" );
		WSDLBinding binding = m_wsdl.bindings()[ tnsBinding ];
		QString tnsType = binding.type().remove( "tns:" );
		WSDLPortType portType = m_wsdl.portTypes()[ tnsType ];
		
		foreach( WSDLOperation operation, portType.operations() ) {
			Operation wsOperation ( operation.name() );
			
			QString tnsInputMessage = operation.inputMessage().remove ( "tns:" );
			WSDLMessage inputMessage = m_wsdl.messages()[ tnsInputMessage ];
			foreach( WSDLPart part, inputMessage.parts() ) {
				Parameter param( part.name(), part.type() );
				wsOperation.m_inputParam.append( param );
			}
			
			
			QString tnsOutputMessage = operation.outputMessage().remove ( "tns:" );
			WSDLMessage outputMessage = m_wsdl.messages()[ tnsOutputMessage ];
			foreach( WSDLPart part, outputMessage.parts() ) {
				Parameter param( part.name(), part.type() );
				wsOperation.m_outputParam.append( param );
			}
			
			m_list.append( wsOperation );
		}
	}
	
	emit updated();
}

void WebServices::httpSoapRequestFinished ( int id, bool error ) {
	if( ( id == m_requestId ) && ( ! error ) ) {
		m_response->seek( 0 );

		
		QDomDocument document;
		QString errorStr;
		int errorLine;
		int errorColumn;  
		if (!document.setContent( m_response, true, &errorStr, &errorLine, &errorColumn)) {
			QMessageBox::information(qApp->activeWindow(), QObject::tr("Invok WebServices file"), QObject::tr("Parse error at line %1, column %2:\n%3")
																						.arg(errorLine)
	        			                      											.arg(errorColumn)
																						.arg(errorStr));
		    return;
		}  
		
		emit soapResponse( m_query, document.toString(), QString(), QString() );
		
		m_response->close();
		m_http->disconnect();
	}
}

void WebServices::askWSDL( QWidget * parent ) {
	QUrl wsdlUrl( m_link );
	QBuffer buffer;
	buffer.open( QIODevice::ReadWrite );

	ConnectionWebServicesDialogImpl dlg( parent );
	dlg.setHost( wsdlUrl.host(), wsdlUrl.port() );
	if( dlg.get( wsdlUrl.path() + "?WSDL", &buffer ) ) {
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

/* 
In:
<soap:Envelope xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns="urn:GCE" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">
	<soap:Body soap:encodingStyle="http://schema.xmlsoap.org/soap/encoding/">
		<ns:retrieve>
			<String_1 xsi:type="xsd:string"></String1>
		</ns:retrieve>
	</soap:Body>
</soap:Envelope>

Out:
<?xml version="1.0" encoding="UTF-8"?>
<env:Envelope xmlns:env="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns0="urn:GCE" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">
	<env:Body>
		<env:Fault>
			<faultcode>env:Server</faultcode>
			<faultstring>Internal Server Error (...)</faultstring>
		</env:Fault>
	</env:Body>
</env:Envelope>

Out:2
<?xml version="1.0" encoding="UTF-8"?>
<env:Envelope xmlns:env="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns0="urn:GCE" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">
	<env:Body>
		<ns0:retrieveReponse env:encoding...>
			<result xsi:type="xsd:string">
			</result>
		</ns0:retrieveReponse>
	</env:Body>
</env:Envelope>
*/

void WebServices::call( Operation op, const QStringList & param ) {
	QDomDocument document;
	QDomElement envelope = document.createElementNS( "http://schemas.xmlsoap.org/soap/envelope/", "soap:Envelope" );
	document.appendChild( envelope );
	
//	envelope.setAttribute( "xmlns:soap", "http://schemas.xmlsoap.org/soap/envelope/" );
//	envelope.setAttribute( "xmlns:ns",   "urn:GCE" );
	envelope.setAttribute( "xmlns:xsd", "http://www.w3.org/2001/XMLSchema" );
//	envelope.setAttribute( "xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance" );
	envelope.setAttribute( "xmlns:soapenc", "http://schemas.xmlsoap.org/soap/encoding/" );
	
	QDomElement body = document.createElement( "soap:Body" );
	envelope.appendChild( body );
	
	body.setAttribute( "soap:encodingStyle", "http://schemas.xmlsoap.org/soap/encoding/" );

	QDomElement operation = document.createElementNS( "urn:GCE", QString( "ns:%1" ).arg( op.name() ) );
	body.appendChild( operation );
	
	for( int i = 0; i < op.inputParam().count(); i++ ) {
		QDomElement param_elt = document.createElement( op.inputParam()[i].paramString() );
		operation.appendChild( param_elt );
		
		param_elt.setAttributeNS( "http://www.w3.org/2001/XMLSchema-instance", "xsi:type", op.inputParam()[i].paramType() );
		
		if( i < param.count() ) {
			m_query += op.inputParam()[i].paramString() + "=\n" + param[i] + "\n";

			QString query = param[i].simplified();
			QDomText text = document.createTextNode( query );
			param_elt.appendChild( text );
		}
	}
	delete m_response;
	m_response = new QBuffer();
	m_response->open( QIODevice::ReadWrite );

	QUrl queryUrl( m_wsdl.services()[0].port().addressLocation() );
	m_http->setHost( queryUrl.host(), queryUrl.port() );
	connect( m_http, SIGNAL( requestFinished(int,bool) ), this, SLOT(httpSoapRequestFinished(int,bool)) );

	m_query = document.toString();
	QByteArray buffer = document.toString().toUtf8();
	buffer.truncate( buffer.size() - 1 );
	
	QHttpRequestHeader header( "POST", queryUrl.path() );
	header.setValue( "Host", queryUrl.host() );
	header.setValue( "Connection", "Keep-Alive" );
	header.setContentLength( buffer.size() );
	header.setContentType( "text/xml" );

	m_requestId = m_http->request( header, buffer, m_response );
//	m_requestId = m_http->post( queryUrl.path(), buffer, m_response ); 
}


/* WebServicesModel */

WebServicesModel::WebServicesModel( QObject *parent, WebServicesList * list ) : QAbstractItemModel( parent ), m_list( list ) {
	assert( list != NULL );
	
	foreach( WebServices * ws, *m_list ) 
		connect( ws, SIGNAL(updated()), this, SIGNAL(layoutChanged()) );
}

WebServicesModel::~WebServicesModel() {
	
}
	
QVariant WebServicesModel::data(const QModelIndex &index, int role) const {
	if (!index.isValid()) return QVariant();

	if( index.internalPointer() ) {
		if( role == Qt::DecorationRole && index.column() == 0 ) {
			return QIcon(":/CVpublic_slot.png");
		} 	
		if( role == Qt::DisplayRole && index.column() == 0 ) {
			WebServices * services = static_cast<WebServices*>( index.internalPointer() );
			Operation operation = services->operations().at( index.row() );
			return operation.name();
		}
	} else {
		if( role == Qt::DecorationRole && index.column() == 0 ) {
			return QIcon(":/CVstruct.png");
		} 
		if( role == Qt::DisplayRole && index.column() == 0 ) {
			WebServices * services = m_list->at( index.row() );
			return services->name();
		}
	}
	
	return QVariant();
}
	
Qt::ItemFlags WebServicesModel::flags(const QModelIndex &index) const {
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;	
}
	
QModelIndex WebServicesModel::index(int row, int column, const QModelIndex &parent) const {
	if( column > 0 ) return QModelIndex();
		
	if( parent.isValid() ) {
		return createIndex( row, 0, m_list->at( parent.row() ) );
	} else
		return createIndex( row, 0 );
}
	
QModelIndex WebServicesModel::parent(const QModelIndex &index) const {
	if ( !index.isValid() )
		return QModelIndex();

	if( index.internalPointer() ) {
		WebServices * services = static_cast<WebServices*>(index.internalPointer());
		return createIndex( m_list->indexOf( services ), 0 );	
	} else
		return QModelIndex();

}
	
int WebServicesModel::rowCount(const QModelIndex &parent) const {
	if( parent.isValid() ) {
		if( ! parent.internalPointer() )
			return m_list->at( parent.row() )->operations().count();
		else
			return 0;		
	} else
		return m_list->count();
}

int WebServicesModel::columnCount(const QModelIndex &parent) const {
	Q_UNUSED(parent);
	
	return 1;
}

void WebServicesModel::reset( WebServicesList * list ) {
	assert( ( m_list != NULL ) || ( list != NULL ) );
	if( list ) m_list = list;
		
	foreach( WebServices * ws, *m_list ) 
		connect( ws, SIGNAL(updated()), this, SIGNAL(layoutChanged()) );
	
	QAbstractItemModel::reset();
}
