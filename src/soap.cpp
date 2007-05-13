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

Envelop::Envelop( const QString & operation ) {
	QString operationNamespace = "urn:GCE";

	QDomElement envelope = m_envelop.createElement( "soap:Envelope" );
	m_envelop.appendChild( envelope );
	
	envelope.setAttribute( "xmlns:soap", "http://schemas.xmlsoap.org/soap/envelope/" );
	envelope.setAttribute( "xmlns:ns",   operationNamespace );
	envelope.setAttribute( "xmlns:xsd", "http://www.w3.org/2001/XMLSchema" );
	envelope.setAttribute( "xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance" );
	envelope.setAttribute( "xmlns:soapenc", "http://schemas.xmlsoap.org/soap/encoding/" );
	
	QDomElement body = m_envelop.createElement( "soap:Body" );
	envelope.appendChild( body );
	
	body.setAttribute( "soap:encodingStyle", "http://schemas.xmlsoap.org/soap/encoding/" );

	m_operation = m_envelop.createElement( QString( "ns:%1" ).arg( operation ) );
	body.appendChild( m_operation );

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

QString Envelop::toString() {
	return m_envelop.toString();
}

