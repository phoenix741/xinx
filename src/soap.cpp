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
 
#include "soap.h"

#include <assert.h>

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

Envelop::Envelop( const QString & encodingStyle, const QString & namespaceString, const QString & operation ) {
	QDomElement envelope = m_envelop.createElement( "soap:Envelope" );
	m_envelop.appendChild( envelope );
	
	envelope.setAttribute( "xmlns:soap", "http://schemas.xmlsoap.org/soap/envelope/" );
	envelope.setAttribute( "xmlns:ns",   namespaceString );
	envelope.setAttribute( "xmlns:xsd", "http://www.w3.org/2001/XMLSchema" );
	envelope.setAttribute( "xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance" );
	envelope.setAttribute( "xmlns:soapenc", encodingStyle );
	
	QDomElement body = m_envelop.createElement( "soap:Body" );
	envelope.appendChild( body );
	
	body.setAttribute( "soap:encodingStyle", encodingStyle );

	m_operation = m_envelop.createElement( QString( "ns:%1" ).arg( operation ) );
	body.appendChild( m_operation );

}

Envelop::Envelop( const QString & element, const QString & response ) {
	m_envelop.setContent( element, true );
	
	QDomElement envelop = m_envelop.documentElement();
	QDomElement body = envelop.firstChildElement( "Body" );
	m_operation = body.firstChildElement( response );
	
	QDomElement fault = body.firstChildElement( "Fault" );
	QDomElement faultCode = fault.firstChildElement( "faultcode" );
	QDomElement faultString = fault.firstChildElement( "faultstring" );

	QDomNode text = faultCode.firstChild();
	while( ! text.isNull() ) {
		if( text.isText() ) {
			m_errorCode += text.toText().data();
		}
		text = text.nextSibling();
	}
	text = faultString.firstChild();
	while( ! text.isNull() ) {
		if( text.isText() ) {
			m_errorString += text.toText().data();
		}
		text = text.nextSibling();
	}
}

Envelop::~Envelop() {

}


void Envelop::setParam( const QString & name, const QString & type, const QString & value ) {
	QDomElement param_elt = m_envelop.createElement( name );
	m_operation.appendChild( param_elt );
			
	param_elt.setAttribute( "xsi:type", type );

	QString query = value.simplified();
	QDomText text = m_envelop.createTextNode( query );
	param_elt.appendChild( text );
}

QStringList Envelop::getParams() {
	QStringList result;
	
	QDomElement param = m_operation.firstChildElement();
	while( ! param.isNull() ) {
		result << param.tagName();
		param = param.nextSiblingElement();
	}
	
	return result;
}

QPair<QString,QString> Envelop::getParam( const QString & name ) {
	QDomElement param = m_operation.firstChildElement();
	while( ! param.isNull() ) {
		if( param.tagName() == name ) {
			QString type = param.attribute( "type" );
			QString value;
			
			QDomNode text = param.firstChild();
			while( ! text.isNull() ) {
				if( text.isText() ) {
					value += text.toText().data();
				}
				text = text.nextSibling();
			}
		
			return qMakePair( value, type );
		}
		param = param.nextSiblingElement();
	}
	return qMakePair( QString(), QString() );
}

QString Envelop::getErrorCode() {
	return m_errorCode;
}

QString Envelop::getErrorString() {
	return m_errorString;
}

QString Envelop::toString() {
	return m_envelop.toString();
}

