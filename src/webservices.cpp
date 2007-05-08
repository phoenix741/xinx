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

#include "webservices.h"
#include "xslproject.h"

WSDLPart::WSDLPart( const QDomElement & element ) {
	m_name = element.attribute( "name" );
	m_type = element.attribute( "type" );
	m_element = element.attribute( "element" );
}

WSDLMessage::WSDLMessage( const QDomElement & element ) {
	m_name = element.attribute( "name" );
	
	QDomElement childs = element.firstChildElement( "part" );
	while( ! childs.isNull() ) {
		m_parts.append( WSDLPart( childs ) );
		
		childs = childs.nextSiblingElement( "part" );
	}
}

WSDLOperation::WSDLOperation( const QDomElement & element ) {
	m_name = element.attribute( "name" );
	m_parameterOrder = element.attribute( "parameterOrder" );
	
	QDomElement input = element.firstChildElement( "input" );
	QDomElement output = element.firstChildElement( "output" );
	
	m_inputMessage = input.attribute( "message" );
	m_outputMessage = output.attribute( "message" );
}

WSDLBinding::WSDLBinding( const QDomElement & element ) {
	m_name = element.attribute( "name" );
	m_type = element.attribute( "type" );
	
	QDomElement childs = element.firstChildElement( "operation" );
	while( ! childs.isNull() ) {
		m_operations.append( WSDLOperation( childs ) );
		
		childs = childs.nextSiblingElement( "operation" );
	}
}

WSDLPortType::WSDLPortType( const QDomElement & element ) {
	m_name = element.attribute( "name" );
	
	QDomElement childs = element.firstChildElement( "operation" );
	while( ! childs.isNull() ) {
		m_operations.append( WSDLOperation( childs ) );
		
		childs = childs.nextSiblingElement( "operation" );
	}
}

WSDLPort::WSDLPort( const QDomElement & element ) {
	m_name = element.attribute( "name" );
	m_binding = element.attribute( "binding" );
	
	QDomElement address = element.firstChildElement( "address" );
	m_addressLocation = address.attribute( "location" );
}

WSDLService::WSDLService( const QDomElement & element ) {
	m_name = element.attribute( "name" );

	QDomElement port = element.firstChildElement( "port" );
	m_port = WSDLPort( port );
}

WSDL::WSDL( const QDomElement & element ) {
	m_name = element.attribute( "name" );
	
	QDomElement message = element.firstChildElement( "message" );
	while( ! message.isNull() ) {
		WSDLMessage e ( message );
		m_messages[ e.name() ] = e;
		
		message = message.nextSiblingElement( "message" );
	}

	QDomElement portType = element.firstChildElement( "portType" );
	while( ! portType.isNull() ) {
		WSDLPortType e ( portType );
		m_portTypes[ e.name() ] = e;
		
		portType = portType.nextSiblingElement( "portType" );
	}

	QDomElement binding = element.firstChildElement( "binding" );
	while( ! binding.isNull() ) {
		WSDLBinding e ( binding );
		m_bindings[ e.name() ] = e;
		
		binding = binding.nextSiblingElement( "binding" );
	}

	QDomElement service = element.firstChildElement( "service" );
	while( ! service.isNull() ) {
		m_services.append( WSDLService ( service ) );
		
		service = service.nextSiblingElement( "service" );
	}
}


WebServices::WebServices( const QString & link, QObject * parent ) : QObject( parent ), m_requestId( -1 ), m_http( NULL ), m_response( NULL ) {
	m_response = new QBuffer( this );
//	m_response = new QFile( "c:\\temp.services" , this );
	m_http = new QHttp( this );

	askWSDL( link );
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

void WebServices::call( Operation operation, const QStringList & param, QString & errorCode, QString & errorString ) {
	
}


/* WebServicesModel */

WebServicesModel::WebServicesModel( QObject *parent, XSLProject * project ) : QAbstractItemModel( parent ), m_project( project ) {
	for( int i = 0; i < m_project->webServices().count(); i++ ) {
		connect( m_project->webServices().at( i ), SIGNAL(updated()), this, SIGNAL(layoutChanged()) );
	}
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
			WebServices * services = m_project->webServices().at( index.row() );
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
		return createIndex( row, 0, m_project->webServices().at( parent.row() ) );
	} else
		return createIndex( row, 0 );
}
	
QModelIndex WebServicesModel::parent(const QModelIndex &index) const {
	if ( !index.isValid() )
		return QModelIndex();

	if( index.internalPointer() ) {
		WebServices * services = static_cast<WebServices*>(index.internalPointer());
		return createIndex( m_project->webServices().indexOf( services ), 0 );	
	} else
		return QModelIndex();

}
	
int WebServicesModel::rowCount(const QModelIndex &parent) const {
	if( parent.isValid() ) {
		if( ! parent.internalPointer() )
			return m_project->webServices().at( parent.row() )->operations().count();
		else
			return 0;		
	} else
		return m_project->webServices().count();
}

int WebServicesModel::columnCount(const QModelIndex &parent) const {
	Q_UNUSED(parent);
	
	return 1;
}

void WebServicesModel::reset() {
	for( int i = 0; i < m_project->webServices().count(); i++ ) {
		connect( m_project->webServices().at( i ), SIGNAL(updated()), this, SIGNAL(layoutChanged()) );
	}
	
	QAbstractItemModel::reset();
}
