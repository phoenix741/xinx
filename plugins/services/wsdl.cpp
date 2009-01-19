/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */
 
#include "wsdl.h"

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
	
	QDomElement inputMsg = element.firstChildElement( "input" );
	QDomElement outputMsg = element.firstChildElement( "output" );
	
	m_inputMessage = inputMsg.attribute( "message" );
	m_outputMessage = outputMsg.attribute( "message" );
	
	QDomElement input = element.firstChildElement( "input" );
	if( ! input.isNull() ) {
		QDomElement inputBody = input.firstChildElement( "body" );
		m_inputEncodingStyle = inputBody.attribute( "encodingStyle" );
		m_inputNamespace = inputBody.attribute( "namespace" );
	}
	
	QDomElement output = element.firstChildElement( "output" );
	if( ! input.isNull() ) {
		QDomElement outputBody = output.firstChildElement( "body" );
		m_outputEncodingStyle = outputBody.attribute( "encodingStyle" );
		m_outputNamespace = outputBody.attribute( "namespace" );
	}
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


