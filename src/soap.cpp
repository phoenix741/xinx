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
#include "wsdl.h"


Envelop::Envelop( WSDL * wsdl ) : m_wsdl( wsdl ) {
	QDomElement envelope = m_envelop.createElementNS( "http://schemas.xmlsoap.org/soap/envelope/", "soap:Envelope" );
	m_envelop.appendChild( envelope );
	
	envelope.setAttribute( "xmlns:soap", "http://schemas.xmlsoap.org/soap/envelope/" );
	envelope.setAttribute( "xmlns:ns",   "urn:GCE" );
	envelope.setAttribute( "xmlns:xsd", "http://www.w3.org/2001/XMLSchema" );
	envelope.setAttribute( "xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance" );
	envelope.setAttribute( "xmlns:soapenc", "http://schemas.xmlsoap.org/soap/encoding/" );
	
	QDomElement body = m_envelop.createElementNS( "http://schemas.xmlsoap.org/soap/envelope/", "soap:Body" );
	envelope.appendChild( body );
	
	body.setAttribute( "soap:encodingStyle", "http://schemas.xmlsoap.org/soap/encoding/" );
}
